package main

import (
	"fmt"
	"regexp"
)

var urls = []string{
	"/resource/cluster",
	"/resource/cluster/",
	"/resource/cluster/abc",
	"/resource/cluster/range",
	"/resource/user/",
	"/",
	"/api/v1/resource/cluster/range",
}
var patterns = []string{
	// "/resource/cluster/*",
	// "/resource/cluster/",
	// "/resource/cluster",
	"/resource/cluster/(range|abd|abcd)",
	"/resource/cluster/[range|abd|abcd]",
}

func regex_match() {
	for _, pattern := range patterns {
		fmt.Println("Match patten ", pattern)
		for _, url := range urls {
			match, _ := regexp.MatchString(pattern, url)
			if match == true {
				fmt.Printf("\t%s matched \n", url)
			} else {
				fmt.Printf("\t%s doesn't match \n", url)
			}
		}
	}
}

func regex_compile() {
	for _, pattern := range patterns {
		fmt.Println("Match patten ", pattern)
		for _, url := range urls {
			re := regexp.MustCompile(pattern)
			match := re.FindStringIndex(url)
			fmt.Println("url: ", url, "Match: ", match)
		}
	}
}

func main() {
	regex_match()
	regex_compile()
}
