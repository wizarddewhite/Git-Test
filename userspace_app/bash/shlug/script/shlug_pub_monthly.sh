#!/usr/bin/env bash

# avoid cross day boundary
export TZ=GMT

RES_REPO=$HOME/proj/doc/res`date +%Y`
WEB_REPO=$HOME/proj/doc/shanghailug.github.io
TN_SIZE=240x160

RES_REMOTE="https://raw.githubusercontent.com/shanghailug/res`date +%Y`"

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

fmt() {
  y=`date +%-y`
  m=`date +%-m`
  d=`date +%d`
  t="0123456789abcdefghijklmnopqrstuvwxyz"
  echo "${t:$y:1}${t:$m:1}$d"
}

for i in "$@"; do
  echo "publish $i"
done
confirm "continue " || exit

dir="`fmt`.monthly"

dst="$RES_REPO/$dir"

mkdir -p "$dst"

echo "copy images to '$dst'"

cp "$@" "$dst"

cd "$dst"

trans() {
    a=${1%.jpg}
    b=${a%.JPG}
    sz=$2
    echo "${b}.${sz}.jpg"
}

for i in "$@"; do
    sz=$TN_SIZE
    c=`trans $i $sz`
    echo "$i -> $c"
    convert -strip "$i" -resize $sz "$c"
done

git add *.JPG
git add *.jpg

git commit -a -m "Add photos for Monthly Meetup of $(date +%F)"

confirm "push to remote" || exit

git push

echo "------ now do post ---------"

prefix=`date +%Y-%m-%d`
post_file="$WEB_REPO/_posts/${prefix}-monthly-photo.markdown"

echo "---
layout: post
title:  \"本月月度交流分享活动照片\"
date:   $(date '+%F %H:%M:%S %z')
categories: monthly
---
" > $post_file

for i in $@; do
  j=`trans $i $TN_SIZE`
  echo "[<img src='$RES_REMOTE/master/$dir/$j'>]($RES_REMOTE/master/$dir/$i)" \
    >> $post_file
done


echo "
有关Hacking Thursday活动的介绍：
http://www.shlug.org/?page_id=523

SHLUG的新浪微博地址：http://weibo.com/shanghailug 有每次活动照片以及信息发布

" >> $post_file

echo "$post_file"
echo ">>>>>>>>>>>>>"

cat $post_file

confirm "post " || exit

cd "$WEB_REPO"

git add "$post_file"
git commit -m "Post monthly meetup photos for `date +%F`"

git push

