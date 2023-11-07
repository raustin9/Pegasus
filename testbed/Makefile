CXX=clang++
CCFLAGS=-std=c++17 -g -fdeclspec -fPIC
INCLUDES=-Isrc -I../engine/src
LDFLAGS=-L../bin/ -lengine -Wl,-rpath,./bin/ #-lX11 -lvulkan
CCFILES=$(shell find . -type f -name "*.cc")
DEFINES=-DQIMPORT -D_DEBUG

all: ../bin/testbed
	echo "testbed built successfully"

../bin/testbed: $(CCFILES)
	$(CXX) $(CCFILES) $(CCFLAGS) -o $@ $(DEFINES) $(INCLUDES) $(LDFLAGS)
