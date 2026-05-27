# Makefile - Leitor/Exibidor JVM
# Disciplina: Software Basico

ifeq (run,$(firstword $(MAKECMDGOALS)))
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  $(eval $(RUN_ARGS):;@:)
endif

ifeq ($(OS),Windows_NT)
    RM = rmdir /s /q
    MKDIR = if not exist $1 mkdir $1
    TARGET_EXT := .exe
    FIX_PATH = $(subst /,\,$1)
else
    RM = rm -rf
    MKDIR = mkdir -p $1
    TARGET_EXT :=
    FIX_PATH = $1
endif

CC       := gcc
CFLAGS   := -std=c99 -Wall -Wextra -g

SRC_DIR  := src
INC_DIR  := include
OBJ_DIR  := obj
BIN_DIR  := bin

TARGET   := $(BIN_DIR)/leitor_exibidor$(TARGET_EXT)

SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

INCLUDES := -I$(INC_DIR)

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(call MKDIR,$(BIN_DIR))
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(call MKDIR,$(OBJ_DIR))
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@if exist $(OBJ_DIR) $(RM) $(OBJ_DIR)
	@if exist $(BIN_DIR) $(RM) $(BIN_DIR)

run: all
	.$(call FIX_PATH,/$(TARGET)) $(RUN_ARGS)

.PHONY: all clean run
# make: Apenas compila o projeto. 

# make run: Compila todo o projeto e já inicia a execução do programa imediatamente.

# make clean: Apaga completamente as pastas de lixo de compilação (obj/ e bin/). 