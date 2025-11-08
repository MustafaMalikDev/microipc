CC := gcc

CFLAGS := -Wall -Wextra -Iinclude -std=c99 -Wno-missing-braces

SRC_DIR := ./src
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
BIN := $(BUILD_DIR)/demo-server

MAIN_SRC := $(SRC_DIR)/demo.c
SRCS := $(filter-out $(MAIN_SRC), $(shell find $(SRC_DIR) -name '*.c'))
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN_OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(MAIN_SRC))

all: $(BIN)

$(BIN): $(OBJS) $(MAIN_OBJ)
	@mkdir -p $(BUILD_DIR)	
	$(CC) $(CFLAGS) -o $@ $^

$(MAIN_OBJ): $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

fmt:
	ligen2 --license ./ligen.txt --allow include --width 1
	ligen2 --license ./ligen.txt --allow src --width 1
	sh ./format.sh

run:
	./$(BIN)

clean:
	rm -rf $(BUILD_DIR)

install:
	sudo cp ./$(BIN) /usr/local/bin

.PHONY: all clean fmt run install