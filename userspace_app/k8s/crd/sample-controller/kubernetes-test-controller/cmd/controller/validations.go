package main

var CRDsValidation map[string]string = map[string]string{
	"virtualmachine": `openAPIV3Schema:
  properties:
    spec:
      type: object
      properties:
        command:
          type: string
        customProperty:
          type: string
      required:
      - command
      - customProperty
`,
}
