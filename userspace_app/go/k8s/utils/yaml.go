package main

import (
	"fmt"
	"os"
	"strings"

	v1 "k8s.io/api/core/v1"
	k8syaml "k8s.io/apimachinery/pkg/util/yaml"
)

func getPodFromFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		panic(fmt.Errorf("Failed to read file %v, %v", filename, err))
	}
	defer file.Close()

	pod := v1.Pod{}
	err = k8syaml.NewYAMLToJSONDecoder(file).Decode(&pod)
	if err != nil {
		panic(fmt.Errorf("Failed to parse pod from file %v, %v", filename, err))
	}
	fmt.Printf("Pod.Name: %s\n", pod.Name)
}

const template = `
apiVersion: v1
kind: Pod
metadata:
  name: nginx-app
  labels:
    run: nginx-app
spec:
  containers:
  - name: nginx
    env:
    - name: USER
      value: "test"
    image: nginx:latest
    ports:
    - containerPort: 80
`

func getPodFromTemplate(template string) {

	pod := v1.Pod{}
	err := k8syaml.NewYAMLToJSONDecoder(strings.NewReader(template)).Decode(&pod)
	if err != nil {
		panic(fmt.Errorf("Failed to parse pod from template %v, %v", template, err))
	}
	fmt.Printf("Pod.Name: %s\n", pod.Name)
}

func main() {
	getPodFromFile("pod.yaml")
	getPodFromTemplate(template)
}
