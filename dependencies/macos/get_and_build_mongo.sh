#!/bin/bash

cmake --version 2> /dev/null
if [ $? -ne 0 ]; then
echo "************** ERROR **************"
echo "Please install CMake"
echo "***********************************"
exit
fi

curl --version > /dev/null
if [ $? -ne 0 ]; then
echo "************** ERROR **************"
echo "Please install curl"
echo "***********************************"
exit
fi

curl --cacert ./cacert.crt --output cacert.pem https://curl.haxx.se/ca/cacert.pem
if [ $? -eq 0 ]; then
rm cacert.crt
mv cacert.pem cacert.crt
fi

rm -rf ../build
mkdir ../build

curl -L --cacert ./cacert.crt -o ../build/mongo_c_driver.tar.gz https://github.com/mongodb/mongo-c-driver/releases/download/1.7.0/mongo-c-driver-1.7.0.tar.gz
curl -L --cacert ./cacert.crt -o ../build/mongo_cpp_driver.tar.gz https://github.com/mongodb/mongo-cxx-driver/archive/r3.1.3.tar.gz

export LDFLAGS="-L/usr/local/opt/openssl/lib"
export CPPFLAGS="-I/usr/local/opt/openssl/include"

cd ..
cd ./build

tar xzf mongo_c_driver.tar.gz
cd ./mongo-c-driver-1.7.0

./configure --enable-static

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build MongoDB C driver, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

make

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build MongoDB C driver, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

sudo ../../macos/make_install.sh

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build MongoDB C driver, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

cd ..

tar xzf mongo_cpp_driver.tar.gz
cd ./mongo-cxx-driver-r3.1.3/build

sudo cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_PREFIX_PATH=/usr/local ..

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build MongoDB C++ driver, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

make

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build MongoDB C++ driver, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

sudo ../../../macos/make_install.sh

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build MongoDB C++ driver, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

cd ..
cd ..
cd ..

rm -rf ./build

