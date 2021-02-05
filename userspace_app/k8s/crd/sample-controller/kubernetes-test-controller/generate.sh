
GOFLAGS= controller-gen crd:allowDangerousTypes=true paths=./lib/testresource/v1beta1/
cd config/crd
for file in *; do
    tail -n +3 $file >$file"new"
    mv $file"new" $file
done
