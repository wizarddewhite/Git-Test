package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"net/url"
	"os"
	"strings"
	"time"

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

func Get(apiURL string, params url.Values) (rs []byte, err error) {
	var Url *url.URL
	Url, err = url.Parse(apiURL)
	if err != nil {
		fmt.Printf("rul error:\r\n%v", err)
		return nil, err
	}
	Url.RawQuery = params.Encode()
	resp, err := http.Get(Url.String())
	if err != nil {
		fmt.Println("err:", err)
		return nil, err
	}
	defer resp.Body.Close()
	return ioutil.ReadAll(resp.Body)
}

// get month holiday
func retrieve_data(month string) ([]byte, error) {
	// api url
	juheURL := "http://v.juhe.cn/calendar/month"

	param := url.Values{}

	param.Set("key", "api-key")
	param.Set("year-month", month)

	// get data
	data, err := Get(juheURL, param)
	if err != nil {
		fmt.Errorf("request error:\r\n%v", err)
		return nil, err
	}
	return data, nil
}

func isHoliday(data []byte, date string) bool {
	js, _ := NewJson(data)
	// reason, _ := js.Get("reason").String()
	// fmt.Println(reason)
	holidays, _ := js.Get("result").Get("data").Get("holiday_array").Array()
	for _, holiday_arr := range holidays {
		holiday_list := holiday_arr.(map[string]interface{})["list"]
		// fmt.Println(holiday_list)
		for _, holiday := range holiday_list.([]interface{}) {
			h := holiday.(map[string]interface{})
			if 0 == strings.Compare("1", h["status"].(string)) &&
				0 == strings.Compare(date, h["date"].(string)) {
				return true
			}
		}
	}
	return false
}

func juhe_holiday() {
	// file, _ := ioutil.ReadFile("juhe_holiday.conf")
	file, _ := retrieve_data(time.Now().Format("2006-1"))
	// fmt.Println(string(file))
	if isHoliday(file, "2020-1-24") {
		fmt.Println("2020-1-24 is holiday")
	} else {
		fmt.Println("2020-1-24 is not holiday")
	}
}

func main() {
	now := time.Now().Format("2006-1")
	fmt.Println(now)
	// simple_test()
	juhe_holiday()
}
