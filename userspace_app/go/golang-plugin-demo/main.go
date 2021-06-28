package main

import (
	"log"
	"plugin"

	"./types"
)

func LoadPlugins(plugins []string) {

	d := types.InData{V: 1}
	log.Printf("Invoking pipeline with data: %#v\n", d)
	o := types.OutData{}
	for _, p := range plugins {
		p, err := plugin.Open(p)
		if err != nil {
			log.Fatal(err)
		}
		pName, err := p.Lookup("Name")
		if err != nil {
			panic(err)
		}
		log.Printf("Invoking plugin: %s\n", *pName.(*string))

		input, err := p.Lookup("Input")
		if err != nil {
			panic(err)
		}
		f, err := p.Lookup("F")
		if err != nil {
			panic(err)
		}

		*input.(*types.InData) = d
		f.(func())()

		output, err := p.Lookup("Output")
		if err != nil {
			panic(err)
		}
		// Feed the output to the next plugin's input
		d = types.InData{V: output.(*types.OutData).V}
		*input.(*types.InData) = d

		o = *output.(*types.OutData)
	}
	log.Printf("Final result: %#v\n", o)
}

func main() {
	plugins := []string{"plugin1/plugin1.so", "plugin2/plugin2.so"}
	LoadPlugins(plugins)
}
