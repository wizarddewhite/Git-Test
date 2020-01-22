package main

import (
	"bytes"
	"fmt"
	"io/ioutil"
	"net/http"
	"net/smtp"
	"net/url"
	"strings"
	"text/template"
	"time"

	. "github.com/bitly/go-simplejson"
)

/**
 * user : example@example.com login smtp server user
 * password : xxxxx login smtp server password
 * host : smtp.example.com:port smtp.163.com:25
 * to : example@example.com;example1@163.com;example2@sina.com.cn;...
 * subject : The subject of mail
 * body : The content of mail
 * mailtyoe : mail type html or text
**/
func send(to, subject, body, mailtype string) error {
	user := "wizarddewhite@gmail.com"
	password := "xxx"
	host := "smtp.gmail.com:587"

	hp := strings.Split(host, ":")
	auth := smtp.PlainAuth("", user, password, hp[0])
	var content_type string
	if mailtype == "html" {
		content_type = "Content-Type: text/" + mailtype + "; charset=UTF-8"
	} else {
		content_type = "Content-Type: text/plain" + "; charset=UTF-8"
	}

	msg := "From: Shlug <wizarddewhite@gmail.com> \n" +
		"To: " + to + "\n" +
		content_type + "\n" +
		"Subject: " + subject + "\n\n" +
		body
	send_to := strings.Split(to, ";")
	err := smtp.SendMail(host, auth, user, send_to, []byte(msg))
	return err
}

type ConfimrMail struct {
	Name string
	Hash string
}

func next4() string {
	now := time.Now()
	weekday := int(now.Weekday())
	n4 := now.AddDate(0, 0, 4-weekday)
	return n4.Format("2006/01/02")
}

var myClient = &http.Client{Timeout: 10 * time.Second}

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

func is_holiday() bool {
	now := time.Now()
	data, _ := retrieve_data(now.Format("2006-1"))
	weekday := int(now.Weekday())
	n4 := now.AddDate(0, 0, 4-weekday)
	//fmt.Println(n4.Format("20060102"))
	return isHoliday(data, n4.Format("2006-1-2"))
}

func H4Notify(uname, to, hash string) {
	holiday := is_holiday()
	Templ := ``
	if holiday {
		Templ += `
欢度假期，本期活动取消。

有关Hacking Thursday活动的介绍：
http://www.shlug.org/about/#hacking-thursday

SHLUG的新浪微博地址：http://weibo.com/shanghailug 有每次活动照片以及信息发布
`

	} else {
		Templ += `
店名：JAcafe花园咖啡
点评：http://www.dianping.com/shop/2019466
地址：静安区南京西路1649号静安公园内(近静安公园)
走法：地铁2号线静安寺站，5号口出来，下沉式广场东边上来，直走即可（最近5号口封闭，需要从无障碍电梯走）。
内容：没有演讲主题,自由交流
时间：一般19：00~22：00
消费：随意

有任何想学习或者愿意分享的内容，可以回复本帖，让大家事先做好准备。

有关Hacking Thursday活动的介绍：
http://www.shlug.org/about/#hacking-thursday

SHLUG的新浪微博地址：http://weibo.com/shanghailug 有每次活动照片以及信息发布
`
	}

	var body bytes.Buffer
	t, _ := template.New("cm").Parse(Templ)
	t.Execute(&body, &ConfimrMail{uname, hash})
	if holiday {
		send(to, next4()+" 吃吃喝喝Hacking Thursday Night聚餐活动 暂停一次", body.String(), "text")
	} else {
		send(to, next4()+" 吃吃喝喝Hacking Thursday Night聚餐活动 at JAcafe花园咖啡", body.String(), "text")
	}
}

func main() {
	H4Notify("a", "shlug@googlegroups.com", "abc")
}
