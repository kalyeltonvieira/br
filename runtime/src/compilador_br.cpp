// Compilador Nativo BR - CLI
// Compila arquivos .br para bytecode .brc
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include "compilador_bytecode.h"

namespace fs = std::filesystem;

void mostrar_ajuda() {
    std::cout << "Compilador Nativo BR v2.0\n";
    std::cout << "===========================\n\n";
    std::cout << "Uso: br_compilar [opções] <arquivo.br>\n\n";
    std::cout << "Opções:\n";
    std::cout << "  -o, --saida <arquivo>    Arquivo de saída .brc (padrão: mesmo nome .brc)\n";
    std::cout << "  -O, --otimizar           Otimizar bytecode\n";
    std::cout << "  -v, --verbose            Mostrar informações detalhadas\n";
    std::cout << "  --perm-internet          Habilitar permissão de internet\n";
    std::cout << "  --perm-arquivos          Habilitar permissão de arquivos\n";
    std::cout << "  --perm-rede              Habilitar permissão de rede\n";
    std::cout << "  --perm-banco             Habilitar permissão de banco\n";
    std::cout << "  -h, --help               Mostrar esta ajuda\n";
    std::cout << "  --version                Mostrar versão\n\n";
    std::cout << "Exemplos:\n";
    std::cout << "  br_compilar programa.br\n";
    std::cout << "  br_compilar -o saida.brc programa.br\n";
    std::cout << "  br_compilar --perm-internet --perm-arquivos app.br\n\n";
}

void mostrar_versao() {
    std::cout << "Compilador Nativo BR v2.0.0\n";
    std::cout << "Runtime BR v2.0.0\n";
    std::cout << "Bytecode BRC1 v1.0\n";
}

std::string obter_saida_padrao(const std::string& entrada) {
    fs::path caminho(entrada);
    return caminho.replace_extension(".brc").string();
}

bool compilar_arquivo(const std::string& entrada, const std::string& saida, 
                       bool otimizar, bool verbose,
                       bool perm_internet, bool perm_arquivos, 
                       bool perm_rede, bool perm_banco) {
    if (verbose) {
        std::cout << "[Compilador] Lendo arquivo: " << entrada << std::endl;
    }
    
    // Verificar se arquivo existe
    if (!fs::exists(entrada)) {
        std::cerr << "[Erro] Arquivo não encontrado: " << entrada << std::endl;
        return false;
    }
    
    // Criar compilador
    CompiladorBytecode compilador;
    
    // Configurar permissões
    if (perm_internet) {
        compilador.definir_permissao_internet(true);
        if (verbose) std::cout << "[Compilador] Permissão de internet habilitada" << std::endl;
    }
    if (perm_arquivos) {
        compilador.definir_permissao_arquivos(true);
        if (verbose) std::cout << "[Compilador] Permissão de arquivos habilitada" << std::endl;
    }
    if (perm_rede) {
        compilador.definir_permissao_rede(true);
        if (verbose) std::cout << "[Compilador] Permissão de rede habilitada" << std::endl;
    }
    if (perm_banco) {
        compilador.definir_permissao_banco(true);
        if (verbose) std::cout << "[Compilador] Permissão de banco habilitada" << std::endl;
    }
    
    // Compilar
    if (verbose) {
        std::cout << "[Compilador] Compilando para bytecode..." << std::endl;
    }
    
    bool sucesso = compilador.compilar_arquivo(entrada);
    
    if (!sucesso) {
        std::cerr << "[Erro] Falha na compilação" << std::endl;
        
        auto erros = compilador.obter_erros();
        for (const auto& erro : erros) {
            std::cerr << "[Erro] " << erro << std::endl;
        }
        
        return false;
    }
    
    if (verbose) {
        std::cout << "[Compilador] Compilação concluída com sucesso" << std::endl;
    }
    
    // Otimizar se solicitado
    if (otimizar) {
        if (verbose) {
            std::cout << "[Compilador] Otimizando bytecode..." << std::endl;
        }
        // Implementação de otimizações seria aqui
        // Por enquanto, apenas marcamos
    }
    
    // Salvar bytecode
    if (verbose) {
        std::cout << "[Compilador] Salvando bytecode em: " << saida << std::endl;
    }
    
    sucesso = compilador.salvar_bytecode(saida);
    
    if (!sucesso) {
        std::cerr << "[Erro] Falha ao salvar bytecode" << std::endl;
        return false;
    }
    
    // Mostrar estatísticas
    if (verbose) {
        auto bytecode = compilador.obter_bytecode();
        std::cout << "[Compilador] Estatísticas:" << std::endl;
        std::cout << "  Tamanho do bytecode: " << bytecode.size() << " bytes" << std::endl;
        std::cout << "  Arquivo de saída: " << saida << std::endl;
        
        // Tamanho do arquivo
        auto tamanho_arquivo = fs::file_size(saida);
        std::cout << "  Tamanho do arquivo: " << tamanho_arquivo << " bytes" << std::endl;
    }
    
    std::cout << "[Sucesso] Bytecode gerado: " << saida << std::endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        mostrar_ajuda();
        return 1;
    }
    
    std::string arquivo_entrada;
    std::string arquivo_saida;
    bool otimizar = false;
    bool verbose = false;
    bool perm_internet = false;
    bool perm_arquivos = false;
    bool perm_rede = false;
    bool perm_banco = false;
    
    // Parsear argumentos
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            mostrar_ajuda();
            return 0;
        }
        else if (arg == "--version") {
            mostrar_versao();
            return 0;
        }
        else if (arg == "-o" || arg == "--saida") {
            if (i + 1 < argc) {
                arquivo_saida = argv[++i];
            } else {
                std::cerr << "[Erro] Argumento faltando para " << arg << std::endl;
                return 1;
            }
        }
        else if (arg == "-O" || arg == "--otimizar") {
            otimizar = true;
        }
        else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        }
        else if (arg == "--perm-internet") {
            perm_internet = true;
        }
        else if (arg == "--perm-arquivos") {
            perm_arquivos = true;
        }
        else if (arg == "--perm-rede") {
            perm_rede = true;
        }
        else if (arg == "--perm-banco") {
            perm_banco = true;
        }
        else if (arg[0] != '-') {
            // Arquivo de entrada
            if (arquivo_entrada.empty()) {
                arquivo_entrada = arg;
            } else {
                std::cerr << "[Erro] Múltiplos arquivos de entrada especificados" << std::endl;
                return 1;
            }
        }
        else {
            std::cerr << "[Erro] Opção desconhecida: " << arg << std::endl;
            return 1;
        }
    }
    
    // Verificar se arquivo de entrada foi especificado
    if (arquivo_entrada.empty()) {
        std::cerr << "[Erro] Nenhum arquivo de entrada especificado" << std::endl;
        mostrar_ajuda();
        return 1;
    }
    
    // Determinar arquivo de saída se não especificado
    if (arquivo_saida.empty()) {
        arquivo_saida = obter_saida_padrao(arquivo_entrada);
    }
    
    // Compilar
    bool sucesso = compilar_arquivo(arquivo_entrada, arquivo_saida, otimizar, verbose,
                                     perm_internet, perm_arquivos, perm_rede, perm_banco);
    
    return sucesso ? 0 : 1;
}
