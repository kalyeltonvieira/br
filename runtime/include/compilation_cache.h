// Compilation Cache (Incremental) para Linguagem BR
// Cache de compilação incremental com hash-based invalidation
#ifndef COMPILATION_CACHE_H
#define COMPILATION_CACHE_H

#include <string>
#include <map>
#include <vector>
#include <chrono>
#include <functional>
#include <cstdint>

// Forward declarations
struct NoAST;

// Entrada de cache
struct CacheEntry {
    std::string hash_arquivo;
    std::chrono::system_clock::time_point timestamp;
    void* codigo_compilado;
    size_t tamanho_codigo;
    std::string caminho_arquivo;
    std::vector<std::string> dependencias;
    bool valido;
    
    CacheEntry() : codigo_compilado(nullptr), tamanho_codigo(0), valido(true) {
        timestamp = std::chrono::system_clock::now();
    }
    
    bool expirado(int segundos_maximo = 3600) const {
        auto agora = std::chrono::system_clock::now();
        auto duracao = std::chrono::duration_cast<std::chrono::seconds>(agora - timestamp);
        return duracao.count() > segundos_maximo;
    }
};

// Metadata de dependência
struct DependencyMetadata {
    std::string caminho;
    std::string hash_conteudo;
    std::chrono::system_clock::time_point modificado;
    
    DependencyMetadata() {}
    DependencyMetadata(const std::string& path) : caminho(path) {
        modificado = std::chrono::system_clock::now();
    }
};

// Compilation Cache principal
class CompilationCache {
private:
    // Cache de código compilado
    std::map<std::string, CacheEntry> cache;
    
    // Metadata de dependências
    std::map<std::string, DependencyMetadata> dependencias;
    
    // Diretório de cache
    std::string diretorio_cache;
    
    // Estatísticas
    size_t total_hits;
    size_t total_misses;
    size_t total_invalidacoes;
    size_t memoria_cache_usada;
    
    // Configurações
    size_t tamanho_maximo_cache;
    int tempo_maximo_segundos;
    bool habilitado;
    
public:
    CompilationCache();
    ~CompilationCache();
    
    // Operações de cache
    void* carregar_cache(const std::string& arquivo);
    bool salvar_cache(const std::string& arquivo, void* codigo, size_t tamanho);
    
    // Compilação incremental
    void* recompilar_incremental(const std::string& arquivo, 
                                  const std::vector<std::string>& arquivos_modificados,
                                  std::function<void*(const std::string&)> compilador);
    
    // Gerenciamento de dependências
    void adicionar_dependencia(const std::string& arquivo, const std::string& dependencia);
    std::vector<std::string> obter_dependencias(const std::string& arquivo);
    bool dependencia_modificada(const std::string& dependencia);
    
    // Invalidação
    void invalidar_arquivo(const std::string& arquivo);
    void invalidar_dependencias(const std::string& arquivo);
    void invalidar_tudo();
    void invalidar_expirados();
    
    // Hash
    std::string calcular_hash_arquivo(const std::string& arquivo);
    std::string calcular_hash_conteudo(const std::string& conteudo);
    std::string calcular_hash_string(const std::string& s);
    
    // Persistência
    bool salvar_cache_disco();
    bool carregar_cache_disco();
    void limpar_cache_disco();
    
    // Estatísticas
    size_t obter_tamanho_cache() const { return cache.size(); }
    size_t obter_hits() const { return total_hits; }
    size_t obter_misses() const { return total_misses; }
    size_t obter_invalidacoes() const { return total_invalidacoes; }
    double obter_taxa_hit() const;
    size_t obter_memoria_usada() const { return memoria_cache_usada; }
    
    // Configuração
    void definir_diretorio_cache(const std::string& dir);
    void definir_tamanho_maximo(size_t bytes);
    void definir_tempo_maximo(int segundos);
    void habilitar(bool hab);
    bool esta_habilitado() const { return habilitado; }
    
    // Limpeza
    void limpar();
    void compactar();
    
    // Relatório
    void relatorio_estatisticas();
    void relatorio_entradas();
    
private:
    void* carregar_codigo_compilado(const std::string& caminho_cache);
    bool salvar_codigo_compilado(const std::string& caminho_cache, void* codigo, size_t tamanho);
    
    bool entrada_valida(const CacheEntry& entry);
    void atualizar_estatisticas_hit();
    void atualizar_estatisticas_miss();
    void atualizar_estatisticas_invalidacao();
    
    void evict_entradas_antigas();
    void evict_entradas_aleatorias(size_t quantidade);
    
    std::string gerar_caminho_cache(const std::string& arquivo);
    std::string gerar_nome_arquivo_cache(const std::string& hash);
};

// Cache incremental para AST
class ASTCache {
private:
    std::map<std::string, NoAST*> ast_cache;
    std::map<std::string, std::string> ast_hashes;
    
public:
    ASTCache();
    ~ASTCache();
    
    NoAST* obter_ast(const std::string& arquivo);
    void salvar_ast(const std::string& arquivo, NoAST* ast);
    void invalidar_ast(const std::string& arquivo);
    
    bool ast_modificado(const std::string& arquivo, const std::string& novo_hash);
    std::string calcular_hash_ast(NoAST* ast);
    
    void limpar();
    size_t tamanho() const { return ast_cache.size(); }
};

// Cache para bytecode
class BytecodeCache {
private:
    std::map<std::string, std::vector<uint8_t>> bytecode_cache;
    std::map<std::string, std::string> bytecode_hashes;
    
public:
    BytecodeCache();
    ~BytecodeCache();
    
    std::vector<uint8_t> obter_bytecode(const std::string& arquivo);
    void salvar_bytecode(const std::string& arquivo, const std::vector<uint8_t>& bytecode);
    void invalidar_bytecode(const std::string& arquivo);
    
    bool bytecode_modificado(const std::string& arquivo, const std::string& novo_hash);
    
    void limpar();
    size_t tamanho() const { return bytecode_cache.size(); }
};

#endif // COMPILATION_CACHE_H
