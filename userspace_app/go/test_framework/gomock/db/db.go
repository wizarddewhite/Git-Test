package main

import (
	"fmt"
	"log"
)

//定义了一个订单接口，有一个获取名称的方法
type OrderDBI interface {
	GetName(orderid int) string
}

//定义结构体
type OrderInfo struct {
	orderid int
}

//实现接口的方法
func (order OrderInfo) GetName(orderid int) string {
	log.Println("原本应该连接数据库去取名称")
	return "xdcute"
}

func main() {
	//创建接口实例
	var orderDBI OrderDBI
	orderDBI = new(OrderInfo)
	//调用方法，返回名称
	ret := orderDBI.GetName(1)
	fmt.Println("取到的用户名:", ret)
}
