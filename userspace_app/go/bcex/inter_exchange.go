package main

import (
	"context"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"math"
	"os"
	"os/signal"
	"sort"
	"strconv"
	"strings"
	"sync"
	"syscall"
	"time"

	. "github.com/RichardWeiYang/bcex/lib"
	"golang.org/x/sync/semaphore"
)

var name = "lrc_eth"
var stg Strategy
var currencypair []string
var exchanges []string
var keys map[string]ExchangeKey
var exs map[string]ExData
var min_amount float64
var in_process int

var ps []ExPrice
var sem *semaphore.Weighted
var mutex = &sync.Mutex{}

type ExData struct {
	ex      Exchange
	balance []Balance
}

type ExPrice struct {
	ex    string
	sell1 Unit
	buy1  Unit
}

type ExchangeKey struct {
	AccessKeyId string `json:"AccessKeyId"`
	SecretKeyId string `json:"SecretKeyId"`
}

func ReadConf() error {
	raw, err := ioutil.ReadFile("config.json")
	if err != nil {
		return err
	}

	plain := Decrypt(string(raw), "placeholderplaceholderplaceholde")

	json.Unmarshal([]byte(plain), &keys)
	return nil
}

type Strategy struct {
	CP       string    `json:"CP"`
	Exchange [2]string `json:"Exchange"`
	// rate as buyer exchange
	Rate      [2]float64 `json:"Rate"`
	MinAmount float64    `json:"MinAmount"`
	MaxAmount float64    `json:"MaxAmount"`
	MinRate   float64    `json:"MinRate"`
	MaxRate   float64    `json:"MaxRate"`

	// increase when buy on Exchange[0] success
	// decrease when buy on Exchange[1] success
	RelAmount float64 `json:"RelAmount"`
}

func ReadStrategy(file string) error {
	raw, err := ioutil.ReadFile(file)
	if err != nil {
		return err
	}

	json.Unmarshal(raw, &stg)

	currencypair = append(currencypair, stg.CP)
	exchanges = append(exchanges, stg.Exchange[0], stg.Exchange[1])
	min_amount = stg.MinAmount
	sem = semaphore.NewWeighted(int64(len(exchanges)))

	return nil
}

func WriteStrategy(file string) {
	plain, _ := json.Marshal(stg)
	ioutil.WriteFile(file, plain, 0644)
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
	depth, err := ex.GetDepth(cp)
	if err == nil && len(depth.Asks) != 0 && len(depth.Bids) != 0 {
		//dump_depth(exname, cp.String(), depth)
		mutex.Lock()
		ps = append(ps, ExPrice{exname,
			depth.Asks[len(depth.Asks)-1], depth.Bids[0]})
		mutex.Unlock()
	}
	sem.Release(1)
}

// A_B
func cal_amount(cp *CurrencyPair, sell_ex, buy_ex string,
	sell_am, buy_am, min_am float64, buy_price float64) (string, float64) {
	/* we sell A get B at first exchange, so the amount is minimum of
	 * amount in depth and amount A in balance

	 * and buy A with B at second exchange, so the amount is minimum of
	 * amount in depth and amount B in balance
	 */

	var err error

	balance_A := 0.0
	for _, b := range exs[sell_ex].balance {
		if strings.EqualFold(cp.CurrencyA.Symbol, b.Currency) {
			balance_A, err = strconv.ParseFloat(b.Balance, 64)
			if err != nil {
				return "", 0
			}
			break
		}
	}
	if balance_A < min_am {
		fmt.Println(sell_ex, "need more", cp.CurrencyA.Symbol)
		return "sell", 0
	}
	max_sell := math.Min(sell_am, balance_A)

	balance_B := 0.0
	for _, b := range exs[buy_ex].balance {
		if strings.EqualFold(cp.CurrencyB.Symbol, b.Currency) {
			balance_B, err = strconv.ParseFloat(b.Balance, 64)
			if err != nil {
				return "", 0
			}
			break
		}
	}
	balance_B = balance_B / buy_price
	if balance_B < min_am {
		fmt.Println(buy_ex, "need more", cp.CurrencyB.Symbol)
		return "buy", 0
	}
	max_buy := math.Min(buy_am, balance_B)
	return "", math.Min(max_sell, max_buy)
}

func placeorder(cp *CurrencyPair, sell_ex, buy_ex string,
	sell_price, buy_price float64, amount float64) {
	var sell_id, buy_id string
	var err error
	var filled int
	// execute binance first

	if sell_ex == "binance" {
		sell_id, err = exs[sell_ex].ex.NewOrder(&Order{CP: *cp, Side: "sell", Price: sell_price, Amount: amount})
		if err != nil {
			fmt.Println(sell_ex, err)
			goto EXIT
		}
		buy_id, err = exs[buy_ex].ex.NewOrder(&Order{CP: *cp, Side: "buy", Price: buy_price, Amount: amount})
		if err != nil {
			fmt.Println(buy_ex, err)
			exs[sell_ex].ex.CancelOrder(&Order{Id: sell_id, CP: *cp})
			goto EXIT
		}
	} else {
		buy_id, err = exs[buy_ex].ex.NewOrder(&Order{CP: *cp, Side: "buy", Price: buy_price, Amount: amount})
		if err != nil {
			fmt.Println(buy_ex, err)
			goto EXIT
		}
		sell_id, err = exs[sell_ex].ex.NewOrder(&Order{CP: *cp, Side: "sell", Price: sell_price, Amount: amount})
		if err != nil {
			fmt.Println(sell_ex, err)
			exs[buy_ex].ex.CancelOrder(&Order{Id: buy_id, CP: *cp})
			goto EXIT
		}
	}
	fmt.Println("Sell:", sell_id, "Buy:", buy_id)

	filled = 0
	for {
		time.Sleep(time.Duration(10) * time.Second)
		o, _ := exs[sell_ex].ex.QueryOrder(&Order{Id: sell_id, CP: *cp})
		if o.Amount == o.Executed {
			filled += 1
		}
		o, _ = exs[buy_ex].ex.QueryOrder(&Order{Id: buy_id, CP: *cp})
		if o.Amount == o.Executed {
			filled += 1
		}
		if filled == 2 {
			break
		}
		filled = 0
	}
	in_process--
	adj_stg(buy_ex, "deal")
	adj_relamount(buy_ex, amount)
	return
EXIT:
	fmt.Println("Order failed:", err)
	WriteStrategy(name + ".conf")
	os.Exit(1)
}

func setup_signal() {
	c := make(chan os.Signal)
	signal.Notify(c, syscall.SIGINT, syscall.SIGTERM)
	<-c
	WriteStrategy(name + ".conf")
	os.Exit(1)
}

func main() {
	err := ReadConf()
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	err = ReadStrategy(name + ".conf")
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	go setup_signal()

	// setup exchange data
	exs = make(map[string]ExData, len(exchanges))
	for _, exname := range exchanges {
		ex := GetEx(exname)
		ek := keys[exname]
		ex.SetKey(ek.AccessKeyId, ek.SecretKeyId)
		exs[exname] = ExData{ex, []Balance{}}
	}

Next:
	for {
		fmt.Println("======================", time.Now().Format("2006-01-02T15:04:05"))
		// retrieve balance
		for _, exname := range exchanges {
			b, err := exs[exname].ex.GetBalance()
			if err != nil {
				fmt.Println(err.Error())
				continue Next
			}
			exs[exname] = ExData{exs[exname].ex, b}
		}

		// iterate on each pair
		for _, pair := range currencypair {
			ctx := context.TODO()
			ps = make([]ExPrice, 0, 0)
			cp := NewCurrencyPair2(pair)

			time.Sleep(time.Duration(2) * time.Second)

			ts := time.Now()
			for _, exname := range exchanges {
				sem.Acquire(ctx, 1)

				go get_depth(exname, &cp)

			}
			sem.Acquire(ctx, int64(len(exchanges)))
			sem.Release(int64(len(exchanges)))

			// it is meaningless to compare for 1 result
			if len(ps) <= 1 {
				continue
			}

			// it is too "old" for a one second
			te := time.Now()
			elapsed := te.Sub(ts)
			if elapsed > time.Second {
				fmt.Println("out of time")
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

			max_gap := (hep.buy1.Price - lep.sell1.Price) / lep.sell1.Price

			// compare the buyer's exchange rate
			var idx int
			var ex string
			for idx, ex = range stg.Exchange {
				if ex == lep.ex {
					break
				}
			}

			//if max_gap < 0.005 {
			if max_gap < 0 {
				fmt.Println("NegGap:", max_gap, lep.ex, hep.ex)
				continue
			} else if max_gap < stg.Rate[idx] {
				fmt.Println("LowGap:", max_gap, lep.ex, hep.ex)
				continue
			}

			if hep.buy1.Amount < min_amount || lep.sell1.Amount < min_amount {
				fmt.Println("LowAmount:", hep.buy1.Amount, lep.sell1.Amount)
				continue
			}

			fmt.Println(pair, elapsed)
			fmt.Println("Highest buyer", hep.ex, hep.buy1.Price, hep.buy1.Amount)
			fmt.Println("Lowest seller", lep.ex, lep.sell1.Price, lep.sell1.Amount)
			fmt.Println("MaxGap:  ", max_gap)

			// calculate possible amount
			fx, max_am := cal_amount(&cp, hep.ex, lep.ex,
				hep.buy1.Amount, lep.sell1.Amount,
				min_amount, lep.sell1.Price)
			eq_btc := max_am * lep.sell1.Price
			fmt.Println("Max Amount", max_am, "Equal BTC:", eq_btc)

			// place order
			if max_am < min_amount {
				switch fx {
				case "buy":
					adj_stg(lep.ex, "fail")
				case "sell":
					adj_stg(hep.ex, "fail")
				}
			} else {
				if max_am/3 > stg.MaxAmount {
					max_am = stg.MaxAmount
				} else {
					max_am = stg.MinAmount
				}
				in_process++
				placeorder(&cp, hep.ex, lep.ex, hep.buy1.Price, lep.sell1.Price, math.Floor(max_am))
			}
		}

		for in_process > 2 {
			time.Sleep(time.Duration(5) * time.Second)
		}
	}
}

func adj_relamount(ex string, am float64) {
	if ex == stg.Exchange[0] {
		stg.RelAmount += am
	}

	stg.RelAmount -= am
}

func increase_rate(ex string) {
	var i int
	var exchange string
	for i, exchange = range stg.Exchange {
		if ex == exchange {
			break
		}
	}

	// adjust after we catch up the other side
	if i == 0 && stg.RelAmount < 0 {
		return
	} else if i == 1 && stg.RelAmount > 0 {
		return
	}

	rate := stg.Rate[i] + 0.0001
	stg.Rate[i] = math.Min(stg.MaxRate, rate)
}

func decrease_rate(ex string) {
	var i int
	var exchange string
	for i, exchange = range stg.Exchange {
		if ex == exchange {
			break
		}
	}

	rate := (stg.Rate[i] + stg.MinRate) / 2
	stg.Rate[i] = math.Max(stg.MinRate, rate)
}

// The idea is simple:
// 1. Deal: increase the buyer's exchange rate
// 2. Fail: decrease the opposite side exchange rate
func adj_stg(ex, state string) {
	switch state {
	case "deal":
		// increase ex(self) rate
		increase_rate(ex)
	case "fail":
		// decrease ex(the other) rate
		decrease_rate(ex)
	}
	fmt.Println("adj_stg", state, stg.Rate[0], stg.Rate[1])
}
