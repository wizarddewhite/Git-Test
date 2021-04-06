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

type Resource struct {
	Name  string
	Group string
}

type Info struct {
	Version   string
	Date      string
	Resources []Resource
}

func New() *restful.WebService {
	service := new(restful.WebService)
	service.Doc("YW test webservice")
	service.
		Path("/users").
		Consumes(restful.MIME_XML, restful.MIME_JSON).
		Produces(restful.MIME_JSON)
		// Produces(restful.MIME_XML, restful.MIME_JSON)

	service.Route(service.GET("/{user-id}").To(FindUser).
		Param(service.QueryParameter("pretty", "If 'true', then the output is pretty printed.")).
		Param(service.QueryParameter("detail", "If 'true', then the output is detail.")))
	service.Route(service.POST("").To(UpdateUser))
	service.Route(service.PUT("/{user-id}").To(CreateUser))
	service.Route(service.DELETE("/{user-id}").To(RemoveUser))

	service.Route(service.GET("/").Writes(Info{}).To(func(req *restful.Request, resp *restful.Response) {
		info := Info{Version: "0.1", Date: "2020.1.20"}
		info.Resources = []Resource{
			{
				Name:  "add",
				Group: "admin",
			},
		}
		resp.WriteAsJson(info)
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
	pretty := request.QueryParameter("pretty")
	detail := request.QueryParameter("detail")
	// here you would fetch user from some persistence system
	if pretty == "true" {
		log.Println("pretty")
	}
	if detail == "true" {
		log.Println("detail")
	}
	usr := User{Id: id, Name: "John Doe"}
	response.WriteEntity(usr)
}

func UpdateUser(request *restful.Request, response *restful.Response) {
	log.Println("update User: ")
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
	log.Println("create User: ", request.PathParameter("user-id"))
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
