package main

import (
	"bufio"
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"io/ioutil"
	"net/http"
	"os"
	"strconv"
	"strings"
)

type Stat struct {
	Name     string
	Inbound  string
	Outbound string
}

type StatSlice struct {
	Users string
	Stats []Stat
}

func send_total_used(s *StatSlice) {
	b, err := json.Marshal(s)
	if err != nil {
		fmt.Println("json err:", err)
	}

	res, _ := http.Post("http://185.92.221.13/statistic/update", "application/json; charset=utf-8", bytes.NewReader(b))
	io.Copy(os.Stdout, res.Body)
}

func parse_total_used(s *StatSlice) {
	file, err := os.Open("/root/total_used")
	if err != nil {
		return
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	var inbound, outbound, val float64
	inbound = 0
	outbound = 0
	for scanner.Scan() {
		words := strings.Fields(scanner.Text())
		s.Stats = append(s.Stats, Stat{Name: words[0],
			Inbound: words[1], Outbound: words[2]})
		val, err = strconv.ParseFloat(words[1], 64)
		inbound += val
		val, err = strconv.ParseFloat(words[2], 64)
		outbound += val
	}
	s.Stats = append(s.Stats, Stat{Name: "total",
		Inbound:  strconv.FormatFloat(inbound, 'f', 6, 64),
		Outbound: strconv.FormatFloat(outbound, 'f', 6, 64)})
}

func parse_user_sessions(s *StatSlice) {
	b, err := ioutil.ReadFile("/root/sessions") // just pass the file name
	if err != nil {
		fmt.Print(err)
		return
	}

	s.Users = string(b)
}
func main() {
	var s StatSlice
	parse_total_used(&s)
	parse_user_sessions(&s)
	send_total_used(&s)
}
