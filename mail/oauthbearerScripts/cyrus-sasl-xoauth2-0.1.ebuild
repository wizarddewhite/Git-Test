# Copyright 1999-2017 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

inherit autotools eutils multilib multilib-minimal

DESCRIPTION="xoauth2 plugin for cyrus-sasl"
HOMEPAGE="https://github.com/moriyoshi/cyrus-sasl-xoauth2"
SRC_URI="https://github.com/moriyoshi/${PN}/archive/v${PV}.tar.gz -> ${P}.tar.gz"

# License wraps lines differently and doesn't have last paragraph
# in square brackets about advertising.  But otherwise the license
# wording is identical to MIT:
LICENSE="MIT"

SLOT="0"
KEYWORDS="amd64 ~x86"

RDEPEND="dev-libs/cyrus-sasl"
DEPEND="${RDEPEND}"

src_prepare() {
	# FUTURE: It would probably be better to make this a properly-named
	#  "configure"-able variable, and invoke configure with something like
	#     --with-plugindir="${EPREFIX}/usr/$(get_libdir)/sasl2"
	sed -i -e 's%^pkglibdir =.*/lib/%pkglibdir = @libdir@/%' Makefile.am

	default

	eautoreconf
}

multilib_src_configure() {
	ECONF_SOURCE=${S} \
	econf \
		--with-cyrus-sasl="${EPREFIX}/usr" \
		--disable-static
}

multilib_src_install() {
	default

	# Adapted from similar in cyrus-sasl ebuild; removed static-libs check
	# FUTURE: Would it be useful to add a static-libs use
	#   flag and prefix this with "use static-libs ||"?  It isn't
	#   clear that that would be useful for anything; things using
	#   sasl would probably have to explicitly statically link
	#   this specific plugin.
	[[ $(get_modname) != .so ]] || \
		prune_libtool_files --modules
}
