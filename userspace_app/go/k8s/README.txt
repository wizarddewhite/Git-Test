This is a directory for example code snippet for k8s, so that we can test and
show the effect of the program.

1. Preparation

Usually, we need go env for test.

2. Run

There are two class of examples:

   * util
   * in-cluster

Examples in util/, they could be run out side cluster. So we just need to run:

$ go run xxx.go

For examples in-cluster/, we should run in cluster and specify the environment:

e.g.

$ export KUBECONFIG=/root/.kube/config
$ go run listPod.go
