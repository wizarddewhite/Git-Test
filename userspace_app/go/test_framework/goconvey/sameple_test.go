package main

import (
	"net/http"
	"net/http/httptest"
	"testing"

	"github.com/smartystreets/goconvey/convey"
)

func TestCheckUrl(t *testing.T) {
	convey.Convey("TestCheckUrl", t, func() {
		ok := CheckUrl("learnku.com")
		convey.So(ok, convey.ShouldBeTrue)
	})

	convey.Convey("TestCheckTeachUrl", t, func() {
		convey.Convey("TestCheckTeachUrl true", func() {
			ok := CheckUrl("learnku.com")
			convey.So(ok, convey.ShouldBeTrue)
		})
		convey.Convey("TestCheckTeachUrl false", func() {
			ok := CheckUrl("xxxxxx.com")
			convey.So(ok, convey.ShouldBeFalse)
		})
		convey.Convey("TestCheckTeachUrl null", func() {
			ok := CheckUrl("")
			convey.So(ok, convey.ShouldBeFalse)
		})
	})
}

func TestHttpGetWithTimeOut(t *testing.T) {

	convey.Convey("TestHttpGetWithTimeOut", t, func() {
		convey.Convey("TestHttpGetWithTimeOut normal", func() {
			ts := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
				w.WriteHeader(http.StatusOK)
				w.Write([]byte("TestHttpGetWithTimeOut success!!"))
				if r.Method != "GET" {
					t.Errorf("Except 'Get' got '%s'", r.Method)
				}
				if r.URL.EscapedPath() != "/要访问的url" {
					t.Errorf("Expected request to '/要访问的url', got '%s'", r.URL.EscapedPath())
				}
			}))
			api := ts.URL
			t.Logf("api %s\n", api)

			defer ts.Close()
			var header = make(map[string]string)
			ok := HttpGetWithTimeOut(api, header, 30)
			convey.So(ok, convey.ShouldBeFalse)
		})
	})
}
