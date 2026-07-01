# Disciplina: Software Basico

# O arquivo alvo padrao caso o usuario rode o comando sem passar o parametro FILE
FILE ?= Main.class

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
CFLAGS   := -std=c99 -Wall -Wextra -g -m32

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
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(call MKDIR,$(OBJ_DIR))
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@if exist $(OBJ_DIR) $(RM) $(OBJ_DIR)
	@if exist $(BIN_DIR) $(RM) $(BIN_DIR)

# Inicia o visualizador estatico do .class
run-leitor: all
	.$(call FIX_PATH,/$(TARGET)) -l $(FILE)

# Inicia a maquina virtual para executar o bytecode
run-interpreter: all
	.$(call FIX_PATH,/$(TARGET)) -i $(FILE)

.PHONY: all clean run run-leitor run-interpreter
# make            : Apenas compila o projeto gerando o binário na pasta bin/.
# make run-leitor : Compila o projeto e executa a ferramenta de análise estática (Leitor/Exibidor).
# make run-interpreter : Compila o projeto e executa a máquina virtual (Interpretador/Motor de Execução).
# make clean      : Apaga completamente as pastas de lixo de compilação (obj/ e bin/).