// Example showing how to patch kubernetes resources.
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
	replicationControllerName = "my-rc"
	replicas                  = uint32(3)
)

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

func scaleReplicationController(clientSet *kubernetes.Clientset, replicasetName string, scale uint32) error {
	payload := []patchUInt32Value{{
		Op:    "replace",
		Path:  "/spec/replicas",
		Value: scale,
	}}
	payloadBytes, _ := json.Marshal(payload)
	_, err := clientSet.
		CoreV1().
		ReplicationControllers("default").
		Patch(context.TODO(), replicasetName, types.JSONPatchType, payloadBytes, metav1.PatchOptions{})
	return err
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

	//  Scale our replication controller.
	fmt.Printf("Scaling replication controller %v to %v\n", replicationControllerName, replicas)
	err = scaleReplicationController(clientset, replicationControllerName, replicas)
	if err != nil {
		panic(err.Error())
	}
}
