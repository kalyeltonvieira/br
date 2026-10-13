# Makefile para Linguagem BR v3.0.0 (Performance)
# Uso: make build ou make run ARQUIVO=exemplo.br

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
SRCDIR = runtime/src
INCDIR = runtime/include
OUTDIR = bin

# Fase 1 - Fundamentos
SOURCES_FASE1 = $(SRCDIR)/jit_compiler.cpp $(SRCDIR)/garbage_collector.cpp $(SRCDIR)/memory_pool.cpp $(SRCDIR)/string_pool.cpp

# Fase 2 - Otimizações
SOURCES_FASE2 = $(SRCDIR)/jit_optimizing.cpp $(SRCDIR)/inline_cache.cpp $(SRCDIR)/type_specialization.cpp $(SRCDIR)/compilation_cache.cpp

# Serviços existentes
SOURCES_SERVICOS = $(SRCDIR)/servico_arquivo.cpp $(SRCDIR)/servico_rede.cpp $(SRCDIR)/servico_gui.cpp $(SRCDIR)/sqlite_servico.cpp

# Main
SOURCES_MAIN = $(SRCDIR)/main.cpp

# Todos os sources
SOURCES = $(SOURCES_MAIN) $(SOURCES_FASE1) $(SOURCES_FASE2) $(SOURCES_SERVICOS)

# Objects
OBJECTS_FASE1 = $(OUTDIR)/jit_compiler.o $(OUTDIR)/garbage_collector.o $(OUTDIR)/memory_pool.o $(OUTDIR)/string_pool.o
OBJECTS_FASE2 = $(OUTDIR)/jit_optimizing.o $(OUTDIR)/inline_cache.o $(OUTDIR)/type_specializer.o $(OUTDIR)/compilation_cache.o
OBJECTS_SERVICOS = $(OUTDIR)/servico_arquivo.o $(OUTDIR)/servico_rede.o $(OUTDIR)/servico_gui.o $(OUTDIR)/sqlite_servico.o
OBJECTS_MAIN = $(OUTDIR)/main.o

OBJECTS = $(OBJECTS_MAIN) $(OBJECTS_FASE1) $(OBJECTS_FASE2) $(OBJECTS_SERVICOS)
TARGET = $(OUTDIR)/br.exe

.PHONY: all build clean run test help

all: build

help:
	@echo "Linguagem BR - Makefile"
	@echo ""
	@echo "Comandos disponíveis:"
	@echo "  make build      - Compila o runtime"
	@echo "  make run        - Executa um arquivo .br"
	@echo "  make test       - Executa testes"
	@echo "  make clean      - Remove arquivos compilados"
	@echo "  make help       - Mostra esta ajuda"
	@echo ""
	@echo "Exemplos:"
	@echo "  make build"
	@echo "  make run ARQUIVO=exemplos/demo_funcionalidades_avancadas.br"

build: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linkando executável..."
	$(CXX) $(OBJECTS) -o $(TARGET)
	@echo "Build concluído: $(TARGET)"

$(OUTDIR)/main.o: $(SRCDIR)/main.cpp
	@echo "Compilando main.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Fase 1 - Fundamentos
$(OUTDIR)/jit_compiler.o: $(SRCDIR)/jit_compiler.cpp
	@echo "Compilando jit_compiler.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/garbage_collector.o: $(SRCDIR)/garbage_collector.cpp
	@echo "Compilando garbage_collector.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/memory_pool.o: $(SRCDIR)/memory_pool.cpp
	@echo "Compilando memory_pool.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/string_pool.o: $(SRCDIR)/string_pool.cpp
	@echo "Compilando string_pool.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Fase 2 - Otimizações
$(OUTDIR)/jit_optimizing.o: $(SRCDIR)/jit_optimizing.cpp
	@echo "Compilando jit_optimizing.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/inline_cache.o: $(SRCDIR)/inline_cache.cpp
	@echo "Compilando inline_cache.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/type_specializer.o: $(SRCDIR)/type_specialization.cpp
	@echo "Compilando type_specializer.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/compilation_cache.o: $(SRCDIR)/compilation_cache.cpp
	@echo "Compilando compilation_cache.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Serviços existentes
$(OUTDIR)/servico_arquivo.o: $(SRCDIR)/servico_arquivo.cpp
	@echo "Compilando servico_arquivo.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/servico_rede.o: $(SRCDIR)/servico_rede.cpp
	@echo "Compilando servico_rede.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/servico_gui.o: $(SRCDIR)/servico_gui.cpp
	@echo "Compilando servico_gui.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OUTDIR)/sqlite_servico.o: $(SRCDIR)/sqlite_servico.cpp
	@echo "Compilando sqlite_servico.cpp..."
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

run: build
	@if [ -z "$(ARQUIVO)" ]; then \
		echo "Erro: Especifique o arquivo com ARQUIVO=exemplo.br"; \
		exit 1; \
	fi
	@echo "Executando $(ARQUIVO)..."
	@$(TARGET) $(ARQUIVO)

test:
	@echo "Executando testes..."
	@python ferramentas/tester.py testes/

clean:
	@echo "Limpando arquivos compilados..."
	@del /Q $(OUTDIR)\*.o 2>nul || true
	@del /Q $(TARGET) 2>nul || true
	@echo "Limpeza concluída!"