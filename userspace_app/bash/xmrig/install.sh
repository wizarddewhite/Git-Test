sudo apt-get install git build-essential cmake libuv1-dev libmicrohttpd-dev
git clone https://github.com/xmrig/xmrig.git
cd xmrig
mkdir build
cd build
cmake ..
make

sysctl -w vm.nr_hugepages=32
