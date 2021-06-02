// http://networkbit.ch/golang-ssh-client/
package main

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"os"
	"path/filepath"
	"strings"
	"time"

	"golang.org/x/crypto/ssh"
)

func configWithKey(user, pass, host string) *ssh.ClientConfig {
	// get host public key
	// hostKey := getHostKey(host)

	return &ssh.ClientConfig{
		User: user,
		Auth: []ssh.AuthMethod{
			ssh.Password(pass),
		},
		// allow any host key to be used (non-prod)
		HostKeyCallback: ssh.InsecureIgnoreHostKey(),

		// verify host public key
		// HostKeyCallback: ssh.FixedHostKey(hostKey),
		// optional host key algo list
		HostKeyAlgorithms: []string{
			ssh.KeyAlgoRSA,
			ssh.KeyAlgoDSA,
			ssh.KeyAlgoECDSA256,
			ssh.KeyAlgoECDSA384,
			ssh.KeyAlgoECDSA521,
			ssh.KeyAlgoED25519,
		},
		// optional tcp connect timeout
		Timeout: 5 * time.Second,
	}
}

func configJustPass(user, password string) *ssh.ClientConfig {
	return &ssh.ClientConfig{
		User: user,
		Auth: []ssh.AuthMethod{
			ssh.Password(password),
		},
		HostKeyCallback: ssh.HostKeyCallback(func(hostname string, remote net.Addr, key ssh.PublicKey) error {
			return nil
		}),
	}
}

func sessStdOut(sess *ssh.Session, cmd string) {
	// setup standard out and error
	// uses writer interface
	sess.Stdout = os.Stdout
	sess.Stderr = os.Stderr

	// run single command
	err := sess.Run(cmd)
	if err != nil {
		log.Fatal(err)
	}
}

func sessStringOut(sess *ssh.Session, cmd string) {
	sess.Stdout = nil
	sess.Stderr = nil
	b, err := sess.CombinedOutput(cmd)
	if err != nil {
		log.Fatal(err)
	}
	output := string(b)
	// fmt.Println("output from ssh: \n", output)
	outputs := strings.Split(output, "\n")
	for _, o := range outputs {
		split := strings.Split(o, ":")
		if len(split) == 2 && strings.Contains(split[0], "Serial Number") {
			fmt.Println(len(split), split[1])
		}
	}
}

func main() {

	host := "yourhost"
	port := "22"
	user := "root"
	pass := "123456"
	cmd := "dmidecode -t system"

	// ssh client config
	// config := configWithKey(user, pass, host)
	config := configJustPass(user, pass)

	// connect
	client, err := ssh.Dial("tcp", host+":"+port, config)
	if err != nil {
		log.Fatal(err)
	}
	defer client.Close()

	// start session
	sess, err := client.NewSession()
	if err != nil {
		log.Fatal(err)
	}
	defer sess.Close()

	// sessStdOut(sess, cmd)
	sessStringOut(sess, cmd)
}

func getHostKey(host string) ssh.PublicKey {
	// parse OpenSSH known_hosts file
	// ssh or use ssh-keyscan to get initial key
	file, err := os.Open(filepath.Join(os.Getenv("HOME"), ".ssh", "known_hosts"))
	if err != nil {
		log.Fatal(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	var hostKey ssh.PublicKey
	for scanner.Scan() {
		fields := strings.Split(scanner.Text(), " ")
		if len(fields) != 3 {
			continue
		}
		if strings.Contains(fields[0], host) {
			var err error
			hostKey, _, _, _, err = ssh.ParseAuthorizedKey(scanner.Bytes())
			if err != nil {
				log.Fatalf("error parsing %q: %v", fields[2], err)
			}
			break
		}
	}

	if hostKey == nil {
		log.Fatalf("no hostkey found for %s", host)
	}

	return hostKey
}
