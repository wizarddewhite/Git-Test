package main

import (
	"context"
	"fmt"
	"os"

	"k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/fields"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	coreClient *kubernetes.Clientset
)

func init() {
	kubeconfig := os.Getenv("KUBECONFIG")

	config, err := clientcmd.BuildConfigFromFlags("", kubeconfig)
	if err != nil {
		panic(err)
	}
	coreClient, err = kubernetes.NewForConfig(config)
	if err != nil {
		panic(err)
	}
}

func listPod(namespace string) {
	list, err := coreClient.CoreV1().Pods(namespace).List(context.TODO(), metav1.ListOptions{})
	if err != nil {
		return
	}

	for i := range list.Items {
		pod := &list.Items[i]
		fmt.Printf("\t %-40s %s\n", pod.Name, pod.Spec.NodeName)
	}
}

func listPodOnNode(nodeName string) {
	handlerNodeSelector := fields.ParseSelectorOrDie("spec.nodeName=" + nodeName)
	list, err := coreClient.CoreV1().Pods(v1.NamespaceAll).
		List(context.TODO(), metav1.ListOptions{
			FieldSelector: handlerNodeSelector.String(),
		})
	if err != nil {
		return
	}

	for i := range list.Items {
		pod := &list.Items[i]
		fmt.Printf("\t %-40s %s\n", pod.Name, pod.Spec.NodeName)
	}
}

func main() {
	fmt.Println("Pods in NamespaceAll")
	listPod(v1.NamespaceAll)
	fmt.Println("Pods in NamespaceDefault")
	listPod(v1.NamespaceDefault)
	fmt.Println("Pods on localhost.localdomain")
	listPodOnNode("localhost.localdomain")
}
