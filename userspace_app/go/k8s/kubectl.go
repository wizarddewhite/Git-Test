package main

import (
	// "io/ioutil"
	// "os"

	"k8s.io/kubectl/pkg/cmd"
	// "k8s.io/kubectl/pkg/cmd/util"
)

func main() {
	// kubectl := cmd.NewKubectlCommand(util.NewFactory(nil), os.Stdin, ioutil.Discard, ioutil.Discard)
	kubectl := cmd.NewDefaultKubectlCommand()
	kubectl.GenBashCompletionFile("out.sh")
}
