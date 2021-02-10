package main

import (
	"errors"
	"fmt"
	"os"
	"strconv"

	"github.com/spf13/cobra"
)

var (
	nType   string
	Region  string
	dynamic bool
)

var versionCmd = &cobra.Command{
	Use:   "version",
	Short: "Print the version number of Hugo",
	Long:  `All software has versions. This is Hugo's`,
	Args:  cobra.NoArgs,
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("Hugo Static Site Generator v0.9 -- HEAD")
	},
}

var listCmd = &cobra.Command{
	Use:   "list",
	Short: "Print the all elements",
	Long:  `All software has versions. This is Hugo's`,
	Args:  cobra.NoArgs,
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("list all elements on ", Region)
	},
}

var restartCmd = &cobra.Command{
	Use:   "restart",
	Short: "Restart some element",
	Long:  `Restart some element`,
	Args: func(c *cobra.Command, args []string) error {
		if len(args) != 3 {
			return errors.New("restart require vm_name row column")
		}

		if _, err := strconv.Atoi(args[1]); err != nil {
			return fmt.Errorf("%v should be a number", args[1])
		}

		if _, err := strconv.Atoi(args[2]); err != nil {
			return fmt.Errorf("%v should be a number", args[2])
		}
		return nil
	},
	Run: func(cmd *cobra.Command, args []string) {
		fmt.Println("restart elements on ", Region)
	},
}

func init() {
	listCmd.Flags().StringVarP(&Region, "region", "r", "", "AWS region (required)")
}

func newCommand() *cobra.Command {
	cmd := &cobra.Command{
		Use:   "cobra_cmd",
		Short: "cobra command",
		Long:  "cobra command for test",
		Args: func(c *cobra.Command, args []string) error {
			if len(args) < 1 {
				return errors.New("requires a color argument")
			}
			return nil
		},
		PersistentPreRun: func(cmd *cobra.Command, args []string) {
			fmt.Printf("Inside rootCmd PersistentPreRun with args: %v\n", args)
		},
		Run: func(c *cobra.Command, args []string) {
			fmt.Println("command run: ", args)
		},
		PersistentPostRun: func(cmd *cobra.Command, args []string) {
			fmt.Printf("Inside rootCmd PersistentPostRun with args: %v\n", args)
		},
	}

	flags := cmd.Flags()
	flags.StringVar(&nType, "ntype", "", "What type of network to run")
	cmd.MarkFlagRequired("ntype")

	flags.BoolVar(&dynamic, "dynamic", false, "Dynamic config")

	cmd.AddCommand(versionCmd)
	cmd.AddCommand(listCmd)
	cmd.AddCommand(restartCmd)

	return cmd
}

func main() {
	cmd := newCommand()

	if err := cmd.Execute(); err != nil {
		os.Exit(1)
	}
}
