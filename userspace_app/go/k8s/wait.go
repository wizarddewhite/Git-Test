package main

import (
	"fmt"
	"time"

	"k8s.io/apimachinery/pkg/util/wait"
	"k8s.io/klog"
	"k8s.io/sample-controller/pkg/signals"
)

var (
	stopCh <-chan struct{}
)

func process() {
	fmt.Println("Processing...")
}

func until() {
	// run process() every 4 seconds
	klog.Info("Starting process")
	go wait.Until(process, 2*time.Second, stopCh)

	<-stopCh
	klog.Info("Receive signal and quit")
}

func poll_until() {
	count := 5

	// PollImmediateUntil tries a condition func until it returns true
	wait.PollImmediateUntil(100*time.Millisecond,
		func() (bool, error) {
			fmt.Println("poll immediate until ", count)
			count--
			if count != 0 {
				return false, nil
			}
			return true, nil
		},
		stopCh)
}

func main() {
	// until()
	poll_until()
}

func init() {
	stopCh = signals.SetupSignalHandler()
}
