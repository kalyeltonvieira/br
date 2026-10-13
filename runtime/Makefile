# Makefile para Runtime Nativo BR

CC = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = 

# Detect Windows
ifdef OS
   RM = del /Q
   FIXPATH = $(subst /,\,$1)
   MKDIR = mkdir
else
   RM = rm -f
   FIXPATH = $1
   MKDIR = mkdir -p
endif

# Diretórios
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INC_DIR = include
LIB_DIR = lib

# Arquivos fonte
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Nome do executável
TARGET = $(BIN_DIR)/br

# Regra principal
all: $(TARGET)

# Linkagem
$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) -lws2_32

# Compilação de objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Criar diretórios necessários
$(BIN_DIR):
	$(MKDIR) $(call FIXPATH,$(BIN_DIR))

$(OBJ_DIR):
	$(MKDIR) $(call FIXPATH,$(OBJ_DIR))

# Limpeza
clean:
	-$(RM) $(call FIXPATH,$(OBJ_DIR)\*.o) 2>nul
	-$(RM) $(call FIXPATH,$(BIN_DIR)\br.exe) 2>nul

# Testes
test: $(TARGET)
	@echo "Testando runtime BR..."
	@$(call FIXPATH,$(TARGET)) --version
	@echo "Teste básico concluído."

# Desenvolvimento (com debug)
debug: CXXFLAGS += -g -DDEBUG
debug: clean all

# Release (otimizado)
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

# Compilação rápida para desenvolvimento
dev: $(TARGET)
	@echo "Compilação rápida concluída."

# Ajuda
help:
	@echo "Makefile para Runtime Nativo BR"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  make all        Compila o runtime (padrão)"
	@echo "  make clean      Limpa arquivos compilados"
	@echo "  make test       Executa testes básicos"
	@echo "  make debug      Compila com símbolos de debug"
	@echo "  make release    Compila otimizado para release"
	@echo "  make dev        Compilação rápida para desenvolvimento"
	@echo "  make help       Exibe esta ajuda"

.PHONY: all clean test debug release dev help