package main

import (
	"context"
	"fmt"
	"os"

	"k8s.io/api/core/v1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/fields"
	"k8s.io/apimachinery/pkg/labels"
	"k8s.io/apimachinery/pkg/runtime"
	"k8s.io/apimachinery/pkg/runtime/schema"
	"k8s.io/apimachinery/pkg/runtime/serializer"
	"k8s.io/apimachinery/pkg/util/wait"
	"k8s.io/apimachinery/pkg/watch"
	"k8s.io/client-go/kubernetes"
	"k8s.io/client-go/kubernetes/scheme"
	"k8s.io/client-go/rest"
	"k8s.io/client-go/tools/cache"
	"k8s.io/client-go/tools/clientcmd"
)

var (
	Scheme = runtime.NewScheme()
	Codecs = serializer.NewCodecFactory(Scheme)

	coreClient       *kubernetes.Clientset
	restClient       *rest.RESTClient
	hostName         string
	testGroupVersion = schema.GroupVersion{Group: "apps", Version: "v1"}
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

	config.GroupVersion = &testGroupVersion
	// config.NegotiatedSerializer = serializer.WithoutConversionCodecFactory{CodecFactory: Codecs}
	config.NegotiatedSerializer = scheme.Codecs.WithoutConversion()
	// config.APIPath = "/apis"
	if config.UserAgent == "" {
		config.UserAgent = rest.DefaultKubernetesUserAgent()
	}
	restClient, err = rest.RESTClientFor(config)
	if err != nil {
		panic(err)
	}

	hostName, err = os.Hostname()
	if err != nil {
		panic(err)
	}
}

func NewListWatchFromClient(c cache.Getter, resource string, namespace string, fieldSelector fields.Selector, labelSelector labels.Selector) *cache.ListWatch {
	listFunc := func(options metav1.ListOptions) (runtime.Object, error) {
		options.FieldSelector = fieldSelector.String()
		options.LabelSelector = labelSelector.String()
		return c.Get().
			Namespace(namespace).
			Resource(resource).
			VersionedParams(&options, metav1.ParameterCodec).
			Do(context.TODO()).
			Get()
	}
	watchFunc := func(options metav1.ListOptions) (watch.Interface, error) {
		options.FieldSelector = fieldSelector.String()
		options.LabelSelector = labelSelector.String()
		return c.Get().
			Prefix("watch").
			Namespace(namespace).
			Resource(resource).
			VersionedParams(&options, metav1.ParameterCodec).
			Watch(context.TODO())
	}
	return &cache.ListWatch{ListFunc: listFunc, WatchFunc: watchFunc}
}

func addFunc(obj interface{}) {
	key, err := cache.DeletionHandlingMetaNamespaceKeyFunc(obj)
	if err == nil {
		fmt.Println(key)
	}
}

func informerPod() {
	sharedInformer := cache.NewSharedIndexInformer(
		NewListWatchFromClient(restClient, "pods", v1.NamespaceAll, fields.Everything(), labels.Everything()),
		&v1.Pod{},
		0,
		cache.Indexers{},
	)

	sharedInformer.AddEventHandler(cache.ResourceEventHandlerFuncs{
		AddFunc:    addFunc,
		DeleteFunc: nil,
		UpdateFunc: nil,
	})

	sharedInformer.Run(wait.NeverStop)

	cache.WaitForCacheSync(wait.NeverStop, sharedInformer.HasSynced)
}

func main() {
	// Label, err := labels.Parse(fmt.Sprintf("vmk8s.io/nodeName"+" in (%s)", hostName))
	// if err != nil {
	// 	panic(err)
	// }
	informerPod()

	select {}
}
