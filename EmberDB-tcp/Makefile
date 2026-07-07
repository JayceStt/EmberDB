CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

BUILD_DIR = build
TARGET = $(BUILD_DIR)/EmberDB-tcp

SRC = src/main.c src/hashmap.c src/server.c src/persistance.c src/commands.c src/general.c 
OBJ = $(SRC:src/%.c=$(BUILD_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) -c $< -o $@

run: all
	@printf '\033[2J\033[H\033[3J'
	@./$(TARGET)

clean:
	@rm -rf $(BUILD_DIR)