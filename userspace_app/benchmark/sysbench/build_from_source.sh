# Get source
git clone https://github.com/akopytov/sysbench.git

# Prepare
## Ubuntu
```
apt -y install make automake libtool pkg-config libaio-dev
# For MySQL support
apt -y install libmysqlclient-dev libssl-dev
# For PostgreSQL support
apt -y install libpq-dev
```

## RHEL
```
yum -y install make automake libtool pkgconfig libaio-devel
# For MySQL support, replace with mysql-devel on RHEL/CentOS 5
yum -y install mariadb-devel openssl-devel
# For PostgreSQL support
yum -y install postgresql-devel
```

# Build
```
./autogen.sh
# Add --with-pgsql to build with PostgreSQL support
./configure
make -j
make install
```
