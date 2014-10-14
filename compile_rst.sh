#!/bin/sh

tar zxfv cdf31-dist-cdf.tar.gz
cd cdf31-dist
make OS=linux ENV=gnu all
make INSTALLDIR=$PWD install
cd ..

chmod 755 superdarn-toolkit.linux64.2.10.sh
./superdarn-toolkit.linux64.2.10.sh

export HOMEPATH=$PWD
source profile.superdarn-toolkit.bash
export CDF_PATH=${HOMEPATH}/cdf31-dist

make.all

#./replace_rbpos.sh
