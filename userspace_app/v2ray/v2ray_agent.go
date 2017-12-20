package main

import (
	"strings"
	"sync"
	"strconv"
	    "fmt"
	    "encoding/json"
            "net/http"

	"github.com/RichardWeiYang/go-iptables/iptables"
	)

type Conn struct {
Remote_ip  string
Remote_port string
User_id  string
}

type Bandwidth struct {
	input float64
	output float64
}

var user map[string]Bandwidth
var clear_mux sync.Mutex

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

	for _, s := range(stat) {
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
	user[conn.User_id] := u
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

func notify() {
	return
}

// calculate user bandwidth and sent to master
func cal_handler(w http.ResponseWriter, r *http.Request) {
	// get user chains
	ipt, _ := iptables.New()

	chains, _ := ipt.ListChains("filter")
	if len(chains) == 3 {
		go notify()
		return
	}

	// get statistic for each user chain
	var stats []RawStat
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
	fmt.Println("cal handler", s.user)
	if _, ok := user[s.user]; !ok {
		user[s.user] = Bandwidth{0, 0}
	}

	u := user[s.user]

	for _, r := range(s.stat) {
	fmt.Println("    ", r)
		val, _ := strconv.ParseFloat(r[1], 64)
		fmt.Println(u.output, u.input, val)
		if strings.HasPrefix(r[9], "d") {
			u.output += val
		} else {
			u.input += val
		}
		fmt.Println(u.output, u.input, val)
	}
	user[s.user] = u
	}

	for k, v := range user {
		fmt.Println(k, v.input, v.output)
	}

	// notify master
	go notify()
}

func main() {
	user = make(map[string]Bandwidth)
    http.HandleFunc("/add_connection", add_handler)
    http.HandleFunc("/del_connection", del_handler)
    http.HandleFunc("/cal_bandwidth", cal_handler)
    http.ListenAndServe(":8888", nil)
}
