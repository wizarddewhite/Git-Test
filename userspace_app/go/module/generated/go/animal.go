package animal

var globalName string

func ChangeName(name string) {
	globalName = name
}

func Animal() string {
	return "animal" + globalName
}
