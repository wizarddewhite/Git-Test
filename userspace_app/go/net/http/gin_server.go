// https://github.com/gin-gonic/gin
package main

import (
	"github.com/gin-gonic/gin"
	"net"
	"net/http"
	"time"
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
		c.JSON(200, gin.H{
			"message": "pong",
			"type":    headerType,
			"marker":  v,
			"limits":  l,
		})
	})
	r.Handle(http.MethodGet, "/pong", func(c *gin.Context) {
		time.Sleep(15 * time.Second)
		c.JSON(200, "OK")
	})

	l, _ := net.Listen("tcp", ":8080")
	// r.Run() // listen and serve on 0.0.0.0:8080 (for windows "localhost:8080")
	s := &http.Server{
		Addr:           ":8080",
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
