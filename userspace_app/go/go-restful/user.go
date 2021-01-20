// http://ernestmicklei.com/2012/11/go-restful-first-working-example/
package main

import (
	"github.com/emicklei/go-restful"
	"log"
	"net/http"
	"strings"
)

type User struct {
	Id, Name string
}

type Info struct {
	Version string
	Date    string
}

func New() *restful.WebService {
	service := new(restful.WebService)
	service.
		Path("/users").
		Consumes(restful.MIME_XML, restful.MIME_JSON).
		Produces(restful.MIME_JSON)
		// Produces(restful.MIME_XML, restful.MIME_JSON)

	service.Route(service.GET("/{user-id}").To(FindUser))
	service.Route(service.POST("").To(UpdateUser))
	service.Route(service.PUT("/{user-id}").To(CreateUser))
	service.Route(service.DELETE("/{user-id}").To(RemoveUser))

	service.Route(service.GET("/").To(func(req *restful.Request, resp *restful.Response) {
		resp.WriteAsJson(Info{Version: "0.1", Date: "2020.1.20"})
	}))

	restful.Filter(func(req *restful.Request, resp *restful.Response, chain *restful.FilterChain) {
		log.Println("processing url: ", req.Request.URL)
		if !strings.HasPrefix(req.Request.URL.String(), "/admin") {
			// request is permitted, so proceed with filter chain.
			chain.ProcessFilter(req, resp)
			return
		}
		resp.WriteErrorString(http.StatusUnauthorized, "Not Allowed")
	})

	return service
}

func FindUser(request *restful.Request, response *restful.Response) {
	id := request.PathParameter("user-id")
	log.Println("Find user: ", id)
	// here you would fetch user from some persistence system
	usr := User{Id: id, Name: "John Doe"}
	response.WriteEntity(usr)
}

func UpdateUser(request *restful.Request, response *restful.Response) {
	usr := new(User)
	err := request.ReadEntity(usr)
	// here you would update the user with some persistence system
	if err == nil {
		response.WriteEntity(usr)
	} else {
		response.WriteError(http.StatusInternalServerError, err)
	}
}

func CreateUser(request *restful.Request, response *restful.Response) {
	usr := User{Id: request.PathParameter("user-id")}
	err := request.ReadEntity(usr)
	// here you would create the user with some persistence system
	if err == nil {
		response.WriteEntity(usr)
	} else {
		response.WriteError(http.StatusInternalServerError, err)
	}
}

func RemoveUser(request *restful.Request, response *restful.Response) {
	// here you would delete the user from some persistence system
}

func main() {
	restful.Add(New())
	http.Handle("/swagger-ui/", http.StripPrefix("/swagger-ui/", http.FileServer(http.Dir("."))))
	log.Fatal(http.ListenAndServe(":8080", nil))
}
