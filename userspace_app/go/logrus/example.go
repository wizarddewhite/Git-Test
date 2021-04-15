package main

import (
	"github.com/sirupsen/logrus"
)

func main() {
	log := logrus.WithFields(logrus.Fields{
		"animal":    "walrus",
		"component": "vmapi",
	})
	log.Info("A walrus appears")
	log.Warn("A walrus sick")
	log.Error("A walrus dead")
}
