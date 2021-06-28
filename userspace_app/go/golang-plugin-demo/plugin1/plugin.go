package main

import "../types"

var Input types.InData
var Output types.OutData
var Name string

func init() {
	Name = "plugin1"
}

func process() types.OutData {
	o := types.OutData{V: Input.V * 2}
	return o
}
func F() {
	Output = process()
}
