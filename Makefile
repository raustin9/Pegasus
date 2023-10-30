#Build the whole project
ENGINE=engine/
APPLICATION=application/

all: $(ENGINE)Makefile $(APPLICATION)Makefile
	make -C engine
	make -C application

run: all
	./bin/application

clean: 
	rm -f bin/*
