# .PHONY: cmd/controller/validations_generated.go

cmd/controller/controller: cmd/controller/*.go
	cd cmd/controller && go build

cmd/controller/validations_generated.go: lib/testresource/v1beta1/*.go
	./generate.sh

clean:
	rm cmd/controller/controller
