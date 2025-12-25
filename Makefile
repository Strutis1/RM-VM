# Compiler & flags
CC      := gcc
CFLAGS  := -O2 -Wall -Wextra -Iinclude

# Layout
SRC_DIR := src
BUILD   := build
BIN     := bin

# Executable suffix
ifeq ($(OS),Windows_NT)
  EXE_EXT := .exe
else
  EXE_EXT :=
endif

# ---------- Source discovery (no external tools) ----------
# Add or remove subfolders here if your tree changes.
APP_SOURCES := \
  $(wildcard $(SRC_DIR)/*.c) \
  $(wildcard $(SRC_DIR)/rm/*.c) \
  $(wildcard $(SRC_DIR)/utils/*.c) \
  $(wildcard $(SRC_DIR)/vm/*.c)


# Object files mirror the source tree under $(BUILD)/
APP_OBJS       := $(patsubst %.c,$(BUILD)/%.o,$(APP_SOURCES))

# Targets
TARGET_APP      := $(BIN)/rmvm$(EXE_EXT)


# ---------- OS-specific shims ----------
ifeq ($(OS),Windows_NT)
  MKDIR = powershell -NoProfile -Command "New-Item -ItemType Directory -Force" 
  RMF   = powershell -NoProfile -Command "param([string[]]$$p) if($$p.Length){ Remove-Item -Force -ErrorAction SilentlyContinue $$p }" --
  RMRF  = powershell -NoProfile -Command "param([string[]]$$p) if($$p.Length){ Remove-Item -Recurse -Force -ErrorAction SilentlyContinue $$p }" --
else
  MKDIR = mkdir -p
  RMF   = rm -f
  RMRF  = rm -rf
endif

.PHONY: all clean

# Build both if the compiler sources exist; otherwise only the app.
ifneq ($(strip $(COMPILER_SOURCES)),)
  all: $(TARGET_APP) $(TARGET_COMPILER)
else
  all: $(TARGET_APP)
endif

# Link app
$(TARGET_APP): $(APP_OBJS)
	@$(MKDIR) "$(BIN)"
	$(CC) $(APP_OBJS) -o $@

# Link compiler (only if we discovered sources)
ifneq ($(strip $(COMPILER_SOURCES)),)
$(TARGET_COMPILER): $(COMPILER_OBJS)
	@$(MKDIR) "$(BIN)"
	$(CC) $(COMPILER_OBJS) -o $@
endif

# Compile .c -> build/.../.o (mirrors tree)
$(BUILD)/%.o: %.c
	@$(MKDIR) "$(dir $@)"
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Auto-include dependency files (only those that exist)
-include $(APP_OBJS:.o=.d) $(COMPILER_OBJS:.o=.d)

clean:
	@$(RMRF) "$(BUILD)" "$(BIN)"
