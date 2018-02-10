package main

import (
	"fmt"
	"strings"

	. "github.com/RichardWeiYang/bcex/lib"
)

var base_curr = []string{"btc", "eth", "eos", "neo", "usdt", "ae", "oct", "lrc"}
var exchanges = []string{"bigone", "bitfinex", "binance", "okex", "huobi"}
var possible string

func init() {
	pair := make([]string, 0, len(base_curr)*(len(base_curr)-1))
	for i := 0; i < len(base_curr); i++ {
		for j := i + 1; j < len(base_curr); j++ {
			pair = append(pair, base_curr[i]+"_"+base_curr[j])
			pair = append(pair, base_curr[j]+"_"+base_curr[i])
		}
	}
	possible = strings.Join(pair, " ")
}

// usdt contains usd
func Contains(s, substr string) bool {
	return strings.Index(s, substr) != -1
}

func filter(symbols []string) []string {
	pair := make([]string, 0, 0)
	for _, s := range symbols {
		if Contains(possible, s) {
			pair = append(pair, s)
		}
	}
	return pair
}

func main() {
	//common := make([]string, 0, 0)
	var cp map[string][]string
	cp = make(map[string][]string)
	for _, exname := range exchanges {
		ex := GetEx(exname)
		symbols, _ := ex.GetSymbols()

		cp[exname] = filter(symbols)
	}
	//fmt.Println(cp)
	for k, v := range cp {
		fmt.Println("Exchange:", k)
		fmt.Println(v, "\n")
	}
}
