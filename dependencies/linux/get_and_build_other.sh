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

curl -LR --cacert ./cacert.crt -o ../build/rapidjson.zip https://github.com/Tencent/rapidjson/archive/v1.1.0.zip
curl -L --cacert ./cacert.crt -o ../build/date.zip https://github.com/HowardHinnant/date/archive/v2.2.zip
curl -L --cacert ./cacert.crt -o ../build/gtest.zip https://codeload.github.com/google/googletest/zip/release-1.8.0
curl -L --cacert ./cacert.crt -o ../build/chai.zip https://github.com/ChaiScript/ChaiScript/archive/v6.0.0.zip
curl -L --cacert ./cacert.crt -o ../build/jsonlua.zip https://gist.github.com/tylerneylon/59f4bcf316be525b30ab/archive/7f69cc2cea38bf68298ed3dbfc39d197d53c80de.zip
curl -R -o ../build/lua-5.3.4.tar.gz http://www.lua.org/ftp/lua-5.3.4.tar.gz
curl -R -o ../build/LuaJIT-2.0.5.zip http://luajit.org/download/LuaJIT-2.0.5.zip

7za x -y -o"../build/" ../build/rapidjson.zip
7za x -y -o"../build/" ../build/date.zip
7za x -y -o"../build/" ../build/gtest.zip
7za x -y -o"../build/" ../build/chai.zip
7za x -y -o"../build/" ../build/jsonlua.zip
7za x -y -o"../build/" ../build/LuaJIT-2.0.5.zip

#export LDFLAGS="-L/usr/local/opt/openssl/lib"
#export CPPFLAGS="-I/usr/local/opt/openssl/include"

cd ..
cd ./build

tar zxf lua-5.3.4.tar.gz
cd ./lua-5.3.4
make linux test

make -C src liblua5.3.4

make linux local

mkdir ../../include

mkdir ../../lib
mkdir ../../lib/x64

rm -rf ../../include/lua
rm -rf ../../include/rapidjson
rm -rf ../../include/luajit-2.0
rm -rf ../../lib/x64/*lua*

mkdir ../../include/lua
mkdir ../../include/luajit-2.0

cp -rf ./install/lib/liblua.a ../../lib/x64
cp -rf ./install/include/* ../../include/lua

cd ..	

cp -rf ./59f4bcf316be525b30ab-7f69cc2cea38bf68298ed3dbfc39d197d53c80de/json.lua ../include/lua

cd LuaJIT-2.0.5
make all
cp -rf ./src/libluajit.a ../../lib/x64
cp -rf ./src/lua.h ../../include/luajit-2.0
cp -rf ./src/lua.hpp ../../include/luajit-2.0
cp -rf ./src/luaconf.h ../../include/luajit-2.0
cp -rf ./src/luajit.h ../../include/luajit-2.0
cp -rf ./src/lualib.h ../../include/luajit-2.0
cp -rf ./src/lauxlib.h ../../include/luajit-2.0
cd ..

cd rapidjson-1.1.0
cp -rf ./include/* ../../include/
cd ..

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

