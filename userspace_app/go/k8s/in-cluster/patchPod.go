// Example showing how to patch kubernetes resources.
// This is copied from:
// This is the companion to my article 'Patching Kubernetes Resources in Golang':
//   https://dwmkerr.com/patching-kubernetes-resources-in-golang/
package main

import (
	"context"
	"encoding/json"
	"fmt"

	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	types "k8s.io/apimachinery/pkg/types"
	"k8s.io/client-go/kubernetes"
	_ "k8s.io/client-go/plugin/pkg/client/auth"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	//  Leave blank for the default context in your kube config.
	ctx = ""

	//  Name of the replication controller to scale, and the desired number of replicas.
	podName = "coredns-74ff55c5b-zjtwz"
)

type patchParam struct {
	ClientSet    *kubernetes.Clientset
	PodName      string
	OperatorType string
	OperatorPath string
	OperatorData string
}

//  patchStringValue specifies a patch operation for a string.
type patchStringValue struct {
	Op    string `json:"op"`
	Path  string `json:"path"`
	Value string `json:"value"`
}

//  patchUint32Value specifies a patch operation for a uint32.
type patchUInt32Value struct {
	Op    string `json:"op"`
	Path  string `json:"path"`
	Value uint32 `json:"value"`
}

func jsonpatchPod(param patchParam) error {
	clientSet := param.ClientSet
	podName := param.PodName

	payload := []patchStringValue{{
		Op:    param.OperatorType,
		Path:  param.OperatorPath,
		Value: param.OperatorData,
	}}

	// retrieve the pod first
	pod, err := clientSet.CoreV1().Pods("kube-system").Get(context.TODO(), podName, metav1.GetOptions{})
	fmt.Println("Get Pod: ", pod.Name)
	if err != nil {
		return err
	}

	payloadBytes, _ := json.Marshal(payload)
	// to add a pod label
	_, err = clientSet.
		CoreV1().
		Pods("kube-system").
		Patch(context.TODO(), podName, types.JSONPatchType, payloadBytes, metav1.PatchOptions{})
	return err
}

func jsonPatch_test(clientset *kubernetes.Clientset) {
	fmt.Printf("Patch pod %v \n", podName)

	params := []patchParam{
		{
			ClientSet:    clientset,
			PodName:      "coredns-74ff55c5b-zjtwz",
			OperatorType: "add",
			OperatorPath: "/metadata/labels/test",
			OperatorData: "abcd",
		},
	}

	for _, param := range params {
		err := jsonpatchPod(param)
		if err != nil {
			panic(err.Error())
		}
	}
}

func main() {
	//  Get the local kube config.
	fmt.Printf("Connecting to Kubernetes Context %v\n", ctx)
	config, err := clientcmd.NewNonInteractiveDeferredLoadingClientConfig(
		clientcmd.NewDefaultClientConfigLoadingRules(),
		&clientcmd.ConfigOverrides{CurrentContext: ctx}).ClientConfig()
	if err != nil {
		panic(err.Error())
	}

	// Creates the clientset
	clientset, err := kubernetes.NewForConfig(config)
	if err != nil {
		panic(err.Error())
	}

	jsonPatch_test(clientset)
}
