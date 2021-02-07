GOFLAGS= controller-gen crd:allowDangerousTypes=true paths=./lib/testresource/v1beta1/
for file in config/crd/*; do
    tail -n +3 $file >$file"new"
    mv $file"new" $file
done

cd tools/crd-validation-generator/ && go build
cd ../../
tools/crd-validation-generator/crd-validation-generator

# remove the generated crd.yaml
rm -rf config
rm tools/crd-validation-generator/crd-validation-generator
