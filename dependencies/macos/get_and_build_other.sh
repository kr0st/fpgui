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

7za > /dev/null
if [ $? -ne 0 ]; then
echo "********************* ERROR *********************"
echo "Please install 7zip (p7zip-full package)"
echo "*************************************************"
exit
fi

curl --cacert ./cacert.crt --output cacert.pem https://curl.haxx.se/ca/cacert.pem
if [ $? -eq 0 ]; then
rm cacert.crt
mv cacert.pem cacert.crt
fi

rm -rf ../build
mkdir ../build

curl -L --cacert ./cacert.crt -o ../build/date.zip https://github.com/HowardHinnant/date/archive/v2.2.zip
curl -L --cacert ./cacert.crt -o ../build/gtest.zip https://codeload.github.com/google/googletest/zip/release-1.8.0
curl -L --cacert ./cacert.crt -o ../build/chai.zip https://github.com/ChaiScript/ChaiScript/archive/v6.0.0.zip
curl -L --cacert ./cacert.crt -o ../build/jsonlua.zip https://gist.github.com/tylerneylon/59f4bcf316be525b30ab/archive/7f69cc2cea38bf68298ed3dbfc39d197d53c80de.zip
curl -R -o ../build/lua-5.3.4.tar.gz http://www.lua.org/ftp/lua-5.3.4.tar.gz

7za x -y -o"../build/" ../build/date.zip
7za x -y -o"../build/" ../build/gtest.zip
7za x -y -o"../build/" ../build/chai.zip
7za x -y -o"../build/" ../build/jsonlua.zip

export LDFLAGS="-L/usr/local/opt/openssl/lib"
export CPPFLAGS="-I/usr/local/opt/openssl/include"

cd ..
cd ./build

tar zxf lua-5.3.4.tar.gz
cd ./lua-5.3.4
make macosx test MYCFLAGS="-arch x86_64"

echo 'liblua5.3.4.dylib: $(CORE_O) $(LIB_O)' >> src/makefile
echo -e '\t$(CC) -dynamiclib -o $@ $^ $(LIBS) -arch x86_64 -compatibility_version 5.3.0 -current_version 5.3.4 -install_name @rpath/$@' >> src/makefile
make -C src liblua5.3.4.dylib

make macosx local MYCFLAGS="-arch x86_64"

mkdir ../../include

mkdir ../../lib
mkdir ../../lib/x64

rm -rf ../../include/lua
rm -rf ../../lib/x64/*lua*

mkdir ../../include/lua

cp -rf ./install/lib/liblua.a ../../lib/x64
cp -rf ./src/liblua5.3.4.dylib ../../lib/x64
cp -rf ./install/include/* ../../include/lua

cd ..	

cp -rf ./59f4bcf316be525b30ab-7f69cc2cea38bf68298ed3dbfc39d197d53c80de/json.lua ../include/lua

cd googletest-release-1.8.0
cd googletest

rm -rf ../../../include/gtest
rm -rf ../../../lib/x64/gtest

cmake ./
make

if [ $? -ne 0 ]; then
echo "****************************************** ERROR ******************************************"
echo "unable to build gtest libraries, please inspect console output for clues."
echo "*******************************************************************************************"
exit
fi

cp -rf ./libgtest.a ../../../lib/x64
cp -rf ./libgtest_main.a ../../../lib/x64
cp -rf ./include/gtest ../../../include/

cd ..
cd ..

rm -rf ../include/date
mkdir ../include/date

cp -rf ./date-2.2/* ../include/date/

rm -rf ../include/chaiscript
mkdir ../include/chaiscript

cp -rf ./ChaiScript-6.0.0/include/chaiscript/* ../include/chaiscript/

cd ..
rm -rf ./build

