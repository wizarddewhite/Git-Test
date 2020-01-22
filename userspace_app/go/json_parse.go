package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"os"

	. "github.com/bitly/go-simplejson"
)

type Setting struct {
	ID string `json:"id"`
}

type Config struct {
	Port     uint16  `json:"port"`
	Settings Setting `json:"settings"`
}

func simple_test() {
	file, _ := os.Open("json.conf")
	config := &Config{}
	decoder := json.NewDecoder(file)
	decoder.Decode(config)
	fmt.Println(config.Port)
	fmt.Println(config.Settings.ID)
}

func juhe_holiday() {
	file, _ := ioutil.ReadFile("juhe_holiday.conf")
	// fmt.Println(string(file))
	js, _ := NewJson(file)
	reason, _ := js.Get("reason").String()
	fmt.Println(reason)
	holidays, _ := js.Get("result").Get("data").Get("holiday_array").Array()
	for _, holiday_arr := range holidays {
		holiday_list := holiday_arr.(map[string]interface{})["list"]
		// fmt.Println(holiday_list)
		for _, holiday := range holiday_list.([]interface{}) {
			h := holiday.(map[string]interface{})
			fmt.Println(h["date"])
			fmt.Println(h["status"])
		}
	}
}

func main() {
	simple_test()
	juhe_holiday()
}
