package main

import (
	"context"
	"fmt"
	"os"
	"sort"
	"strings"
	"sync"

	. "github.com/RichardWeiYang/bcex/lib"
	"github.com/jawher/mow.cli"
	"golang.org/x/sync/semaphore"
)

type CLI struct {
	*cli.Cli
}

// NewCLI initializes new command line interface
func NewCLI() *CLI {
	c := &CLI{cli.App("util", "simple util")}
	return c
}

var symbol_list [][]string
var mutex = &sync.Mutex{}

func get_symbols(exname string, sem *semaphore.Weighted) {
	symbols, err := GetEx(exname).GetSymbols()
	if err == nil {
		mutex.Lock()
		symbol_list = append(symbol_list, symbols)
		mutex.Unlock()
	}
	sem.Release(1)
}

func (c *CLI) RegisterCommands() {
	c.Command("exchanges", "list available exchanges", func(cmd *cli.Cmd) {
		cmd.Action = func() {
			lists := ListEx()
			fmt.Println(strings.Join(lists, " "))
		}
	})

	c.Command("coins", "list available coins", func(cmd *cli.Cmd) {
		cmd.Action = func() {
			var coins map[string]int
			var sem *semaphore.Weighted
			symbol_list = make([][]string, 0, 0)
			coins = make(map[string]int)
			ctx := context.TODO()

			lists := ListEx()
			sem = semaphore.NewWeighted(int64(len(lists)))

			for _, ex := range lists {
				sem.Acquire(ctx, 1)
				go get_symbols(ex, sem)
			}
			sem.Acquire(ctx, int64(len(lists)))
			sem.Release(int64(len(lists)))

			for _, symbols := range symbol_list {
				for _, s := range symbols {
					currencys := strings.Split(s, "_")
					coins[currencys[0]] = 1
					coins[currencys[1]] = 1
				}
			}

			var oc []string
			for v, _ := range coins {
				oc = append(oc, v)
			}
			sort.Slice(oc, func(i, j int) bool {
				return oc[i] < oc[j]
			})
			fmt.Println(strings.Join(oc, " "))
		}
	})
}

func main() {
	cli := NewCLI()
	cli.RegisterCommands()
	cli.Run(os.Args)
}
