ROOT_DIR =${CURDIR}
LIBS= $(wildcard libs/*)
LIBS_PATH= $(ROOT_DIR)/libs
CC= g++ 
OPTIONS= -std=c++11 -w
FILES=$(wildcard *.cpp) $(wildcard *.h)
DEP= -lcplex -lm -lpthread
BIN_LIST = EqualFix
                     
all:
	@mkdir -p output
	@$(CC) $(OPTIONS) -I $(ROOT_DIR) -I $(LIBS_PATH) -D IL_STD $(FILES) -o EqualFix -L $(LIBS_PATH) $(DEP)
clean:
	@rm -rf $(BIN_LIST)















	
