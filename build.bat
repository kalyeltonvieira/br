@echo off
echo ========================================
echo   Building Linguagem BR v3.0.0 (Performance)
echo ========================================
echo.

cd runtime/src

echo Compilando main.cpp...
g++ -std=c++17 -O2 -I../include -c main.cpp -o main.o
if errorlevel 1 (
    echo ERRO ao compilar main.cpp
    pause
    exit /b 1
)

echo Compilando servico_arquivo.cpp...
g++ -std=c++17 -O2 -I../include -c servico_arquivo.cpp -o servico_arquivo.o
if errorlevel 1 (
    echo ERRO ao compilar servico_arquivo.cpp
    pause
    exit /b 1
)

echo Compilando servico_rede.cpp...
g++ -std=c++17 -O2 -I../include -c servico_rede.cpp -o servico_rede.o
if errorlevel 1 (
    echo ERRO ao compilar servico_rede.cpp
    pause
    exit /b 1
)

echo Compilando servico_gui.cpp...
g++ -std=c++17 -O2 -I../include -c servico_gui.cpp -o servico_gui.o
if errorlevel 1 (
    echo ERRO ao compilar servico_gui.cpp
    pause
    exit /b 1
)

echo Compilando sqlite_servico.cpp...
g++ -std=c++17 -O2 -I../include -c sqlite_servico.cpp -o sqlite_servico.o
if errorlevel 1 (
    echo ERRO ao compilar sqlite_servico.cpp
    pause
    exit /b 1
)

echo.
echo --- FASE 1: Fundamentos ---
echo.

echo Compilando jit_compiler.cpp...
g++ -std=c++17 -O2 -I../include -c jit_compiler.cpp -o jit_compiler.o
if errorlevel 1 (
    echo ERRO ao compilar jit_compiler.cpp
    pause
    exit /b 1
)

echo Compilando garbage_collector.cpp...
g++ -std=c++17 -O2 -I../include -c garbage_collector.cpp -o garbage_collector.o
if errorlevel 1 (
    echo ERRO ao compilar garbage_collector.cpp
    pause
    exit /b 1
)

echo Compilando memory_pool.cpp...
g++ -std=c++17 -O2 -I../include -c memory_pool.cpp -o memory_pool.o
if errorlevel 1 (
    echo ERRO ao compilar memory_pool.cpp
    pause
    exit /b 1
)

echo Compilando string_pool.cpp...
g++ -std=c++17 -O2 -I../include -c string_pool.cpp -o string_pool.o
if errorlevel 1 (
    echo ERRO ao compilar string_pool.cpp
    pause
    exit /b 1
)

echo.
echo --- FASE 2: Otimizacoes ---
echo.

echo Compilando jit_optimizing.cpp...
g++ -std=c++17 -O2 -I../include -c jit_optimizing.cpp -o jit_optimizing.o
if errorlevel 1 (
    echo ERRO ao compilar jit_optimizing.cpp
    pause
    exit /b 1
)

echo Compilando inline_cache.cpp...
g++ -std=c++17 -O2 -I../include -c inline_cache.cpp -o inline_cache.o
if errorlevel 1 (
    echo ERRO ao compilar inline_cache.cpp
    pause
    exit /b 1
)

echo Compilando type_specialization.cpp...
g++ -std=c++17 -O2 -I../include -c type_specialization.cpp -o type_specializer.o
if errorlevel 1 (
    echo ERRO ao compilar type_specialization.cpp
    pause
    exit /b 1
)

echo Compilando compilation_cache.cpp...
g++ -std=c++17 -O2 -I../include -c compilation_cache.cpp -o compilation_cache.o
if errorlevel 1 (
    echo ERRO ao compilar compilation_cache.cpp
    pause
    exit /b 1
)

echo.
echo Linkando objetos...
g++ main.o servico_arquivo.o servico_rede.o servico_gui.o sqlite_servico.o jit_compiler.o garbage_collector.o memory_pool.o string_pool.o jit_optimizing.o inline_cache.o type_specializer.o compilation_cache.o -o ../../bin/br.exe
if errorlevel 1 (
    echo ERRO ao linkar
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Build concluido com sucesso!
echo ========================================
echo.
echo Executavel criado: bin/br.exe
echo.
pause