package main

import (
	"github.com/smartystreets/goconvey/convey"
	"testing"
)

func TestCheckUrl(t *testing.T) {
	convey.Convey("TestCheckTeachUrl", t, func() {
		ok := CheckUrl("learnku.com")
		convey.So(ok, convey.ShouldBeTrue)
	})
}
