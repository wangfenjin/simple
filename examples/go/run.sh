#!/bin/sh

wget -qO- https://github.com/wangfenjin/simple/releases/download/v0.1.0/libsimple-osx-x64.zip | tar xf -
go build --tags fts5 -o gosimple
./gosimple
