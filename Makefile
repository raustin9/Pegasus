#Build the whole project
ENGINE=engine/
APPLICATION=testbed/
GLSLC=/usr/local/bin/glslc

# Compile the shaders
vertsources = $(shell find ./assets/shaders/vert -type f -name "*.vert")
vertobjfiles = $(patsubst %.vert, %.vert.spv, $(vertsources))
fragsources = $(shell find ./assets/shaders/frag -type f -name "*.frag")
fragobjfiles = $(patsubst %.frag, %.frag.spv, $(fragsources))


all: $(ENGINE)Makefile $(APPLICATION)Makefile $(vertobjfiles) $(fragobjfiles)
	@make -s -C engine
	@make -s -C testbed

run: all
	./bin/testbed

# Shader targets
%.spv: %
	$(GLSLC) $< -o $@

clean: 
	rm -f bin/* assets/shaders/*/*.spv
