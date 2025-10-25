CC = gcc
CFLAGS = -O2 -Wall -Wextra -Iinclude
SRC_DIR = src
BUILD = build
BIN = bin

SOURCES := $(shell find $(SRC_DIR) -type f -name '*.c')
APP_SOURCES := $(filter-out $(SRC_DIR)/compiler/%,$(SOURCES))
COMPILER_SOURCES := $(filter $(SRC_DIR)/compiler/%,$(SOURCES))

APP_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(APP_SOURCES))
COMPILER_OBJS := $(patsubst %.c,$(BUILD)/%.o,$(COMPILER_SOURCES))

TARGET_APP = $(BIN)/rmvm
TARGET_COMPILER = $(BIN)/rmc

.PHONY: all clean
all: $(TARGET_APP) $(TARGET_COMPILER)

$(TARGET_APP): $(APP_OBJS)
	@mkdir -p $(BIN)
	$(CC) $(APP_OBJS) -o $@
	@rm -f $(APP_OBJS) $(APP_OBJS:.o=.d)
	@find $(BUILD) -type d -empty -delete

$(TARGET_COMPILER): $(COMPILER_OBJS)
	@mkdir -p $(BIN)
	$(CC) $(COMPILER_OBJS) -o $@
	@rm -f $(COMPILER_OBJS) $(COMPILER_OBJS:.o=.d)
	@find $(BUILD) -type d -empty -delete

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(APP_OBJS:.o=.d) $(COMPILER_OBJS:.o=.d)

clean:
	rm -rf $(BUILD) $(BIN)
