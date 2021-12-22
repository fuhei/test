package main

import (
	"fmt"
	"os/exec"
	"time"
)

func runGetExit(cmd string, args ...string) (int, string, error) {
	tmp, err := exec.Command(cmd, args...).CombinedOutput()
	logs := string(tmp)
	if err == nil {
		return 0, logs, nil
	}
	exitErr, ok := err.(*exec.ExitError)
	if !ok {
		return -1, logs, err
	}
	status := exitErr.ExitCode()
	return status, logs, nil
}

func main() {
	var path = "/home/rongtao" string
	fmt.Println("Hello")

	if unmounted, _, err := runGetExit("mountpoint", "-q", path); err != nil {
		return fmt.Errorf("exec'ing mountpoint -q %s failed: %v", path, err)
	}
	//TODO
	return fmt.Errorf("umount failed after 3 tries for %s: ", path)
}
