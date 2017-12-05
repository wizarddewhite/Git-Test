#!/bin/bash

sudo apt-get install build-essential libcurl4-openssl-dev git automake libtool libjansson* libncurses5-dev libssl-dev
git clone --recursive https://github.com/tpruvot/cpuminer-multi.git
cd cpuminer-multi
git checkout linux
./autogen.sh
./configure CFLAGS="-O3 -march=native" --with-crypto --with-curl
make
