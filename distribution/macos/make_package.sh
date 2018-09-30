#!/bin/bash

mkdir ./build/dist
cp ../../dependencies/include/lua/json.lua ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libmongocxx.3.1.3.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libmongocxx._noabi.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libbsoncxx.3.1.3.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libbsoncxx._noabi.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libmongoc-1.0.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libmongoc-1.0.0.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/opt/openssl/lib/libssl.1.0.0.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib ./build/fpgui.app/Contents/MacOS
cp /usr/local/lib/libbson-1.0.0.dylib ./build/fpgui.app/Contents/MacOS

mkdir ./build/fpgui.app/Contents/MacOS/QtWidgets.framework
mkdir ./build/fpgui.app/Contents/MacOS/QtWidgets.framework/Versions
mkdir ./build/fpgui.app/Contents/MacOS/QtWidgets.framework/Versions/5

mkdir ./build/fpgui.app/Contents/MacOS/QtGui.framework
mkdir ./build/fpgui.app/Contents/MacOS/QtGui.framework/Versions
mkdir ./build/fpgui.app/Contents/MacOS/QtGui.framework/Versions/5

mkdir ./build/fpgui.app/Contents/MacOS/QtNetwork.framework
mkdir ./build/fpgui.app/Contents/MacOS/QtNetwork.framework/Versions
mkdir ./build/fpgui.app/Contents/MacOS/QtNetwork.framework/Versions/5

mkdir ./build/fpgui.app/Contents/MacOS/QtPrintSupport.framework
mkdir ./build/fpgui.app/Contents/MacOS/QtPrintSupport.framework/Versions
mkdir ./build/fpgui.app/Contents/MacOS/QtPrintSupport.framework/Versions/5

mkdir ./build/fpgui.app/Contents/MacOS/QtCore.framework
mkdir ./build/fpgui.app/Contents/MacOS/QtCore.framework/Versions
mkdir ./build/fpgui.app/Contents/MacOS/QtCore.framework/Versions/5

mkdir ./build/fpgui.app/Contents/MacOS/platforms
mkdir ./build/fpgui.app/Contents/MacOS/styles

cp ~/Qt/5.10.1/clang_64/lib/QtWidgets.framework/Versions/5/QtWidgets ./build/fpgui.app/Contents/MacOS/QtWidgets.framework/Versions/5
cp ~/Qt/5.10.1/clang_64/lib/QtGui.framework/Versions/5/QtGui ./build/fpgui.app/Contents/MacOS/QtGui.framework/Versions/5
cp ~/Qt/5.10.1/clang_64/lib/QtCore.framework/Versions/5/QtCore ./build/fpgui.app/Contents/MacOS/QtCore.framework/Versions/5
cp ~/Qt/5.10.1/clang_64/lib/QtNetwork.framework/Versions/5/QtNetwork ./build/fpgui.app/Contents/MacOS/QtNetwork.framework/Versions/5
cp ~/Qt/5.10.1/clang_64/lib/QtPrintSupport.framework/Versions/5/QtPrintSupport ./build/fpgui.app/Contents/MacOS/QtPrintSupport.framework/Versions/5

cp ~/Qt/5.10.1/clang_64/plugins/platforms/libqcocoa.dylib ./build/fpgui.app/Contents/MacOS/platforms/libqcocoa.dylib
cp ~/Qt/5.10.1/clang_64/plugins/styles/libqmacstyle.dylib ./build/fpgui.app/Contents/MacOS/styles/libqmacstyle.dylib

install_name_tool -add_rpath ./ ./build/fpgui.app/Contents/MacOS/fpgui
install_name_tool -add_rpath /Applications/fpgui.app/Contents/MacOS ./build/fpgui.app/Contents/MacOS/fpgui

install_name_tool -add_rpath /Applications/fpgui.app/Contents/MacOS ./build/fpgui.app/Contents/MacOS/platforms/libqcocoa.dylib
install_name_tool -add_rpath /Applications/fpgui.app/Contents/MacOS ./build/fpgui.app/Contents/MacOS/styles/libqmacstyle.dylib

sudo install_name_tool -change /usr/local/lib/libmongoc-1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libmongoc-1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx.3.1.3.dylib
sudo install_name_tool -change /usr/local/opt/openssl/lib/libssl.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libssl.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx.3.1.3.dylib
sudo install_name_tool -change /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libcrypto.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx.3.1.3.dylib
sudo install_name_tool -change /usr/local/lib/libbson-1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libbson-1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libbsoncxx.3.1.3.dylib
sudo install_name_tool -change /usr/local/lib/libbson-1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libbson-1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongoc-1.0.0.dylib
sudo install_name_tool -change /usr/local/opt/openssl/lib/libssl.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libssl.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongoc-1.0.0.dylib
sudo install_name_tool -change /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libcrypto.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongoc-1.0.0.dylib
sudo install_name_tool -change /usr/local/Cellar/openssl/1.0.2o_2/lib/libcrypto.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libcrypto.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libssl.1.0.0.dylib
sudo install_name_tool -change /usr/local/lib/libmongoc-1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libmongoc-1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx._noabi.dylib
sudo install_name_tool -change /usr/local/opt/openssl/lib/libssl.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libssl.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx._noabi.dylib
sudo install_name_tool -change /usr/local/opt/openssl/lib/libcrypto.1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libcrypto.1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx._noabi.dylib
sudo install_name_tool -change /usr/local/lib/libbson-1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libbson-1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libbsoncxx._noabi.dylib
sudo install_name_tool -change /usr/local/lib/libbson-1.0.0.dylib /Applications/fpgui.app/Contents/MacOS/libbson-1.0.0.dylib ./build/fpgui.app/Contents/MacOS/libmongocxx._noabi.dylib


packagesbuild -v ./fpgui.pkgproj
productsign --sign "Developer ID Installer: Rostislav Kuratch" ./build/fpgui.pkg ./build/dist/fpgui.pkg
