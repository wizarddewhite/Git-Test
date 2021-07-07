#!/bin/bash

cat << EOF
apiVersion: v1
kind: Pod
metadata:
  name: $1
  labels:
    run: vcont-yw
spec:
  runtimeClassName: kata
  containers:
  - name: vcont-yw
    image: nginx:latest
    securityContext:
      privileged: true
EOF
