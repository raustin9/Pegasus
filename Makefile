#Build the whole project
ENGINE=engine/
APPLICATION=application/

all: $(ENGINE)Makefile $(APPLICATION)Makefile
	@make -s -C engine
	@make -s -C application

run: all
	./bin/application

clean: 
	rm -f bin/*
