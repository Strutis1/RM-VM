# Compiler & flags
CC      := gcc
CFLAGS  := -O2 -Wextra -Iinclude

# Layout
SRC_DIR := src
BIN     := bin

# Windows exe suffix if OS var is set (MSYS keeps this)
ifeq ($(OS),Windows_NT)
  EXE_EXT := .exe
else
  EXE_EXT :=
endif

# Source lists (no recursion; list subdirs explicitly)
APP_SOURCES := \
  $(wildcard $(SRC_DIR)/*.c) \
  $(wildcard $(SRC_DIR)/rm/*.c) \
  $(wildcard $(SRC_DIR)/utils/*.c) \
  $(wildcard $(SRC_DIR)/vm/*.c)

COMPILER_SOURCES := \
  $(wildcard $(SRC_DIR)/compiler/*.c)

# Targets
TARGET_APP      := $(BIN)/rmvm$(EXE_EXT)
TARGET_COMPILER := $(BIN)/rmc$(EXE_EXT)

.PHONY: all clean
all: $(TARGET_APP) $(TARGET_COMPILER)

$(TARGET_APP): $(APP_SOURCES)
	@powershell -NoProfile -Command "New-Item -ItemType Directory -Force '$(BIN)'" >NUL
	$(CC) $(CFLAGS) $(APP_SOURCES) -o $@

$(TARGET_COMPILER): $(COMPILER_SOURCES)
	@powershell -NoProfile -Command "New-Item -ItemType Directory -Force '$(BIN)'" >NUL
	$(CC) $(CFLAGS) $(COMPILER_SOURCES) -o $@

clean:
	@powershell -NoProfile -Command "If (Test-Path '$(BIN)') { Remove-Item -Recurse -Force '$(BIN)' }"
