// Compilation Cache (Incremental) - Implementação
#include "compilation_cache.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <random>

namespace fs = std::filesystem;

// ===================== COMPILATION CACHE IMPLEMENTATION =====================

CompilationCache::CompilationCache() 
    : total_hits(0), total_misses(0), total_invalidacoes(0), memoria_cache_usada(0),
      tamanho_maximo_cache(100 * 1024 * 1024), // 100MB
      tempo_maximo_segundos(3600), // 1 hora
      habilitado(true) {
    
    // Define diretório de cache padrão
    diretorio_cache = ".br_cache";
    
    // Cria diretório se não existir
    if (!fs::exists(diretorio_cache)) {
        fs::create_directories(diretorio_cache);
    }
    
    std::cout << "[CompilationCache] Inicializado (dir: " << diretorio_cache << ")" << std::endl;
}

CompilationCache::~CompilationCache() {
    // Salva cache em disco antes de destruir
    salvar_cache_disco();
    
    // Limpa memória
    limpar();
}

void* CompilationCache::carregar_cache(const std::string& arquivo) {
    if (!habilitado) return nullptr;
    
    std::string hash = calcular_hash_arquivo(arquivo);
    
    auto it = cache.find(hash);
    if (it != cache.end() && entrada_valida(it->second)) {
        atualizar_estatisticas_hit();
        std::cout << "[CompilationCache] HIT: " << arquivo << std::endl;
        return it->second.codigo_compilado;
    }
    
    atualizar_estatisticas_miss();
    std::cout << "[CompilationCache] MISS: " << arquivo << std::endl;
    return nullptr;
}

bool CompilationCache::salvar_cache(const std::string& arquivo, void* codigo, size_t tamanho) {
    if (!habilitado || !codigo) return false;
    
    std::string hash = calcular_hash_arquivo(arquivo);
    
    // Verifica se cache está cheio
    if (memoria_cache_usada + tamanho > tamanho_maximo_cache) {
        evict_entradas_antigas();
    }
    
    // Cria entrada de cache
    CacheEntry entry;
    entry.hash_arquivo = hash;
    entry.codigo_compilado = codigo;
    entry.tamanho_codigo = tamanho;
    entry.caminho_arquivo = arquivo;
    entry.valido = true;
    
    cache[hash] = entry;
    memoria_cache_usada += tamanho;
    
    // Salva em disco
    std::string caminho_cache = gerar_caminho_cache(arquivo);
    salvar_codigo_compilado(caminho_cache, codigo, tamanho);
    
    std::cout << "[CompilationCache] SALVO: " << arquivo << " (" << tamanho << " bytes)" << std::endl;
    return true;
}

void* CompilationCache::recompilar_incremental(const std::string& arquivo, 
                                                  const std::vector<std::string>& arquivos_modificados,
                                                  std::function<void*(const std::string&)> compilador) {
    if (!habilitado) return compilador(arquivo);
    
    // Verifica se arquivo foi modificado
    std::string hash_atual = calcular_hash_arquivo(arquivo);
    
    auto it = cache.find(hash_atual);
    if (it != cache.end() && entrada_valida(it->second)) {
        // Verifica dependências
        bool dependencias_validas = true;
        for (const auto& dep : it->second.dependencias) {
            if (dependencia_modificada(dep)) {
                dependencias_validas = false;
                break;
            }
        }
        
        if (dependencias_validas) {
            atualizar_estatisticas_hit();
            std::cout << "[CompilationCache] REUSE (incremental): " << arquivo << std::endl;
            return it->second.codigo_compilado;
        }
    }
    
    // Recompila
    std::cout << "[CompilationCache] RECOMPILANDO (incremental): " << arquivo << std::endl;
    void* codigo = compilador(arquivo);
    
    if (codigo) {
        // Salva no cache
        size_t tamanho = 0; // Placeholder - na prática obteria tamanho real
        salvar_cache(arquivo, codigo, tamanho);
    }
    
    return codigo;
}

void CompilationCache::adicionar_dependencia(const std::string& arquivo, const std::string& dependencia) {
    std::string hash = calcular_hash_arquivo(arquivo);
    
    auto it = cache.find(hash);
    if (it != cache.end()) {
        it->second.dependencias.push_back(dependencia);
        
        // Registra metadata da dependência
        DependencyMetadata meta(dependencia);
        dependencias[dependencia] = meta;
    }
}

std::vector<std::string> CompilationCache::obter_dependencias(const std::string& arquivo) {
    std::string hash = calcular_hash_arquivo(arquivo);
    
    auto it = cache.find(hash);
    if (it != cache.end()) {
        return it->second.dependencias;
    }
    
    return {};
}

bool CompilationCache::dependencia_modificada(const std::string& dependencia) {
    auto it = dependencias.find(dependencia);
    if (it == dependencias.end()) return false;
    
    std::string hash_atual = calcular_hash_arquivo(dependencia);
    return hash_atual != it->second.hash_conteudo;
}

void CompilationCache::invalidar_arquivo(const std::string& arquivo) {
    std::string hash = calcular_hash_arquivo(arquivo);
    
    auto it = cache.find(hash);
    if (it != cache.end()) {
        if (it->second.codigo_compilado) {
            // Liberar código (placeholder)
            // free(it->second.codigo_compilado);
        }
        
        memoria_cache_usada -= it->second.tamanho_codigo;
        cache.erase(it);
        
        atualizar_estatisticas_invalidacao();
        std::cout << "[CompilationCache] INVALIDADO: " << arquivo << std::endl;
    }
}

void CompilationCache::invalidar_dependencias(const std::string& arquivo) {
    std::vector<std::string> deps = obter_dependencias(arquivo);
    
    for (const auto& dep : deps) {
        invalidar_arquivo(dep);
    }
}

void CompilationCache::invalidar_tudo() {
    size_t count = cache.size();
    
    for (auto& [hash, entry] : cache) {
        if (entry.codigo_compilado) {
            // free(entry.codigo_compilado);
        }
    }
    
    cache.clear();
    memoria_cache_usada = 0;
    total_invalidacoes += count;
    
    std::cout << "[CompilationCache] INVALIDADO TUDO (" << count << " entradas)" << std::endl;
}

void CompilationCache::invalidar_expirados() {
    std::vector<std::string> para_remover;
    
    for (auto& [hash, entry] : cache) {
        if (entry.expirado(tempo_maximo_segundos)) {
            para_remover.push_back(hash);
        }
    }
    
    for (const auto& hash : para_remover) {
        auto it = cache.find(hash);
        if (it != cache.end()) {
            if (it->second.codigo_compilado) {
                // free(it->second.codigo_compilado);
            }
            memoria_cache_usada -= it->second.tamanho_codigo;
            cache.erase(it);
            total_invalidacoes++;
        }
    }
    
    if (!para_remover.empty()) {
        std::cout << "[CompilationCache] EXPIRADOS removidos: " << para_remover.size() << std::endl;
    }
}

std::string CompilationCache::calcular_hash_arquivo(const std::string& arquivo) {
    // Lê arquivo e calcula hash
    std::ifstream file(arquivo, std::ios::binary);
    if (!file) return "";
    
    std::string conteudo((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    
    return calcular_hash_conteudo(conteudo);
}

std::string CompilationCache::calcular_hash_conteudo(const std::string& conteudo) {
    // Hash simples (placeholder - na prática usaria SHA256 ou similar)
    size_t hash = std::hash<std::string>{}(conteudo);
    return std::to_string(hash);
}

std::string CompilationCache::calcular_hash_string(const std::string& s) {
    return std::to_string(std::hash<std::string>{}(s));
}

bool CompilationCache::salvar_cache_disco() {
    try {
        std::string caminho = diretorio_cache + "/cache_index.dat";
        std::ofstream file(caminho, std::ios::binary);
        
        if (!file) return false;
        
        // Salva metadata do cache
        for (const auto& [hash, entry] : cache) {
            file.write(hash.c_str(), hash.size());
            file.write("\0", 1);
            file.write((char*)&entry.tamanho_codigo, sizeof(size_t));
            file.write(entry.caminho_arquivo.c_str(), entry.caminho_arquivo.size());
            file.write("\0", 1);
        }
        
        std::cout << "[CompilationCache] Cache salvo em disco" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CompilationCache] Erro ao salvar cache: " << e.what() << std::endl;
        return false;
    }
}

bool CompilationCache::carregar_cache_disco() {
    try {
        std::string caminho = diretorio_cache + "/cache_index.dat";
        std::ifstream file(caminho, std::ios::binary);
        
        if (!file) return false;
        
        // Carrega metadata do cache
        while (file) {
            std::string hash;
            std::getline(file, hash, '\0');
            
            if (hash.empty()) break;
            
            size_t tamanho;
            file.read((char*)&tamanho, sizeof(size_t));
            
            std::string caminho_arquivo;
            std::getline(file, caminho_arquivo, '\0');
            
            // Recarrega código compilado do disco
            std::string caminho_cache = gerar_caminho_cache(caminho_arquivo);
            void* codigo = carregar_codigo_compilado(caminho_cache);
            
            if (codigo) {
                CacheEntry entry;
                entry.hash_arquivo = hash;
                entry.codigo_compilado = codigo;
                entry.tamanho_codigo = tamanho;
                entry.caminho_arquivo = caminho_arquivo;
                entry.valido = true;
                
                cache[hash] = entry;
                memoria_cache_usada += tamanho;
            }
        }
        
        std::cout << "[CompilationCache] Cache carregado do disco (" << cache.size() << " entradas)" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[CompilationCache] Erro ao carregar cache: " << e.what() << std::endl;
        return false;
    }
}

void CompilationCache::limpar_cache_disco() {
    try {
        if (fs::exists(diretorio_cache)) {
            fs::remove_all(diretorio_cache);
            fs::create_directories(diretorio_cache);
            std::cout << "[CompilationCache] Cache em disco limpo" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[CompilationCache] Erro ao limpar cache: " << e.what() << std::endl;
    }
}

double CompilationCache::obter_taxa_hit() const {
    size_t total = total_hits + total_misses;
    if (total == 0) return 0.0;
    return (double)total_hits / total * 100.0;
}

void CompilationCache::definir_diretorio_cache(const std::string& dir) {
    diretorio_cache = dir;
    
    if (!fs::exists(diretorio_cache)) {
        fs::create_directories(diretorio_cache);
    }
}

void CompilationCache::definir_tamanho_maximo(size_t bytes) {
    tamanho_maximo_cache = bytes;
}

void CompilationCache::definir_tempo_maximo(int segundos) {
    tempo_maximo_segundos = segundos;
}

void CompilationCache::habilitar(bool hab) {
    habilitado = hab;
    std::cout << "[CompilationCache] " << (hab ? "Habilitado" : "Desabilitado") << std::endl;
}

void CompilationCache::limpar() {
    for (auto& [hash, entry] : cache) {
        if (entry.codigo_compilado) {
            // free(entry.codigo_compilado);
        }
    }
    
    cache.clear();
    memoria_cache_usada = 0;
}

void CompilationCache::compactar() {
    // Remove entradas expiradas e evict se necessário
    invalidar_expirados();
    
    if (memoria_cache_usada > tamanho_maximo_cache) {
        evict_entradas_antigas();
    }
}

void CompilationCache::relatorio_estatisticas() {
    std::cout << "=== Relatório de Compilation Cache ===" << std::endl;
    std::cout << "Entradas no cache: " << cache.size() << std::endl;
    std::cout << "Memória usada: " << (memoria_cache_usada / 1024) << " KB / " 
              << (tamanho_maximo_cache / 1024) << " KB" << std::endl;
    std::cout << "Hits: " << total_hits << std::endl;
    std::cout << "Misses: " << total_misses << std::endl;
    std::cout << "Invalidações: " << total_invalidacoes << std::endl;
    std::cout << "Taxa de hit: " << obter_taxa_hit() << "%" << std::endl;
    std::cout << "Habilitado: " << (habilitado ? "sim" : "não") << std::endl;
}

void CompilationCache::relatorio_entradas() {
    std::cout << "\n--- Entradas do Cache ---" << std::endl;
    for (const auto& [hash, entry] : cache) {
        std::cout << entry.caminho_arquivo << " (" << entry.tamanho_codigo << " bytes)"
                  << " - válido: " << (entry.valido ? "sim" : "não") << std::endl;
    }
}

void* CompilationCache::carregar_codigo_compilado(const std::string& caminho_cache) {
    // Carrega código compilado do disco
    std::ifstream file(caminho_cache, std::ios::binary);
    if (!file) return nullptr;
    
    file.seekg(0, std::ios::end);
    size_t tamanho = file.tellg();
    file.seekg(0, std::ios::beg);
    
    void* codigo = malloc(tamanho);
    if (!codigo) return nullptr;
    
    file.read((char*)codigo, tamanho);
    return codigo;
}

bool CompilationCache::salvar_codigo_compilado(const std::string& caminho_cache, void* codigo, size_t tamanho) {
    // Salva código compilado no disco
    std::ofstream file(caminho_cache, std::ios::binary);
    if (!file) return false;
    
    file.write((char*)codigo, tamanho);
    return true;
}

bool CompilationCache::entrada_valida(const CacheEntry& entry) {
    return entry.valido && !entry.expirado(tempo_maximo_segundos);
}

void CompilationCache::atualizar_estatisticas_hit() {
    total_hits++;
}

void CompilationCache::atualizar_estatisticas_miss() {
    total_misses++;
}

void CompilationCache::atualizar_estatisticas_invalidacao() {
    total_invalidacoes++;
}

void CompilationCache::evict_entradas_antigas() {
    // Evict entradas mais antigas (LRU)
    std::vector<std::pair<std::chrono::system_clock::time_point, std::string>> entradas_ordenadas;
    
    for (const auto& [hash, entry] : cache) {
        entradas_ordenadas.push_back({entry.timestamp, hash});
    }
    
    std::sort(entradas_ordenadas.begin(), entradas_ordenadas.end());
    
    size_t para_remover = entradas_ordenadas.size() / 4; // Remove 25% mais antigos
    
    for (size_t i = 0; i < para_remover; i++) {
        auto it = cache.find(entradas_ordenadas[i].second);
        if (it != cache.end()) {
            if (it->second.codigo_compilado) {
                // free(it->second.codigo_compilado);
            }
            memoria_cache_usada -= it->second.tamanho_codigo;
            cache.erase(it);
            total_invalidacoes++;
        }
    }
    
    std::cout << "[CompilationCache] Evicted " << para_remover << " entradas antigas" << std::endl;
}

void CompilationCache::evict_entradas_aleatorias(size_t quantidade) {
    std::vector<std::string> chaves;
    for (const auto& [hash, entry] : cache) {
        chaves.push_back(hash);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(chaves.begin(), chaves.end(), g);
    
    for (size_t i = 0; i < std::min(quantidade, chaves.size()); i++) {
        auto it = cache.find(chaves[i]);
        if (it != cache.end()) {
            if (it->second.codigo_compilado) {
                // free(it->second.codigo_compilado);
            }
            memoria_cache_usada -= it->second.tamanho_codigo;
            cache.erase(it);
            total_invalidacoes++;
        }
    }
}

std::string CompilationCache::gerar_caminho_cache(const std::string& arquivo) {
    std::string hash = calcular_hash_arquivo(arquivo);
    std::string nome_arquivo = gerar_nome_arquivo_cache(hash);
    return diretorio_cache + "/" + nome_arquivo;
}

std::string CompilationCache::gerar_nome_arquivo_cache(const std::string& hash) {
    return "cache_" + hash + ".bin";
}

// ===================== AST CACHE =====================

ASTCache::ASTCache() {
}

ASTCache::~ASTCache() {
    limpar();
}

NoAST* ASTCache::obter_ast(const std::string& arquivo) {
    auto it = ast_cache.find(arquivo);
    if (it != ast_cache.end()) {
        return it->second;
    }
    return nullptr;
}

void ASTCache::salvar_ast(const std::string& arquivo, NoAST* ast) {
    ast_cache[arquivo] = ast;
    ast_hashes[arquivo] = calcular_hash_ast(ast);
}

void ASTCache::invalidar_ast(const std::string& arquivo) {
    ast_cache.erase(arquivo);
    ast_hashes.erase(arquivo);
}

bool ASTCache::ast_modificado(const std::string& arquivo, const std::string& novo_hash) {
    auto it = ast_hashes.find(arquivo);
    if (it == ast_hashes.end()) return true;
    return it->second != novo_hash;
}

std::string ASTCache::calcular_hash_ast(NoAST* ast) {
    // Hash simplificado da AST (placeholder)
    return std::to_string((uintptr_t)ast);
}

void ASTCache::limpar() {
    ast_cache.clear();
    ast_hashes.clear();
}

// ===================== BYTECODE CACHE =====================

BytecodeCache::BytecodeCache() {
}

BytecodeCache::~BytecodeCache() {
    limpar();
}

std::vector<uint8_t> BytecodeCache::obter_bytecode(const std::string& arquivo) {
    auto it = bytecode_cache.find(arquivo);
    if (it != bytecode_cache.end()) {
        return it->second;
    }
    return {};
}

void BytecodeCache::salvar_bytecode(const std::string& arquivo, const std::vector<uint8_t>& bytecode) {
    bytecode_cache[arquivo] = bytecode;
    bytecode_hashes[arquivo] = std::to_string(std::hash<std::string>{}(
        std::string(bytecode.begin(), bytecode.end())
    ));
}

void BytecodeCache::invalidar_bytecode(const std::string& arquivo) {
    bytecode_cache.erase(arquivo);
    bytecode_hashes.erase(arquivo);
}

bool BytecodeCache::bytecode_modificado(const std::string& arquivo, const std::string& novo_hash) {
    auto it = bytecode_hashes.find(arquivo);
    if (it == bytecode_hashes.end()) return true;
    return it->second != novo_hash;
}

void BytecodeCache::limpar() {
    bytecode_cache.clear();
    bytecode_hashes.clear();
}
