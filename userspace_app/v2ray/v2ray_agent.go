package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"net/http"
	"strconv"
	"strings"
	"sync"

	"github.com/RichardWeiYang/go-iptables/iptables"
	"github.com/robfig/cron"
)

type Conn struct {
	Remote_ip   string
	Remote_port string
	User_id     string
}

type Bandwidth struct {
	input  float64
	output float64
}

var user map[string]Bandwidth
var clear_mux sync.Mutex

type Stat struct {
	Name     string
	Inbound  string
	Outbound string
}

type StatSlice struct {
	Stats []Stat
}

func add_rule(conn Conn) {
	ipt, _ := iptables.New()
	// check whether there is a user chain
	_, err := ipt.List("filter", conn.User_id)

	// create the chain and add it to INPUT chain
	if err != nil {
		err2 := ipt.NewChain("filter", conn.User_id)
		if err2 != nil {
			return
		}
		ipt.Append("filter", "INPUT", "-j", conn.User_id)
	}

	// add the rule to user chain
	ipt.AppendUnique("filter", conn.User_id, "-p", "tcp", "--dport", conn.Remote_port)
	ipt.AppendUnique("filter", conn.User_id, "-p", "tcp", "--sport", conn.Remote_port)
}

// add iptable rul
func add_handler(w http.ResponseWriter, r *http.Request) {
	fmt.Println(r.URL.Path[1:])
	switch r.Method {
	case "POST":
		fmt.Println("POST")
		var conn Conn
		err := json.NewDecoder(r.Body).Decode(&conn)
		if err != nil {
			fmt.Println("body error")
			return
		}
		go add_rule(conn)
		fmt.Println(conn)
	default:
	}
}

func del_rule(conn Conn) {
	ipt, _ := iptables.New()
	// get the statistic of the rule before del
	clear_mux.Lock()
	stat, _ := ipt.Stats("filter", "shyw13")
	// delete the rule
	err := ipt.Delete("filter", conn.User_id, "-p", "tcp", "--dport", conn.Remote_port)
	err = ipt.Delete("filter", conn.User_id, "-p", "tcp", "--sport", conn.Remote_port)
	clear_mux.Unlock()
	if err != nil {
		return
	}

	// if this is the last one, remove the chain
	rules, _ := ipt.List("filter", conn.User_id)
	if len(rules) == 1 {
		ipt.Delete("filter", "INPUT", "-j", conn.User_id)
		ipt.DeleteChain("filter", conn.User_id)
	}

	// process stat now
	// get the user first, create it if not
	if _, ok := user[conn.User_id]; !ok {
		user[conn.User_id] = Bandwidth{0, 0}
	}

	u := user[conn.User_id]

	for _, s := range stat {
		if !strings.Contains(s[9], conn.Remote_port) {
			continue
		}
		val, _ := strconv.ParseFloat(s[1], 64)
		if strings.HasPrefix(s[9], "d") {
			u.output += val
		} else {
			u.input += val
		}
	}
	user[conn.User_id] = u
	fmt.Println(conn.User_id, u.input, u.output)
}

// del iptable rul and calculate the bandwidth
func del_handler(w http.ResponseWriter, r *http.Request) {
	fmt.Println(r.URL.Path[1:])
	switch r.Method {
	case "POST":
		var conn Conn
		err := json.NewDecoder(r.Body).Decode(&conn)
		if err != nil {
			fmt.Println("body error")
			return
		}
		go del_rule(conn)
		fmt.Println("POST")
	default:
	}
}

type RawStat struct {
	user string
	stat [][]string
}

func notify(s *StatSlice) {
	b, err := json.Marshal(s)
	if err != nil {
		fmt.Println("json err:", err)
	}

	http.Post("http://185.92.221.13:80/statistic/update", "application/json; charset=utf-8", bytes.NewReader(b))
}

// calculate user bandwidth and sent to master
func cal_handler() {
	var stats []RawStat
	// get user chains
	ipt, _ := iptables.New()

	chains, _ := ipt.ListChains("filter")
	if len(chains) == 3 {
		goto NOTIFY
	}

	// get statistic for each user chain
	for _, c := range chains[3:] {
		clear_mux.Lock()
		s, err := ipt.Stats("filter", c)
		// clear chains
		ipt.ZeroChain("filter", c)
		clear_mux.Unlock()
		if err == nil {
			stats = append(stats, RawStat{c, s})
		}
	}

	// calculate bandwidth for each user
	for _, s := range stats {
		if _, ok := user[s.user]; !ok {
			user[s.user] = Bandwidth{0, 0}
		}

		u := user[s.user]

		for _, r := range s.stat {
			val, _ := strconv.ParseFloat(r[1], 64)
			if strings.HasPrefix(r[9], "d") {
				u.output += val
			} else {
				u.input += val
			}
		}
		user[s.user] = u
	}

NOTIFY:
	var s StatSlice
	var inbound, outbound float64
	inbound = 0
	outbound = 0
	for k, v := range user {
		s.Stats = append(s.Stats, Stat{k,
			strconv.FormatFloat(v.input/(1024*1024), 'f', 6, 64),
			strconv.FormatFloat(v.output/(1024*1024), 'f', 6, 64)})
		delete(user, k)
		inbound += v.input / (1024 * 1024)
		outbound += v.output / (1024 * 1024)
	}
	s.Stats = append(s.Stats, Stat{Name: "total",
		Inbound:  strconv.FormatFloat(inbound, 'f', 6, 64),
		Outbound: strconv.FormatFloat(outbound, 'f', 6, 64)})

	// notify master
	go notify(&s)
}

func main() {
	user = make(map[string]Bandwidth)
	http.HandleFunc("/add_connection", add_handler)
	http.HandleFunc("/del_connection", del_handler)

	c := cron.New()
	spec := "0 */1 * * * *"
	c.AddFunc(spec, cal_handler)
	c.Start()
	http.ListenAndServe(":8888", nil)

}
