package main

import (
	"fmt"
	"time"

	"k8s.io/apimachinery/pkg/util/wait"
	"k8s.io/klog"
	"k8s.io/sample-controller/pkg/signals"
)

func process() {
	fmt.Println("Processing...")
}

func main() {
	stopCh := signals.SetupSignalHandler()

	// run process() every 4 seconds
	klog.Info("Starting process")
	go wait.Until(process, 2*time.Second, stopCh)

	<-stopCh
	klog.Info("Receive signal and quit")
}
