BUILD_DIR := bin
OBJ_DIR := obj

ASSEMBLY := engine
EXTENSION := .so
COMPILER_FLAGS := -g -fdeclspec -Werror=vla  -fPIC -std=c++17
INCLUDE_FLAGS := -Iengine/src -I$(VULKAN_SDK)/include
LINKER_FLAGS := -g -shared -lvulkan  -lX11  -L$(VULKAN_SDK)/lib 
DEFINES := -D_QDEBUG -DQEXPORT


SRC_FILES := $(shell find $(ASSEMBLY) -name *.cc)		# .cc files
DIRECTORIES := $(shell find $(ASSEMBLY) -type d)		# directories with .h files
OBJ_FILES := $(SRC_FILES:%=$(OBJ_DIR)/%.o)		# compiled .o objects

all: scaffold compile link

.PHONY: scaffold
scaffold: # create build directory
	@echo Scaffolding folder structure...
	@mkdir -p $(addprefix $(OBJ_DIR)/,$(DIRECTORIES))
	@echo Done.

.PHONY: link
link: scaffold $(OBJ_FILES) # link
	@echo Linking $(ASSEMBLY)...
	@clang++ $(OBJ_FILES) -o $(BUILD_DIR)/lib$(ASSEMBLY)$(EXTENSION) $(LINKER_FLAGS)

.PHONY: compile
compile: #compile .c files
	@echo Compiling...

.PHONY: clean
clean: # clean build directory
	rm -rf $(BUILD_DIR)/$(ASSEMBLY)
	rm -rf $(OBJ_DIR)/$(ASSEMBLY)

$(OBJ_DIR)/%.cc.o: %.cc # compile .c to .o object
	@echo   $<...
	@clang++ $< $(COMPILER_FLAGS) -c -o $@ $(DEFINES) $(INCLUDE_FLAGS)
