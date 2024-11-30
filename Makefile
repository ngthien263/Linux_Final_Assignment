.PHONY := all clean
PRJ_NAME := FINAL
CUR_PATH := .
BIN_DIR := $(CUR_PATH)/bin
INC_DIR := $(CUR_PATH)/include
SRC_DIR := $(CUR_PATH)/source
OBJ_DIR := $(CUR_PATH)/obj
LIB_DIR := $(CUR_PATH)/lib
CFLAGS = -I$(INC_DIR) -Wall


CC = gcc
create_obj:
	$(CC) $(CFLAGS) -c -fPIC $(SRC_DIR)/database.c -o $(OBJ_DIR)/database.o
	$(CC) $(CFLAGS) -c -fPIC $(SRC_DIR)/linkedList.c -o $(OBJ_DIR)/linkedList.o
	$(CC) $(CFLAGS) -c -fPIC $(SRC_DIR)/sensor.c -o $(OBJ_DIR)/sensor.o
	$(CC) $(CFLAGS) -c -fPIC $(SRC_DIR)/socket.c -o $(OBJ_DIR)/socket.o
	$(CC) $(CFLAGS) -c -fPIC $(SRC_DIR)/log.c -o $(OBJ_DIR)/log.o
	$(CC) $(CFLAGS) -c -fPIC $(CUR_PATH)/sensor_gateway.c -o $(CUR_PATH)/sensor_gateway.o 
	$(CC) $(CFLAGS) -c -fPIC $(CUR_PATH)/sensor_node.c -o $(CUR_PATH)/sensor_node.o 

create_shared_lib:
	$(CC) -shared $(OBJ_DIR)/database.o $(OBJ_DIR)/linkedList.o $(OBJ_DIR)/sensor.o $(OBJ_DIR)/socket.o $(OBJ_DIR)/log.o -o $(LIB_DIR)/lib$(PRJ_NAME).so
install_lib:
	sudo cp -f $(LIB_DIR)/lib$(PRJ_NAME).so /usr/lib

all: create_obj create_shared_lib install_lib
	gcc  $(CUR_PATH)/sensor_gateway.o -L$(LIB_DIR) -l$(PRJ_NAME) -o $(BIN_DIR)/sensor_gateway -lpthread -lsqlite3
	gcc  $(CUR_PATH)/sensor_node.o -L$(LIB_DIR) -l$(PRJ_NAME) -o $(BIN_DIR)/sensor_node -lpthread -lsqlite3

clean:
	rm -rf $(BIN_DIR)/*
	rm -rf $(CUR_PATH)/*.o
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(LIB_DIR)/*
