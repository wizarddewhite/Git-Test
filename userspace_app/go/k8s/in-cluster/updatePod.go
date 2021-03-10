package main

import (
	"context"
	"fmt"
	"os"
	"time"

	"k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
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

func listPod(namespace string) *v1.Pod {
	list, err := coreClient.CoreV1().Pods(namespace).List(context.TODO(), metav1.ListOptions{})
	if err != nil {
		return nil
	}

	pod := &list.Items[0]
	fmt.Printf("\t %-40s %s %s\n", pod.Name, pod.Spec.NodeName, pod.ResourceVersion)
	return pod
}

func updatePod(pod *v1.Pod) {
	pod.Labels["node"] = "Mars"
	_, err := coreClient.CoreV1().Pods(pod.Namespace).Update(context.TODO(), pod, metav1.UpdateOptions{})
	if err != nil {
		fmt.Println("Update error: ", err)
		return
	}
	fmt.Println("Update succeed ")
}

func main() {
	fmt.Println("Pods in NamespaceAll")
	pod := listPod(v1.NamespaceAll)

	time.Sleep(5 * time.Second)

	updatePod(pod)
}
