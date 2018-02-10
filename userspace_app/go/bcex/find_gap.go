package main

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"math"
	"net/http"
	"net/url"
	"os"
	"os/signal"
	"sort"
	"strconv"
	"strings"
	"sync"
	"time"

	. "github.com/RichardWeiYang/bcex/lib"
	"github.com/google/gops/goprocess"
	"github.com/jawher/mow.cli"
	"golang.org/x/sync/semaphore"
)

//var currencypair = []string{"omg_btc", "eos_eth", "neo_usdt", "btc_usdt", "neo_btc", "eos_btc", "eth_usdt", "eos_usdt", "eth_btc", "yoyo_btc", "xmr_eth", "xmr_btc", "xrp_eth", "xrp_btc"}
var currencypair []string

var exchanges []string
var sem *semaphore.Weighted
var mutex = &sync.Mutex{}
var ps []ExPrice
var http_error int

type ExData struct {
	ex      Exchange
	symbols []string
}

var exs map[string]ExData
var cp_ex map[string][]string

type ExPrice struct {
	ex    string
	sell1 Unit
	buy1  Unit
}

func min(a, b int) int {
	if a > b {
		return b
	}
	return a
}

func dump_depth(exname, cp string, depth Depth) {
	fmt.Println("Depth of ", cp, "on ", exname)
	fmt.Println("\tPrice      \tAmount")
	fmt.Println("Asks:")
	for i := min(5, len(depth.Asks)); i >= 1; i-- {
		fmt.Printf("\t%0.8f\t%0.8f\n",
			depth.Asks[len(depth.Asks)-i].Price,
			depth.Asks[len(depth.Asks)-i].Amount)
	}
	fmt.Println("Bids:")
	for i := 0; i < min(5, len(depth.Bids)); i++ {
		fmt.Printf("\t%0.8f\t%0.8f\n",
			depth.Bids[i].Price,
			depth.Bids[i].Amount)
	}
}

func get_depth(exname string, cp *CurrencyPair) {
	ex := exs[exname].ex
	if has_pair(exs[exname].symbols, cp.ToSymbol("_")) {
		depth, err := ex.GetDepth(cp)
		if err == nil && len(depth.Asks) != 0 && len(depth.Bids) != 0 {
			//dump_depth(exname, cp.String(), depth)
			mutex.Lock()
			ps = append(ps, ExPrice{exname,
				depth.Asks[len(depth.Asks)-1], depth.Bids[0]})
			mutex.Unlock()
		} else {
			http_error++
		}
	}
	sem.Release(1)
}

func has_pair(symbols []string, cp string) bool {
	for _, s := range symbols {
		if s == cp {
			return true
		}
	}
	return false
}

type Gap struct {
	symbol             string
	max_gap, total_gap float64
	max_am, total_am   float64
	total_time         time.Duration
	count              int
}

var statistic map[string]Gap

func dump_stat() {
	var result []Gap

	for _, v := range statistic {
		result = append(result, v)
	}
	fmt.Println("### Order in count")
	sort.Slice(result, func(i, j int) bool {
		return result[i].count > result[j].count
	})
	for _, r := range result {
		symbol := strings.Split(r.symbol, ".")
		fmt.Println(symbol[0], symbol[1], symbol[2])
		fmt.Printf("\tMaxGap: %0.8f  AvgGap:  %0.8f\n", r.max_gap, r.total_gap/float64(r.count))
		fmt.Printf("\tMaxAm:  %0.8f  AvgAm:   %0.8f\n", r.max_am, r.total_am/float64(r.count))
		fmt.Println("\tAvgTime:", r.total_time/time.Duration(r.count))
		fmt.Println("\tCount:", r.count)
	}
	fmt.Println("### Order in gap")
	sort.Slice(result, func(i, j int) bool {
		return result[i].total_gap/float64(result[i].count) > result[j].total_gap/float64(result[j].count)
	})
	for _, r := range result {
		symbol := strings.Split(r.symbol, ".")
		fmt.Println(symbol[0], symbol[1], symbol[2])
		fmt.Printf("\tMaxGap: %0.8f  AvgGap:  %0.8f\n", r.max_gap, r.total_gap/float64(r.count))
		fmt.Printf("\tMaxAm:  %0.8f  AvgAm:   %0.8f\n", r.max_am, r.total_am/float64(r.count))
		fmt.Println("\tAvgTime:", r.total_time/time.Duration(r.count))
		fmt.Println("\tCount:", r.count)
	}
	fmt.Println("### Order in amount")
	sort.Slice(result, func(i, j int) bool {
		return result[i].total_am/float64(result[i].count) > result[j].total_am/float64(result[j].count)
	})
	for _, r := range result {
		symbol := strings.Split(r.symbol, ".")
		fmt.Println(symbol[0], symbol[1], symbol[2])
		fmt.Printf("\tMaxGap: %0.8f  AvgGap:  %0.8f\n", r.max_gap, r.total_gap/float64(r.count))
		fmt.Printf("\tMaxAm:  %0.8f  AvgAm:   %0.8f\n", r.max_am, r.total_am/float64(r.count))
		fmt.Println("\tAvgTime:", r.total_time/time.Duration(r.count))
		fmt.Println("\tCount:", r.count)
	}
}

func setup_signal() {
	c := make(chan os.Signal)
	signal.Notify(c)
	<-c
	dump_stat()
	os.Exit(1)
}

func find_gap(d time.Duration, min_gap, min_amount float64, timeout int) {
	start := time.Now()
	for {
		end := time.Now()
		if end.Sub(start) > d {
			break
		}

		fmt.Println("======================", end.Format("2006-01-02T15:04:05"))
		// iterate on each pair
		for pair, ex_list := range cp_ex {
			if len(ex_list) <= 1 {
				continue
			}

			ctx := context.TODO()
			ps = make([]ExPrice, 0, 0)
			cp := NewCurrencyPair2(pair)

			ts := time.Now()
			for _, exname := range ex_list {
				sem.Acquire(ctx, 1)

				go get_depth(exname, &cp)

			}
			sem.Acquire(ctx, int64(len(ex_list)))
			sem.Release(int64(len(ex_list)))

			te := time.Now()
			elapsed := te.Sub(ts)
			time.Sleep(time.Duration(1+http_error) * time.Second)
			http_error = 0

			// it is meaningless to compare for 1 result
			if len(ps) <= 1 {
				fmt.Println(pair, "toke too long", elapsed)
				continue
			}

			// it is too "old" for a one second
			if elapsed > time.Duration(timeout)*time.Second {
				continue
			}

			// get highest buyer
			sort.Slice(ps, func(i, j int) bool {
				return ps[i].buy1.Price > ps[j].buy1.Price
			})
			hep := ps[0]

			// get lowest seller
			sort.Slice(ps, func(i, j int) bool {
				return ps[i].sell1.Price < ps[j].sell1.Price
			})
			lep := ps[0]

			gap := (hep.buy1.Price - lep.sell1.Price) / lep.sell1.Price
			if gap < min_gap {
				fmt.Println(pair, "LowGap:", gap)
				continue
			}

			if hep.buy1.Amount < min_amount || lep.sell1.Amount < min_amount {
				fmt.Println(pair, "LowAmount:", hep.ex, hep.buy1.Amount, lep.ex, lep.sell1.Amount)
				continue
			}

			fmt.Println(pair, elapsed)
			fmt.Println("\tHighest buyer", hep.ex, hep.buy1.Price, hep.buy1.Amount)
			fmt.Println("\tLowest seller", lep.ex, lep.sell1.Price, lep.sell1.Amount)
			fmt.Println("\tMaxGap:  ", gap)

			// calculate possible amount
			max_am := math.Min(hep.buy1.Amount, lep.sell1.Amount)
			fmt.Println("\tMax Amount", max_am)

			var key string
			if hep.ex > lep.ex {
				key = strings.Join([]string{pair, hep.ex, lep.ex}, ".")
			} else {
				key = strings.Join([]string{pair, lep.ex, hep.ex}, ".")
			}
			if v, ok := statistic[key]; ok {
				v.max_gap = math.Max(statistic[key].max_gap, gap)
				v.max_am = math.Max(statistic[key].max_am, max_am)
				v.total_gap += gap
				v.total_am += max_am
				v.total_time += elapsed
				v.count += 1
				statistic[key] = v
			} else {
				statistic[key] = Gap{key, gap, gap, max_am, max_am, elapsed, 1}
			}
		}
	}
}

var permutation [][]string

func pickup(curr []string, rest []int, start, count int) {
	for i := start; i < len(curr)+1-count; i++ {
		rest[count-1] = i
		if count-1 == 0 {
			var combine []string
			for _, v := range rest {
				combine = append(combine, curr[v])
			}
			permutation = append(permutation, combine)
			combine = nil
			for _, v := range rest {
				combine = append([]string{curr[v]}, combine...)
			}
			permutation = append(permutation, combine)
		} else {
			pickup(curr, rest, i+1, count-1)
		}
	}
}

func get_permutation(curr []string, count int) [][]string {
	var result []int

	result = make([]int, count, count)
	pickup(curr, result, 0, count)

	return permutation
}

type CLI struct {
	*cli.Cli
}

// NewCLI initializes new command line interface
func NewCLI() *CLI {
	c := &CLI{cli.App("find_gap", "find gap between exchanges")}

	return c
}

type Permit struct {
	NumEX   int `json:"NumEX"`
	NumCoin int `json:"NumCoin"`
}

func get_permition(uname, uuid string) Permit {
	req := &http.Request{
		Method: "GET",
	}
	req.URL, _ = url.Parse("http://gapseeker.tk/permit?uname=" + uname + "&uuid=" + uuid)

	client := &http.Client{
		Timeout: 15 * time.Second,
	}
	resp, err := client.Do(req)
	if err != nil {
		return Permit{2, 1}
	}

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return Permit{2, 1}
	}
	resp.Body.Close()

	var perm Permit
	json.Unmarshal(body, &perm)

	return perm
}

type Config struct {
	Uname string `json:"Uname"`
	UUID  string `json:"UUID"`
}

func WriteConfig(uname, uuid string) {
	var conf Config

	conf.Uname = uname
	conf.UUID = uuid

	plain, _ := json.Marshal(conf)
	ioutil.WriteFile("config.json", plain, 0644)
}

func ReadConfig() (Config, error) {
	var conf Config
	plain, err := ioutil.ReadFile("config.json")
	if err != nil {
		return conf, err
	}

	json.Unmarshal(plain, &conf)
	return conf, nil
}

func (c *CLI) RegisterCommands() {
	c.Command("config", "", func(cmd *cli.Cmd) {
		var (
			uname = cmd.StringArg("NA", "we", "User name")
			uuid  = cmd.StringArg("ID", "id", "User name")
		)

		cmd.Action = func() {
			WriteConfig(*uname, *uuid)
		}
	})
	c.Command("run", "run it", func(cmd *cli.Cmd) {
		var (
			exname   = cmd.StringArg("EX", "", "The Exchanges to find")
			base     = cmd.StringArg("BA", "", "Base coins")
			coins    = cmd.StringArg("CO", "", "Currency You want to use")
			duration = cmd.StringArg("DU", "1", "Duration to run in hour")
			timeout  = cmd.IntArg("TO", 1, "Http request timeout in second")
			gap      = cmd.StringArg("GA", "0.001", "Minimum gap between two coin")
			am       = cmd.StringArg("AM", "1", "Minimum amount between two coin")
		)

		cmd.Action = func() {
			// check already running process
			process := goprocess.FindAll()
			instance := 0
			for _, p := range process {
				if strings.Contains(p.Exec, "find_gap") {
					instance++
				}
			}
			if instance != 1 {
				fmt.Println("Only one instance is available")
				return
			}

			exchanges = strings.Split(*exname, " ")
			base_coin := strings.Split(*base, " ")
			currencys := strings.Split(*coins, " ")

			// check permission before run
			conf := Config{"none", "abc"}
			conf, err := ReadConfig()
			if err != nil {
				if err.Error() == "open config.json: no such file or directory" ||
					err.Error() == "open config.json: The system cannot find the file specified." {
					fmt.Println("You haven't configured your account yet")
					fmt.Println("Only 2 exchanges and 1 currency pair could be chosen")
				} else {
					fmt.Println("Configuration error:", err)
					return
				}
			}
			perm := get_permition(conf.Uname, conf.UUID)
			if perm.NumEX >= 0 && len(exchanges) > perm.NumEX {
				fmt.Println("You could only choose", perm.NumEX, "exchanges")
				return
			}

			for _, b := range base_coin {
				for _, c := range currencys {
					if c != "" && b != c {
						currencypair = append(currencypair, c+"_"+b)
					}
				}
			}
			if perm.NumCoin >= 0 && len(currencypair) > perm.NumCoin {
				fmt.Println("You could only choose", perm.NumCoin, "currency including base and quote")
				return
			}

			cp_ex = make(map[string][]string, len(currencypair))
			for _, cp := range currencypair {
				cp_ex[cp] = []string{}
			}
			exs = make(map[string]ExData, len(exchanges))
			for _, exname := range exchanges {
				ex := GetEx(exname)
				if ex == nil {
					fmt.Println(exname, "is not supported")
					continue
				}
				symbols, err := ex.GetSymbols()
				if err != nil {
					fmt.Println("Error:", exname, "connection failed")
					continue
				}
				exs[exname] = ExData{ex, symbols}

				for _, cp := range currencypair {
					if has_pair(symbols, cp) {
						if v, ok := cp_ex[cp]; ok {
							v = append(v, exname)
							cp_ex[cp] = v
						} else {
							cp_ex[cp] = []string{exname}
						}
					}
				}
			}

			if len(exs) < 2 {
				fmt.Println("At lease two exchanges to compare")
				return
			}

			for e, l := range cp_ex {
				if len(l) == 1 {
					fmt.Println("Warning: Only", strings.Join(l, " "), "has", e)
				} else if len(l) == 0 {
					fmt.Println("Warning: No exchange you choose has", e)
				}
			}
			has_pair := false
			for e, l := range cp_ex {
				if len(l) > 1 {
					fmt.Println("Scan gap of", e, "on", strings.Join(l, " "))
					has_pair = true
				}
			}
			if !has_pair {
				fmt.Println("There is no valid currencypair to watch")
				return
			}

			sem = semaphore.NewWeighted(int64(len(exs)))
			duration_f, err := strconv.ParseFloat(*duration, 64)
			if err != nil {
				fmt.Println("The Gap is not correct")
				return
			}
			duration_f *= 60
			if duration_f < 1 {
				fmt.Println("Duration is less than 1 minute")
				return
			}
			min_gap, err := strconv.ParseFloat(*gap, 64)
			if err != nil {
				fmt.Println("The Gap is not correct")
				return
			}
			min_amount, err := strconv.ParseFloat(*am, 64)
			if err != nil {
				fmt.Println("The Amount is not correct")
				return
			}

			//fmt.Println("Target exchanges:", *exname)
			//fmt.Println("Target coins:", currencypair)

			statistic = make(map[string]Gap)
			http_error = 0
			go setup_signal()
			time.Sleep(time.Duration(8) * time.Second)
			find_gap(time.Duration(duration_f)*time.Minute, min_gap/100, min_amount, *timeout)
			dump_stat()
		}
	})
}

func main() {
	cli := NewCLI()
	cli.RegisterCommands()
	cli.Run(os.Args)
}
