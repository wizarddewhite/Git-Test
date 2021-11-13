#!/bin/bash

cat << EOF
apiVersion: v1
kind: PersistentVolumeClaim
metadata:
  name: datavol-$1
spec:
  storageClassName: local-ssd
  accessModes:
    - ReadWriteOnce
  resources:
    requests:
      storage: 50Gi
---
apiVersion: v1
kind: Pod
metadata:
  name: $1
  labels:
    run: vcont-yw
spec:
  runtimeClassName: kata
  nodeSelector:
    node.test.net/role: vm-host
  containers:
  - name: vcont-yw
    image: longtime-test
    command: ["/bin/bash"]
    args: ["-c", "while true; do sleep 1; done"]
    securityContext:
      privileged: true
    volumeMounts:
    - name: datavol
      mountPath: /data0
  volumes:
    - name: datavol
      persistentVolumeClaim:
        claimName: datavol-$1
EOF
