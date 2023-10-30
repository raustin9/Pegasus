CXX=clang++
CCFLAGS=-std=c++17 -g -shared -fdeclspec -fPIC -Wall -Wextra
INCLUDES=-Isrc -Iengine/src
LDFLAGS=-lvulkan -lX11 
CCFILES=$(shell find . -type f -name "*.cc")
DEFINES=-DQEXPORT -DDEBUG

engine: ../bin/libengine.so

../bin/libengine.so: $(CCFILES)
	$(CXX) $(INCLUDES) -o $@ $(CCFLAGS) $(CCFILES) $(LDFLAGS)
