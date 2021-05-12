package main

import (
	"context"
	"encoding/json"
	"fmt"
	"os"

	"k8s.io/api/core/v1"
	"k8s.io/apimachinery/pkg/api/errors"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	coreClient *kubernetes.Clientset
)

type VM struct {
	ID string `json:"id"`
	SN string `json:"sn"`
}

var vms = []VM{
	VM{ID: "abd", SN: "786"},
}

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
	data, _ := json.Marshal(vms)
	cm := &v1.ConfigMap{
		ObjectMeta: metav1.ObjectMeta{
			Name: "create-cm",
		},
		Data: map[string]string{
			"CREATE-TEST": string(data),
		},
		BinaryData: map[string][]byte{
			"vm-config": data,
		},
	}

	_, err := coreClient.CoreV1().ConfigMaps(namespace).Create(context.TODO(), cm, metav1.CreateOptions{})
	if err != nil {
		if errors.IsAlreadyExists(err) {
			fmt.Println("Already exists")
		} else {
			fmt.Printf("CM creation failed: %s\n", err.Error())
		}
	}
}

func updateConfiMap(namespace string) {
	cm, err := coreClient.CoreV1().ConfigMaps(namespace).Get(context.TODO(), "create-cm", metav1.GetOptions{})
	if err != nil {
		fmt.Println("No cm create-cm")
		return
	}

	var localVMs []VM
	err = json.Unmarshal([]byte(cm.Data["CREATE-TEST"]), &localVMs)
	if err != nil {
		fmt.Println("Failed to unmarshal configmap, ", err.Error())
		return
	}

	vm := VM{ID: "qwe", SN: "110"}
	localVMs = append(localVMs, vm)
	data, _ := json.Marshal(localVMs)
	cm.Data["CREATE-TEST"] = string(data)

	_, err = coreClient.CoreV1().ConfigMaps(namespace).Update(context.TODO(), cm, metav1.UpdateOptions{})
	if err != nil {
		fmt.Printf("CM update failed: %s\n", err.Error())
	}
}

func deleteConfiMap(namespace string) {
	err := coreClient.CoreV1().ConfigMaps(namespace).
		Delete(context.TODO(), "create-cm", metav1.DeleteOptions{})
	if err != nil {
		if errors.IsNotFound(err) {
			fmt.Println("Not Found")
		} else {
			fmt.Println("Delete error, ", err.Error())
		}
	}
}

func main() {
	// fmt.Println("ConfigMap in NamespaceAll")
	// listConfiMap(v1.NamespaceAll)
	fmt.Println("ConfigMap in Default")
	listConfiMap(v1.NamespaceDefault)
	// createConfiMap(v1.NamespaceDefault)
	updateConfiMap(v1.NamespaceDefault)
	fmt.Println("ConfigMap in Default")
	listConfiMap(v1.NamespaceDefault)
}
