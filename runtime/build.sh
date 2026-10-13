#!/bin/bash

# Script de build para Runtime BR Nativo

set -e  # Sair em caso de erro

echo "=== Build do Runtime BR Nativo ==="

# Verificar dependências
echo "Verificando dependências..."

if ! command -v g++ &> /dev/null; then
    echo "Erro: g++ não encontrado. Instale com:"
    echo "  Ubuntu/Debian: sudo apt install g++"
    echo "  macOS: brew install gcc"
    echo "  Windows: Instale MinGW ou MSVC"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "Aviso: cmake não encontrado, usando Makefile básico"
    USE_CMAKE=false
else
    USE_CMAKE=true
fi

# Criar diretórios
echo "Criando diretórios..."
mkdir -p obj bin

# Compilar
echo "Compilando..."

if [ "$USE_CMAKE" = true ]; then
    echo "Usando CMake..."
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    cd ..
else
    echo "Usando Makefile..."
    make all
fi

# Testar
echo "Testando runtime..."
if [ -f "bin/br" ]; then
    echo "Versão:"
    ./bin/br --version
    
    echo "Teste básico:"
    echo 'mostre "Olá Mundo do Runtime BR Nativo!"' | ./bin/br -c
    
    echo "Teste de tokenização:"
    ./bin/br -c 'função teste recebendo x e y devolva x + y fim'
    
    echo "✅ Build concluído com sucesso!"
    echo "Executável: ./bin/br"
    echo "Use: ./bin/br --help para opções"
else
    echo "❌ Erro: Executável não criado"
    exit 1
fi

# Opcional: instalar
if [ "$1" = "--install" ]; then
    echo "Instalando..."
    sudo cp bin/br /usr/local/bin/
    sudo mkdir -p /usr/local/lib/br
    sudo cp -r lib/* /usr/local/lib/br/
    echo "✅ Instalação concluída!"
    echo "Use: br --help"
fi

# Opcional: limpar
if [ "$1" = "--clean" ]; then
    echo "Limpando..."
    make clean
    rm -rf build
    echo "✅ Limpeza concluída!"
fi

# Ajuda
if [ "$1" = "--help" ]; then
    echo "Uso: $0 [opção]"
    echo "Opções:"
    echo "  --install   Instala o runtime no sistema"
    echo "  --clean     Limpa arquivos de build"
    echo "  --help      Exibe esta ajuda"
fi