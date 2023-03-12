
CC = gcc
CXX = g++

CFLAGS = -g -Wall 
CXXFLAGS = $(CFLAGS) -fpermissive -std=c++17
LDFLAGS = 
LDSUFFIX = -lm -ldl

OBJ_DIR = .
OUT_DIR = .
OUT_FILE = main

SRC_DIR = .

# Enumerating of every *.c *.cpp as *.o and using that as dependency.	
DIRS = $(shell find $(SRC_DIR) -maxdepth 10 -type d) 

C_SRC_ALL = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
CXX_SRC_ALL = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp)) 

FILTER_FILES = 
CXX_SRC = $(filter-out $(FILTER_FILES), $(CXX_SRC_ALL))

LIB_OBJS = $(CXX_SRC:.cpp=.o) $(C_SRC:.c=.o)

# targets
main: $(LIB_OBJS)
	$(CXX) $(LDFLAGS)  -o $(OUT_DIR)/$(OUT_FILE) $^  $(LDSUFFIX)

%.o:%.c
	$(CC) -c $(CFLAGS) $(INC) $< -o $@

%.o:%.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) $< -o $@ 

clean:
	rm -rf $(LIB_OBJS)
	rm -rf $(OUT_FILE)