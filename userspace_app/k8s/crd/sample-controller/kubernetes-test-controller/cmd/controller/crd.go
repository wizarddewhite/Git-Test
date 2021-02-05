package main

import (
	"fmt"
	"strings"
	"time"

	testresourcev1beta1 "insujang.github.io/kubernetes-test-controller/lib/testresource/v1beta1"
	apiextensions "k8s.io/apiextensions-apiserver/pkg/apis/apiextensions/v1beta1"
	metav1 "k8s.io/apimachinery/pkg/apis/meta/v1"
	"k8s.io/apimachinery/pkg/util/wait"
	k8syaml "k8s.io/apimachinery/pkg/util/yaml"
	"k8s.io/klog"
)

func patchValidation(crd *apiextensions.CustomResourceDefinition) error {
	name := crd.Spec.Names.Singular

	validation, ok := CRDsValidation[name]
	if !ok {
		return nil
	}
	crvalidation := apiextensions.CustomResourceValidation{}
	err := k8syaml.NewYAMLToJSONDecoder(strings.NewReader(validation)).Decode(&crvalidation)
	if err != nil {
		return fmt.Errorf("Couldn't decode validation for %s, %v", name, err)
	}
	crd.Spec.Validation = &crvalidation
	return nil
}

func (c *Controller) doesCRDExist() (bool, error) {
	crd, err := c.apiextensionsclientset.ApiextensionsV1beta1().CustomResourceDefinitions().Get(testresourcev1beta1.Name, metav1.GetOptions{})

	if err != nil {
		return false, err
	}

	// Check whether the CRD is accepted.
	for _, condition := range crd.Status.Conditions {
		if condition.Type == apiextensions.Established &&
			condition.Status == apiextensions.ConditionTrue {
			return true, nil
		}
	}

	return false, fmt.Errorf("CRD is not accepted")
}

func (c *Controller) waitCRDAccepted() error {
	err := wait.Poll(1*time.Second, 10*time.Second, func() (bool, error) {
		return c.doesCRDExist()
	})

	return err
}

// CreateCRD creates a custom resource definition,
// named TestResource.
func (c *Controller) CreateCRD() error {
	if result, _ := c.doesCRDExist(); result {
		return nil
	}

	crd := &apiextensions.CustomResourceDefinition{
		ObjectMeta: metav1.ObjectMeta{
			Name: testresourcev1beta1.Name,
		},
		Spec: apiextensions.CustomResourceDefinitionSpec{
			Group:   testresourcev1beta1.GroupName,
			Version: testresourcev1beta1.Version,
			Scope:   apiextensions.NamespaceScoped,
			Names: apiextensions.CustomResourceDefinitionNames{
				Plural:     testresourcev1beta1.Plural,
				Singular:   testresourcev1beta1.Singluar,
				Kind:       testresourcev1beta1.Kind,
				ShortNames: []string{testresourcev1beta1.ShortName},
			},
			AdditionalPrinterColumns: []apiextensions.CustomResourceColumnDefinition{
				{
					Name:     "command",
					Type:     "string",
					JSONPath: ".spec.command",
				},
			},
		},
	}

	if err := patchValidation(crd); err != nil {
		klog.Fatalf(err.Error())
	}

	_, err := c.apiextensionsclientset.ApiextensionsV1beta1().CustomResourceDefinitions().Create(crd)

	if err != nil {
		klog.Fatalf(err.Error())
	}

	return c.waitCRDAccepted()
}
