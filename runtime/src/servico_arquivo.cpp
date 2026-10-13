// Implementação do Serviço de Arquivos Nativo BR
#include "servico_arquivo.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir _mkdir
#define SEPARADOR "\\"
#else
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#define SEPARADOR "/"
#endif

namespace fs = std::filesystem;

// ============ IMPLEMENTAÇÃO ============

std::string ServicoArquivo::ler(const std::string& caminho) {
    std::ifstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    return buffer.str();
}

bool ServicoArquivo::escrever(const std::string& caminho, const std::string& conteudo) {
    garantir_diretorio_pai(caminho);
    
    std::ofstream arquivo(caminho, std::ios::binary);
    if (!arquivo.is_open()) {
        return false;
    }
    
    arquivo << conteudo;
    return !arquivo.fail();
}

bool ServicoArquivo::anexar(const std::string& caminho, const std::string& conteudo) {
    garantir_diretorio_pai(caminho);
    
    std::ofstream arquivo(caminho, std::ios::binary | std::ios::app);
    if (!arquivo.is_open()) {
        return false;
    }
    
    arquivo << conteudo;
    return !arquivo.fail();
}

bool ServicoArquivo::existe(const std::string& caminho) {
    return fs::exists(caminho);
}

size_t ServicoArquivo::tamanho(const std::string& caminho) {
    try {
        return fs::file_size(caminho);
    } catch (...) {
        return 0;
    }
}

std::string ServicoArquivo::modificado_em(const std::string& caminho) {
    try {
        auto ftime = fs::last_write_time(caminho);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
        std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
        std::tm tm = *std::localtime(&tt);
        
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    } catch (...) {
        return "";
    }
}

bool ServicoArquivo::eh_arquivo(const std::string& caminho) {
    return fs::is_regular_file(caminho);
}

bool ServicoArquivo::eh_diretorio(const std::string& caminho) {
    return fs::is_directory(caminho);
}

bool ServicoArquivo::criar_diretorio(const std::string& caminho) {
    try {
        return fs::create_directories(caminho);
    } catch (...) {
        return false;
    }
}

std::vector<std::string> ServicoArquivo::listar(const std::string& caminho) {
    std::vector<std::string> resultado;
    
    try {
        for (const auto& entry : fs::directory_iterator(caminho)) {
            std::string nome = entry.path().filename().string();
            
            // Ignorar diretórios especiais
            if (nome == "." || nome == "..") {
                continue;
            }
            
            resultado.push_back(nome);
        }
    } catch (...) {
        // Retorna lista vazia em caso de erro
    }
    
    return resultado;
}

bool ServicoArquivo::copiar(const std::string& origem, const std::string& destino) {
    try {
        garantir_diretorio_pai(destino);
        
        if (fs::is_directory(origem)) {
            fs::copy(origem, destino, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        } else {
            fs::copy_file(origem, destino, fs::copy_options::overwrite_existing);
        }
        return true;
    } catch (...) {
        return false;
    }
}

bool ServicoArquivo::mover(const std::string& origem, const std::string& destino) {
    try {
        garantir_diretorio_pai(destino);
        fs::rename(origem, destino);
        return true;
    } catch (...) {
        return false;
    }
}

bool ServicoArquivo::remover(const std::string& caminho) {
    try {
        if (fs::is_directory(caminho)) {
            return fs::remove_all(caminho) > 0;
        } else {
            return fs::remove(caminho);
        }
    } catch (...) {
        return false;
    }
}

bool ServicoArquivo::remover_diretorio(const std::string& caminho) {
    try {
        return fs::remove_all(caminho) > 0;
    } catch (...) {
        return false;
    }
}

std::string ServicoArquivo::caminho_absoluto(const std::string& caminho) {
    try {
        return fs::absolute(caminho).string();
    } catch (...) {
        return caminho;
    }
}

std::string ServicoArquivo::caminho_relativo(const std::string& caminho, const std::string& base) {
    try {
        return fs::relative(caminho, base).string();
    } catch (...) {
        return caminho;
    }
}

bool ServicoArquivo::pode_ler(const std::string& caminho) {
    struct stat info;
    if (stat(caminho.c_str(), &info) != 0) {
        return false;
    }
    
    // Verificar permissões de leitura
    return (info.st_mode & S_IRUSR) != 0;
}

bool ServicoArquivo::pode_escrever(const std::string& caminho) {
    struct stat info;
    if (stat(caminho.c_str(), &info) != 0) {
        return false;
    }
    
    // Verificar permissões de escrita
    return (info.st_mode & S_IWUSR) != 0;
}

bool ServicoArquivo::pode_executar(const std::string& caminho) {
    struct stat info;
    if (stat(caminho.c_str(), &info) != 0) {
        return false;
    }
    
    // Verificar permissões de execução
    return (info.st_mode & S_IXUSR) != 0;
}

bool ServicoArquivo::definir_permissao(const std::string& caminho, const std::string& permissao) {
    // Implementação simplificada
    // Em produção, implementar parse de permissões estilo Unix
    return true;
}

size_t ServicoArquivo::espaco_disponivel(const std::string& caminho) {
    try {
        fs::space_info info = fs::space(caminho);
        return info.available;
    } catch (...) {
        return 0;
    }
}

size_t ServicoArquivo::espaco_total(const std::string& caminho) {
    try {
        fs::space_info info = fs::space(caminho);
        return info.capacity;
    } catch (...) {
        return 0;
    }
}

size_t ServicoArquivo::espaco_usado(const std::string& caminho) {
    try {
        fs::space_info info = fs::space(caminho);
        return info.capacity - info.available;
    } catch (...) {
        return 0;
    }
}

std::string ServicoArquivo::criar_arquivo_temporario(const std::string& prefixo) {
    std::string template_str;
    
#ifdef _WIN32
    char temp_path[MAX_PATH];
    GetTempPathA(MAX_PATH, temp_path);
    template_str = std::string(temp_path) + prefixo + "XXXXXX";
    
    char* nome = _mktemp(&template_str[0]);
    if (nome) {
        return nome;
    }
#else
    template_str = "/tmp/" + prefixo + "XXXXXX";
    
    char buffer[1024];
    strcpy(buffer, template_str.c_str());
    
    int fd = mkstemp(buffer);
    if (fd != -1) {
        close(fd);
        return buffer;
    }
#endif
    
    return "";
}

std::string ServicoArquivo::criar_diretorio_temporario(const std::string& prefixo) {
    std::string base_dir;
    
#ifdef _WIN32
    char temp_path[MAX_PATH];
    GetTempPathA(MAX_PATH, temp_path);
    base_dir = temp_path;
#else
    base_dir = "/tmp/";
#endif
    
    // Gerar nome único baseado em timestamp
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    
    std::string dir_name = base_dir + prefixo + std::to_string(timestamp);
    
    if (criar_diretorio(dir_name)) {
        return dir_name;
    }
    
    return "";
}

std::string ServicoArquivo::nome_arquivo(const std::string& caminho) {
    fs::path p(caminho);
    return p.filename().string();
}

std::string ServicoArquivo::diretorio_pai(const std::string& caminho) {
    fs::path p(caminho);
    return p.parent_path().string();
}

std::string ServicoArquivo::extensao(const std::string& caminho) {
    fs::path p(caminho);
    return p.extension().string();
}

bool ServicoArquivo::eh_caminho_absoluto(const std::string& caminho) {
    fs::path p(caminho);
    return p.is_absolute();
}

// ============ FUNÇÕES PRIVADAS ============

std::string ServicoArquivo::normalizar_caminho(const std::string& caminho) {
    try {
        return fs::canonical(caminho).string();
    } catch (...) {
        return fs::absolute(caminho).string();
    }
}

std::vector<std::string> ServicoArquivo::dividir_caminho(const std::string& caminho) {
    std::vector<std::string> partes;
    std::string parte;
    std::istringstream stream(caminho);
    
    while (std::getline(stream, parte, '/')) {
        if (!parte.empty()) {
            partes.push_back(parte);
        }
    }
    
    return partes;
}

std::string ServicoArquivo::juntar_caminho(const std::vector<std::string>& partes) {
    std::string resultado;
    
    for (size_t i = 0; i < partes.size(); ++i) {
        if (i > 0) {
            resultado += "/";
        }
        resultado += partes[i];
    }
    
    return resultado;
}

void ServicoArquivo::garantir_diretorio_pai(const std::string& caminho) {
    std::string pai = diretorio_pai(caminho);
    if (!pai.empty() && !existe(pai)) {
        criar_diretorio(pai);
    }
}