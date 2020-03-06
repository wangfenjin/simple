# http://www.sqlite.org/download.html

SQLITE_VERSION  ?= 3310100
SQLITE_YEAR     ?= 2020

SQLITE_BASENAME := sqlite-amalgamation-$(SQLITE_VERSION)
SQLITE_URL      := http://www.sqlite.org/$(SQLITE_YEAR)/$(SQLITE_BASENAME).zip

CC = gcc
CPP = g++

LIB = -ldl -lpthread
SHELL_FLAGS = \
-DSQLITE_DEBUG \
-DSQLITE_ENABLE_FTS5 \
-DSQLITE_ENABLE_EXPLAIN_COMMENTS \
-DSQLITE_ENABLE_SELECTTRACE \
-DSQLITE_ENABLE_WHERETRACE


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	LIBNAME := libsimple.dylib
	SHAREDNAME := sqlite3.dylib
else
	LIBNAME := libsimple.so
	SHAREDNAME := sqlite3.so
endif

clean-lib:
	rm -f libsimple.so

${LIBNAME}: ${SHAREDNAME} clean-lib
	${CPP} src/*.h src/*.cc src/*.c -shared -rdynamic -o ${LIBNAME} -I build/ -fPIC ${LIB} ${SHELL_FLAGS}

${SHAREDNAME}: build/sqlite3.c
	${CC} $^ -c -o $@ -fPIC

# Unpack
build/sqlite3.c: $(SQLITE_BASENAME).zip
	unzip -oq "$<"
	rm -rf build
	mv "$(SQLITE_BASENAME)" build
	touch "$@"

# Download
$(SQLITE_BASENAME).zip:
	wget -N -c "$(SQLITE_URL)"

sqlite3-shell: build/sqlite3.c
	${CC} -lm build/sqlite3.c build/shell.c ${LIB} -o $@ ${SHELL_FLAGS}

clean:
	rm -f "$(SQLITE_BASENAME).zip"
	rm -rf "$(SQLITE_BASENAME)"
	rm -rf build
	rm -rf obj
	rm -rf libs
	rm -f ${LIBNAME}
	rm -f ${SHAREDNAME}
	rm -f sqlite3-shell

test:
	./sqlite3-shell < test.sql

main:
	g++ ./src/pinyin.h ./src/simple_tokenizer.h ./src/pinyin.cc ./src/simple_tokenizer.cc main.cc -I build/ -I src/

format:
	clang-format-7 -i *.cc src/*.h src/*.cc src/*.c
	cpplint --extensions=h,cc --linelength=120 --filter=-build/include,-legal/copyright,-readability/streams,-whitespace/comma --root=src src/*

