package main

import (
	"github.com/smartystreets/goconvey/convey"
	"testing"
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
