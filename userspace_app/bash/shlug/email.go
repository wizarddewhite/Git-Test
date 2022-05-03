package main

import (
	"fmt"
	"io/ioutil"
	"os"
	"os/exec"
	"time"
)

func next4() string {
	now := time.Now()
	weekday := int(now.Weekday())
	n4 := now.AddDate(0, 0, 4-weekday)
	return n4.Format("2006/01/02")
}

var Head = `From: Shlug <wizarddewhite@gmail.com>
To: Shlug <shlug@googlegroups.com>
`

var Body = [2]string{
	`，本期活动取消。

有关Hacking Thursday活动的介绍：
http://www.shlug.org/about/#hacking-thursday

SHLUG的新浪微博地址：http://weibo.com/shanghailug 有每次活动照片以及信息发布
`,
	`
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
`,
}

func H4Notify(to string) {
	check_res := "0"
	if len(os.Args) == 2 {
		check_res = os.Args[1]
	}
	reason := ""
	switch check_res {
	case "1":
		reason = "因为其他原因"
	case "2":
		reason = "欢度假期"
	}
	body := ``
	if reason != "" {
		body += "Subject: " + next4() + " 暂停本周Hacking Thursday Night活动\n"
		body += reason + Body[0]
	} else {
		body += "Subject: " + next4() + " 吃吃喝喝Hacking Thursday Night聚餐活动 at JAcafe花园咖啡\n"
		body += Body[1]
	}

	ioutil.WriteFile("shlug_mail", []byte(Head+body), 0644)

	cmd := "cat shlug_mail | msmtp " + to
	exec.Command("bash", "-c", cmd).Output()

	fmt.Println("")
}

func main() {
	// H4Notify("shlug@googlegroups.com")
	H4Notify("wizarddewhite@gmail.com")
}
