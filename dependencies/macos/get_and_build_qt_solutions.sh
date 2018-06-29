#!/bin/bash

export PATH="/usr/local/opt/qt/bin:$PATH"

curl --cacert ./cacert.crt --output cacert.pem https://curl.haxx.se/ca/cacert.pem
if [ $? -eq 0 ]; then
rm cacert.crt
mv cacert.pem cacert.crt
fi

rm -rf ../build
mkdir ../build

curl -L --cacert ./cacert.crt -o ../build/qt-solutions.zip https://github.com/liangqi/qt-solutions/archive/master.zip
7za x -y -o"../build/" ../build/qt-solutions.zip
mv ../build/qt-solutions-master/qtcolorpicker ../qtcolorpicker
cd ../qtcolorpicker
cd ./plugin
qmake
make


