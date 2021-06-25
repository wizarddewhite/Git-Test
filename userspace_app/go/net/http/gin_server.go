// https://github.com/gin-gonic/gin
package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
)

func simple_one() {
	r := gin.Default()
	r.GET("/", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": c.FullPath(),
		})
	})
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": "pong",
		})
	})
	r.Run() // listen and serve on 0.0.0.0:8080 (for windows "localhost:8080")
}

func config_http() {
	r := gin.Default()
	r.GET("/", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": c.FullPath(),
		})
	})
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": "pong",
		})
	})
	// r.Run() // listen and serve on 0.0.0.0:8080 (for windows "localhost:8080")
	s := &http.Server{
		Addr:           ":8080",
		Handler:        r,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	s.ListenAndServe()
}

func config_http2() {
	r := gin.Default()
	r.GET("/", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": c.FullPath(),
		})
	})
	r.GET("/ping", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": "pong",
		})
	})

	l, _ := net.Listen("tcp", ":8080")
	// r.Run() // listen and serve on 0.0.0.0:8080 (for windows "localhost:8080")
	s := &http.Server{
		Addr:           ":8080",
		Handler:        r,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   10 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	s.Serve(l)
}

type PingReq struct {
	Name string `json:"Name"`
	Val  string `json:"Val"`
}

type PingResp struct {
	Message string `json:"message"`
	Type    string `json:"type"`
	Marker  string `json:"marker"`
	Limits  string `json:"limits"`
	Name    string `json:"Name"`
}

func raw_gin() {
	r := gin.New()
	r.Use(gin.Logger())
	r.GET("/", func(c *gin.Context) {
		c.JSON(200, gin.H{
			"message": c.FullPath(),
		})
	})
	r.GET("/ping", func(c *gin.Context) {
		v := c.DefaultQuery("marker", "")
		l := c.DefaultQuery("limits", "20")
		headerType := c.GetHeader("User")
		time.Sleep(15 * time.Second)

		// parse body
		var req PingReq
		if err := c.BindJSON(&req); err != nil {
			fmt.Println(err)
			c.Status(http.StatusAccepted)
			return
		}

		body, _ := json.Marshal(req)
		c.Request.Body = ioutil.NopCloser(bytes.NewBuffer(body))
		body, _ = ioutil.ReadAll(c.Request.Body)
		fmt.Println("Reassigned body: ", string(body))

		c.JSON(200, PingResp{
			Message: "pong",
			Type:    headerType,
			Marker:  v,
			Limits:  l,
			Name:    req.Name,
		})
	})
	r.Handle(http.MethodGet, "/pong", func(c *gin.Context) {
		time.Sleep(15 * time.Second)
		c.JSON(200, "OK")
	})

	pingResp := PingResp{
		Message: "a test",
	}
	// response raw string
	r.GET("/string", func(c *gin.Context) {
		b, _ := json.Marshal(pingResp)
		fmt.Println("We encode pingResp to string: ", string(b))
		c.String(200, string(b))
	})
	// response struct
	r.GET("/jsonstruct", func(c *gin.Context) {
		c.JSON(200, pingResp)
	})

	l, _ := net.Listen("tcp", ":8000")
	// r.Run() // listen and serve on 0.0.0.0:8000 (for windows "localhost:8080")
	s := &http.Server{
		Handler:        r,
		ReadTimeout:    10 * time.Second,
		WriteTimeout:   20 * time.Second,
		MaxHeaderBytes: 1 << 20,
	}
	s.Serve(l)
}

func main() {
	// simple_one()
	// config_http()
	// config_http2()
	raw_gin()
}
