package main

import (
	"fmt"
	"strings"
)

func main() {
	//使用 strings.Replace() 函数，替换字符串
	strHaiCoder := "`HaiCoder` abc HaiCoder"

	fmt.Println("StrReplace =", strings.Replace(strHaiCoder, "HaiCoder", "haicoder", -1))
	fmt.Println("StrReplace =", strings.Replace(strHaiCoder, "`", "\"", -1))
}
