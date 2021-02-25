This is a directory for example code snippet for k8s, so that we can test and
show the effect of the program.

1. Preparation

Usually, we need go env for test.

2. Run

For most examples, I think the following command would give you what expected.

$ go run xxx.go

For cases run in cluster, we should specify the environment:

e.g.

$ export KUBECONFIG=/root/.kube/config
$ go run listPod.go
