package main

import (
	"context"
	"fmt"
	"math"
	"sort"
	"strings"
	"sync"
	"time"

	. "github.com/RichardWeiYang/bcex/lib"
	"golang.org/x/sync/semaphore"
)

var exchange = "bigone"

var base_curr = []string{"btc", "eth", "yoyo", "omg", "neo", "usdt", "eos", "xmr", "qtum", "xrp"}
var possible string

var currencypair []string
var sem *semaphore.Weighted
var mutex = &sync.Mutex{}
var ps map[string]ExPrice

type ExPrice struct {
	ex    string
	cp    CurrencyPair
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
	for i := min(2, len(depth.Asks)); i >= 1; i-- {
		fmt.Printf("\t%0.8f\t%0.8f\n",
			depth.Asks[len(depth.Asks)-i].Price,
			depth.Asks[len(depth.Asks)-i].Amount)
	}
	fmt.Println("Bids:")
	for i := 0; i < min(2, len(depth.Bids)); i++ {
		fmt.Printf("\t%0.8f\t%0.8f\n",
			depth.Bids[i].Price,
			depth.Bids[i].Amount)
	}
}

func get_depth(exname string, cp *CurrencyPair) {
	ex := GetEx(exname)
	symbol := cp.ToSymbol("_")
	depth, err := ex.GetDepth(cp)
	mutex.Lock()
	ps[symbol] = ExPrice{exname, *cp, Unit{0, 0}, Unit{0, 0}}
	if err == nil {
		//dump_depth(exname, cp.String(), depth)
		if len(depth.Asks) != 0 && len(depth.Bids) != 0 {
			ps[symbol] = ExPrice{exname, *cp, depth.Asks[len(depth.Asks)-1], depth.Bids[0]}
		}
	}
	mutex.Unlock()
	sem.Release(1)
	return
}

func setup_possible(base_curr []string) {
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

type App struct {
	symbol string
	begin  int
	end    int
}

/*
 * output currency pair
 *      A_C A_B B_C
 */
func reorder(cps []string) []string {
	var ordered []string
	var position map[string]App
	var aps []App
	position = make(map[string]App)
	for _, cp := range cps {
		symbols := strings.Split(cp, "_")
		if val, ok := position[symbols[0]]; ok {
			val.begin++
			position[symbols[0]] = val
		} else {
			position[symbols[0]] = App{symbols[0], 1, 0}
		}
		if val, ok := position[symbols[1]]; ok {
			val.end++
			position[symbols[1]] = val
		} else {
			position[symbols[1]] = App{symbols[1], 0, 1}
		}
	}

	for _, v := range position {
		aps = append(aps, v)
	}

	sort.Slice(aps, func(i, j int) bool {
		return aps[i].begin > aps[j].begin
	})

	ordered = append(ordered, aps[0].symbol+"_"+aps[2].symbol)
	ordered = append(ordered, aps[0].symbol+"_"+aps[1].symbol)
	ordered = append(ordered, aps[1].symbol+"_"+aps[2].symbol)

	return ordered
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

type Gap struct {
	symbol             string
	max_gap, total_gap float64
	max_am, total_am   float64
	count              int
}

var statistic map[string]Gap

func record(key string, gap, amount float64) {
	if v, ok := statistic[key]; ok {
		v.max_gap = math.Max(statistic[key].max_gap, gap)
		v.max_am = math.Max(statistic[key].max_am, amount)
		v.total_gap += gap
		v.total_am += amount
		v.count += 1
		statistic[key] = v
	} else {
		statistic[key] = Gap{key, gap, gap, amount, amount, 1}
	}
}

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
		fmt.Println("\tCount:", r.count)
	}
	fmt.Println("### Order in gap")
	sort.Slice(result, func(i, j int) bool {
		return result[i].max_gap/float64(result[i].count) > result[j].max_gap/float64(result[j].count)
	})
	for _, r := range result {
		symbol := strings.Split(r.symbol, ".")
		fmt.Println(symbol[0], symbol[1], symbol[2])
		fmt.Printf("\tMaxGap: %0.8f  AvgGap:  %0.8f\n", r.max_gap, r.total_gap/float64(r.count))
		fmt.Printf("\tMaxAm:  %0.8f  AvgAm:   %0.8f\n", r.max_am, r.total_am/float64(r.count))
		fmt.Println("\tCount:", r.count)
	}
	fmt.Println("### Order in amount")
	sort.Slice(result, func(i, j int) bool {
		return result[i].max_am/float64(result[i].count) > result[j].max_am/float64(result[j].count)
	})
	for _, r := range result {
		symbol := strings.Split(r.symbol, ".")
		fmt.Println(symbol[0], symbol[1], symbol[2])
		fmt.Printf("\tMaxGap: %0.8f  AvgGap:  %0.8f\n", r.max_gap, r.total_gap/float64(r.count))
		fmt.Printf("\tMaxAm:  %0.8f  AvgAm:   %0.8f\n", r.max_am, r.total_am/float64(r.count))
		fmt.Println("\tCount:", r.count)
	}
}

func main() {
	ex := GetEx(exchange)
	symbols, _ := ex.GetSymbols()
	permutation = nil
	get_permutation(base_curr, 3)
	fmt.Println("Base curr:", base_curr)
	fmt.Println("Possible permutation:", permutation)
	statistic = make(map[string]Gap)
	start := time.Now()
	for {
		end := time.Now()
		if end.Sub(start) > 12*time.Hour {
			break
		}
		fmt.Println("===============", time.Now().Format("2006-01-02T15:04:05"))
		for _, perm := range permutation {
			setup_possible(perm)
			currencypair = filter(symbols)
			if len(currencypair) != 3 {
				//fmt.Println("Can't form a triangle")
				continue
			}
			time.Sleep(2 * time.Second)

			ocp := reorder(currencypair)
			ps = make(map[string]ExPrice)
			sem = semaphore.NewWeighted(int64(len(ocp)))
			ctx := context.TODO()

			ts := time.Now()
			for _, pair := range ocp {
				cp := NewCurrencyPair2(pair)
				sem.Acquire(ctx, 1)

				go get_depth(exchange, &cp)
			}
			sem.Acquire(ctx, int64(len(ocp)))
			sem.Release(int64(len(ocp)))
			te := time.Now()
			elapsed := te.Sub(ts)

			//for _, pair := range ocp {
			//	fmt.Println(ps[pair])
			//}

			// loop 0->2  0<-1<-2
			high_sell := ps[ocp[0]].buy1.Price
			high_amount := ps[ocp[0]].buy1.Amount
			low_buy := ps[ocp[1]].sell1.Price * ps[ocp[2]].sell1.Price
			low_amount := math.Min(ps[ocp[1]].sell1.Amount, ps[ocp[2]].sell1.Amount)
			max_amount := math.Min(high_amount, low_amount)
			max_gap := (high_sell - low_buy) / low_buy

			if low_buy != 0 && max_gap > 0.005 {
				fmt.Println(exchange, elapsed, "Sell:", ocp[0], "Buy:", ocp[1], ocp[2])
				fmt.Printf("lowest buy  : %0.8f Amount: %0.8f\n", low_buy, low_amount)
				fmt.Printf("highest sell: %0.8f Amount: %0.8f\n", high_sell, high_amount)
				fmt.Printf("MaxGap:       %0.8f\n", max_gap)
				record(strings.Join(ocp, "."), max_gap, max_amount)
			}

			// loop 0<-2  0->1->2
			low_buy = ps[ocp[0]].sell1.Price
			low_amount = ps[ocp[0]].buy1.Amount
			high_sell = ps[ocp[1]].buy1.Price * ps[ocp[2]].buy1.Price
			high_amount = math.Min(ps[ocp[1]].sell1.Amount, ps[ocp[2]].sell1.Amount)
			max_amount = math.Min(high_amount, low_amount)
			max_gap = (high_sell - low_buy) / low_buy

			if low_buy != 0 && max_gap > 0.005 {
				fmt.Println(exchange, elapsed, "Buy:", ocp[0], "Sell:", ocp[1], ocp[2])
				fmt.Printf("lowest buy  : %0.8f Amount: %0.8f\n", low_buy, low_amount)
				fmt.Printf("highest sell: %0.8f Amount: %0.8f\n", high_sell, high_amount)
				fmt.Printf("MaxGap:       %0.8f\n", max_gap)
				record(strings.Join(ocp, "."), max_gap, max_amount)
			}
		}
	}
	dump_stat()
}
