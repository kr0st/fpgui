#!/bin/bash

echo 'building for x86_64, release configuration'

rm -rf ./build/*
rmdir ./build
mkdir ./build

cd ..
cd ..

cd ./dependencies
cd ./macos

./get_and_build_mongo.sh
./get_and_build_other.sh

cd ..
cd ..

cd ./distribution
cd ./macos
cd ./build

PATH=$PATH:~/Qt/5.10.1/clang_64/bin
qmake ../../../fpgui.pro -r CONFIG+=release
make
