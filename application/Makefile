CXX=clang++
CCFLAGS=-std=c++17 -g -fdeclspec -fPIC
INCLUDES=-Isrc -I../engine/src
LDFLAGS=-L../bin/ -lengine -Wl,-rpath,./bin/ -lX11
CCFILES=$(shell find . -type f -name "*.cc")
DEFINES=-DQIMPORT -D_DEBUG

all: ../bin/application

../bin/application: $(CCFILES)
	$(CXX) $(CCFILES) $(CCFLAGS) -o $@ $(DEFINES) $(INCLUDES) $(LDFLAGS)
