package main

import (
	"context"
	"fmt"
	"os"

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

func listConfiMap(namespace string) {
	list, err := coreClient.CoreV1().ConfigMaps(namespace).List(context.TODO(), metav1.ListOptions{})
	if err != nil {
		return
	}

	for i := range list.Items {
		cm := &list.Items[i]
		fmt.Printf("\t %-40s\n", cm.Name)
	}
}

func createConfiMap(namespace string) {
	cm := &v1.ConfigMap{
		ObjectMeta: metav1.ObjectMeta{
			Name: "create-cm",
		},
		Data: map[string]string{
			"CREATE-TEST": "VALUE1",
		},
	}

	_, err := coreClient.CoreV1().ConfigMaps(namespace).Create(context.TODO(), cm, metav1.CreateOptions{})
	if err != nil {
		fmt.Printf("CM creation failed: %s\n", err.Error())
	}
}

func main() {
	// fmt.Println("ConfigMap in NamespaceAll")
	// listConfiMap(v1.NamespaceAll)
	fmt.Println("ConfigMap in Default")
	listConfiMap(v1.NamespaceDefault)
	createConfiMap(v1.NamespaceDefault)
	fmt.Println("ConfigMap in Default")
	listConfiMap(v1.NamespaceDefault)
}
