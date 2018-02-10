package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"math/rand"
	"time"

	. "github.com/RichardWeiYang/bcex/lib"
)

var cp = NewCurrencyPair2("bnb_usdt")
var exname = "binance"
var ex = GetEx(exname)
var r = rand.New(rand.NewSource(time.Now().UnixNano()))

type ExchangeKey struct {
	AccessKeyId string `json:"AccessKeyId"`
	SecretKeyId string `json:"SecretKeyId"`
}

var keys map[string]ExchangeKey

func ReadConf() error {
	raw, err := ioutil.ReadFile("config.json")
	if err != nil {
		return err
	}

	plain := Decrypt(string(raw), "placeholderplaceholderplaceholde")

	json.Unmarshal([]byte(plain), &keys)
	return nil
}

type OS struct {
	id     string
	err    error
	filled bool
}

func PlaceOrder(o *Order, idc chan OS) {
	id, err := ex.NewOrder(o)
	idc <- OS{id, err, false}
}

func LazyTask(state chan int) {
	var osc [2]chan OS
	osc[0] = make(chan OS)
	osc[1] = make(chan OS)
	side := [2]string{"sell", "buy"}
	var price [2]float64
	var os [2]OS

	// get depth
	depth, err := ex.GetDepth(&cp)
	if err != nil {
		return
	}

	// find a margin for 1%
	a := 0
	b := 0
	asks := depth.Asks
	bids := depth.Bids
	var margin float64
	for {
		price[0] = asks[len(asks)-1-a].Price
		price[1] = bids[b].Price
		margin = (price[0] - price[1]) / price[1]
		if margin > 0.005 {
			price[0] -= 0.001
			price[1] += 0.001
			break
		}
		a++
		b++
		if a >= len(asks) || b >= len(bids) {
			break
		}
	}

	if margin < 0.005 {
		fmt.Println("Don't have proper gap now")
		state <- 1
		return
	}

	// calculate amount
	amount := 1.3

	// place order
	for i := range []int{0, 1} {
		go PlaceOrder(&Order{CP: cp, Side: side[i], Price: price[i], Amount: amount}, osc[i])
	}

	for i := range []int{0, 1} {
		os[i] = <-osc[i]
	}
	// cancel orders if one failed
	has_err := false
	for i := range []int{0, 1} {
		if os[i].err != nil {
			has_err = true
			fmt.Println("Error:", os[i].err)
		}
	}

	if has_err {
		for i := range []int{0, 1} {
			if os[i].err == nil {
				ex.CancelOrder(&Order{Id: os[i].id, CP: cp})
			}
		}
		state <- -1
		return
	}

	ts := time.Now()
	fmt.Println(ts.Format("2006-01-02T15:04:05"), "Buy", price[1], "Sell", price[0], "margin", margin)
	fmt.Println("Sell Order:", os[0].id)
	fmt.Println("Buy Order:", os[1].id)

	// check order state
	filled := 0
	for {
		te := time.Now()
		elapsed := te.Sub(ts)
		if elapsed > (2 * time.Hour) {
			break
		}

		time.Sleep(time.Duration(60) * time.Second)
		for i := range []int{0, 1} {
			if os[i].filled {
				continue
			}
			o, _ := ex.QueryOrder(&Order{Id: os[i].id, CP: cp})
			if o.Amount == o.Executed {
				filled += 1
				os[i].filled = true
			}

		}

		if filled == 2 {
			fmt.Println(elapsed, "Buy", price[1], "Sell", price[0], "done")
			fmt.Println("Sell Order:", os[0].id)
			fmt.Println("Buy Order:", os[1].id)
			state <- 0
			return
		}
	}

	// cancel it when timeout
	fmt.Println("Roll back since we are too old")
	for i, v := range []int{2, 3} {
		if os[i].filled {
			continue
		}
		ex.CancelOrder(&Order{Id: os[i].id, CP: cp})
		state <- v
	}
}

func main() {
	state := make(chan int)
	err := ReadConf()
	if err != nil {
		fmt.Println(err.Error())
		return
	}

	ek := keys[exname]
	ex.SetKey(ek.AccessKeyId, ek.SecretKeyId)

	go LazyTask(state)

	errors := 0
	success := 0
	sell_failure := 0
	buy_failure := 0
	for {
		switch s := <-state; s {
		// error
		case -1:
			errors++
			if errors >= 5 {
				fmt.Println("Have 5 errors")
				return
			}
		// succeed
		case 0:
			success++
		// no gap
		case 1:
			time.Sleep(time.Duration(r.Intn(60)) * time.Second)
		// sell fail
		case 2:
			sell_failure++
		// buy fail
		case 3:
			buy_failure++
		}
		fmt.Println("Success:", success, "Sell Failure:", sell_failure, "Buy Failure", buy_failure)
		go LazyTask(state)
	}
}
