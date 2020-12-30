package main

import (
	"os"

	utilruntime "k8s.io/apimachinery/pkg/util/runtime"
	"k8s.io/klog"
)

func main() {
	klog.Info("Testing...")
	utilruntime.Must(nil)
	klog.Info("pass...")

	_, err := os.Open("file_never_exist")
	utilruntime.Must(err)
	klog.Info("pass...")
}
