package main

import (
	"bytes"
	"net/smtp"
	"strings"
	"text/template"
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
	user := "email@gmail.com"
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

	msg := "From: Shlug <email@gmail.com> \n" +
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

func H4Notify(uname, to, hash string) {

	Templ := `
店名：Dunkin Donuts 唐恩都乐
点评：http://www.dianping.com/shop/21378231
地址：静安区南京西路1649号静安公园内(近静安公园)
走法：地铁2号线静安寺站，5号口出来，下沉式广场东边上来，直走即可（最近5号口封闭，需要从无障碍电梯走）。聚会一般在店内的地下室一层。
内容：没有演讲主题,自由交流
时间：一般19：00~22：00
消费：随意

有任何想学习或者愿意分享的内容，可以回复本帖，让大家事先做好准备。

有关Hacking Thursday活动的介绍：
http://www.shlug.org/about/#hacking-thursday

SHLUG的新浪微博地址：http://weibo.com/shanghailug 有每次活动照片以及信息发布
`

	var body bytes.Buffer
	t, _ := template.New("cm").Parse(Templ)
	t.Execute(&body, &ConfimrMail{uname, hash})
	send(to, next4()+" 吃吃喝喝Hacking Thursday Night聚餐活动 at Dunkin Donuts", body.String(), "text")
}

func main() {
	H4Notify("a", "shlug@googlegroups.com", "abc")
}
