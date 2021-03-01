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
		fmt.Println("Get Pod: ", key)
	}
}

func informerPod() {
	sharedInformer := cache.NewSharedIndexInformer(
		&cache.ListWatch{
			ListFunc: func(options metav1.ListOptions) (runtime.Object, error) {
				return coreClient.CoreV1().Pods(v1.NamespaceAll).List(context.TODO(), options)
			},
			WatchFunc: func(options metav1.ListOptions) (watch.Interface, error) {
				return coreClient.CoreV1().Pods(v1.NamespaceAll).Watch(context.TODO(), options)
			},
		},
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

func informerPod_fieldselector() {
	selector := fields.ParseSelectorOrDie("spec.nodeName=" + "localhost.localdomain")
	sharedInformer := cache.NewSharedIndexInformer(
		&cache.ListWatch{
			ListFunc: func(options metav1.ListOptions) (runtime.Object, error) {
				options.FieldSelector = selector.String()
				return coreClient.CoreV1().Pods(v1.NamespaceAll).List(context.TODO(), options)
			},
			WatchFunc: func(options metav1.ListOptions) (watch.Interface, error) {
				options.FieldSelector = selector.String()
				return coreClient.CoreV1().Pods(v1.NamespaceAll).Watch(context.TODO(), options)
			},
		},
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

func informerPod_labelselector() {
	// https://kubernetes.io/docs/concepts/overview/working-with-objects/labels/
	// show how to format the label selector
	label, err := labels.Parse(fmt.Sprintf("k8s-app notin (kube-dns)"))
	if err != nil {
		panic(err)
	}
	sharedInformer := cache.NewSharedIndexInformer(
		&cache.ListWatch{
			ListFunc: func(options metav1.ListOptions) (runtime.Object, error) {
				options.LabelSelector = label.String()
				return coreClient.CoreV1().Pods(v1.NamespaceAll).List(context.TODO(), options)
			},
			WatchFunc: func(options metav1.ListOptions) (watch.Interface, error) {
				options.LabelSelector = label.String()
				return coreClient.CoreV1().Pods(v1.NamespaceAll).Watch(context.TODO(), options)
			},
		},
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
	// informerPod()

	// informerPod_fieldselector()
	informerPod_labelselector()

	select {}
}
