#!/usr/bin/env bash

#WEB_REPO=$HOME/proj/doc/shanghailug.github.io
WEB_REPO=$PWD/../shanghailug.github.io

confirm () {
    # call with a prompt string or use a default
    read -r -p "${1:-Are you sure?} [Y/n] " response
    case $response in
        [nN][oO]|[nN])
            false
            ;;
        *)
            true
            ;;
    esac
}

echo "------ do post ---------"
next_thursday() {
    fmt=$1
    for i in `seq 1 7`; do
        day="$(date +%F) +${i} days"
        if [ X$(date -d "$day" +%w) = X4 ]; then
            date -d "$day" "+${fmt}"
        fi
    done
}

prefix=`date +%F`
h4_date=`next_thursday "%Y/%m/%d"`
post_file="$WEB_REPO/_posts/${prefix}-h4.markdown"
./check.sh
check_res=`echo $?`

case $check_res in
    "1") reason="因为其他原因" ;;
    "2") reason="欢度假期" ;;
    "0") reason="" ;;
    *) exit 1
esac

if [ "X$check_res" != "X0" ]
then
echo "---
layout: post
title:  \"${h4_date} 暂停本周Hacking Thursday Night活动\"
date:   $(date '+%F %H:%M:%S %z')
categories: h4
---
${reason}，本期活动取消。

有关Hacking Thursday活动的介绍：
[http://www.shlug.org/about/#hacking-thursday](http://www.shlug.org/about/#hacking-thursday)

SHLUG的新浪微博地址：[http://weibo.com/shanghailug](http://weibo.com/shanghailug) 有每次活动照片以及信息发布
" > $post_file
else
echo "---
layout: post
title:  \"${h4_date} 吃吃喝喝Hacking Thursday Night聚餐活动 at JAcafe花园咖啡\"
date:   $(date '+%F %H:%M:%S %z')
categories: h4
---
- 店名：JAcafe花园咖啡
- 点评：[http://www.dianping.com/shop/2019466](http://www.dianping.com/shop/2019466)
- 地址：静安区南京西路1649号静安公园内(近静安公园)
- 走法：地铁2号线静安寺站，4号口出来，左转（不用过马路）约80米。
- 内容：没有演讲主题,自由交流
- 时间：一般19：00~22：00
- 消费：随意

有任何想学习或者愿意分享的内容，请在邮件列表回复本帖，让大家事先做好准备。

有关Hacking Thursday活动的介绍：
[http://www.shlug.org/about/#hacking-thursday](http://www.shlug.org/about/#hacking-thursday)

SHLUG的新浪微博地址：[http://weibo.com/shanghailug](http://weibo.com/shanghailug) 有每次活动照片以及信息发布
" > $post_file

fi

echo "$post_file"
echo ">>>>>>>>>>>>>"

cat $post_file

confirm "post " || exit

cd "$WEB_REPO"

git add "$post_file"
git commit -m "Post H4 announcement for `date +%F`"

git fetch
git rebase origin/master
git push
