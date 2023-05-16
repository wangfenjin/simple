#!/bin/sh

wget https://github.com/wangfenjin/simple/releases/download/v0.2.2/libsimple-linux-ubuntu-latest.zip && unzip libsimple-linux-ubuntu-latest.zip && rm -f libsimple-linux-ubuntu-latest.zip
go run --tags fts5 .
