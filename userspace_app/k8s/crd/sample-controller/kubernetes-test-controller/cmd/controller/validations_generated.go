package main

var CRDsValidation map[string]string = map[string]string{
 "testresource" : `openAPIV3Schema:
  description: TestResource describes a TestResource custom resource.
  properties:
    apiVersion:
      description: 'APIVersion defines the versioned schema of this representation
        of an object. Servers should convert recognized schemas to the latest internal
        value, and may reject unrecognized values. More info: https://git.k8s.io/community/contributors/devel/sig-architecture/api-conventions.md#resources'
      type: string
    kind:
      description: 'Kind is a string value representing the REST resource this object
        represents. Servers may infer this from the endpoint the client submits requests
        to. Cannot be updated. In CamelCase. More info: https://git.k8s.io/community/contributors/devel/sig-architecture/api-conventions.md#types-kinds'
      type: string
    metadata:
      type: object
    spec:
      description: TestResourceSpec specifies the 'spec' of TestResource CRD.
      properties:
        command:
          type: string
        customProperty:
          type: string
      required:
      - command
      - customProperty
      type: object
    status:
      description: TestResourceStatus specifies the 'status' of TestResource CRD.
      properties:
        status:
          type: string
      required:
      - status
      type: object
  required:
  - spec
  - status
  type: object
`,
}
