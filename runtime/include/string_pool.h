// String Pool Básico (Interning) para Linguagem BR
// Implementação com string interning para evitar cópias
#ifndef STRING_POOL_H
#define STRING_POOL_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <vector>
#include <string_view>

// String interning pool
class StringPool {
private:
    // Mapa de strings internadas (string original -> string_view)
    std::unordered_map<std::string, std::string_view> interned_strings;
    
    // Conjunto de strings para verificação rápida
    std::unordered_set<std::string_view> interned_set;
    
    // Armazenamento das strings (para manter string_views válidos)
    std::vector<std::string> storage;
    
    // Mutex para thread safety
    std::mutex mutex;
    
    // Estatísticas
    size_t strings_internadas;
    size_t hits_cache;
    size_t misses_cache;
    size_t memoria_economizada;
    
public:
    StringPool();
    ~StringPool();
    
    // String interning
    std::string_view internar(const std::string& s);
    std::string_view internar(std::string&& s);
    std::string_view internar(const char* s, size_t tamanho);
    
    // Verificação
    bool esta_internada(const std::string& s) const;
    bool esta_internada(std::string_view sv) const;
    
    // Obter string internada (retorna string_view se existir)
    std::string_view obter_internada(const std::string& s) const;
    
    // Limpeza
    void limpar();
    size_t limpar_nao_usadas();
    
    // Estatísticas
    size_t obter_contagem() const;
    size_t obter_hits() const;
    size_t obter_misses() const;
    double obter_taxa_hit() const;
    size_t obter_memoria_economizada() const;
    
    // Configuração
    void definir_capacidade_maxima(size_t max);
    size_t obter_capacidade_maxima() const;
    
private:
    size_t capacidade_maxima;
    
    void adicionar_storage(const std::string& s);
    std::string_view criar_string_view(const std::string& s);
};

// StringBuilder para evitar cópias em concatenações
class StringBuilder {
private:
    std::vector<char> buffer;
    size_t tamanho_usado;
    
public:
    StringBuilder(size_t capacidade_inicial = 256);
    ~StringBuilder();
    
    // Append operations
    void append(const std::string& s);
    void append(const char* s, size_t tamanho);
    void append(char c);
    void append(std::string_view sv);
    
    // Append com formatação
    void append_format(const char* formato, ...);
    
    // Build final string
    std::string build();
    std::string_view build_view(StringPool* pool);
    
    // Reset
    void clear();
    void reserve(size_t capacidade);
    
    // Informações
    size_t tamanho() const;
    size_t capacidade() const;
    bool vazio() const;
};

// String hash otimizado (com cache)
struct CachedStringHash {
    size_t hash_cache;
    std::string string_data;
    
    CachedStringHash(const std::string& s) : string_data(s) {
        hash_cache = std::hash<std::string>{}(s);
    }
    
    CachedStringHash(const CachedStringHash& other) 
        : hash_cache(other.hash_cache), string_data(other.string_data) {}
    
    CachedStringHash& operator=(const CachedStringHash& other) {
        if (this != &other) {
            hash_cache = other.hash_cache;
            string_data = other.string_data;
        }
        return *this;
    }
    
    size_t hash() const { return hash_cache; }
    const std::string& str() const { return string_data; }
};

// Funções utilitárias para strings
class StringUtils {
public:
    // Concatenação sem cópia (usando views)
    static std::string concatenar_view(std::string_view a, std::string_view b);
    static std::string concatenar_view(std::vector<std::string_view> partes);
    
    // Substring sem cópia
    static std::string_view substring_view(std::string_view s, size_t start, size_t end);
    
    // Comparação otimizada
    static bool igual_ignorando_caso(std::string_view a, std::string_view b);
    static bool comeca_com(std::string_view str, std::string_view prefixo);
    static bool termina_com(std::string_view str, std::string_view sufixo);
    
    // Hash otimizado
    static size_t hash_string(std::string_view s);
    static size_t hash_string_case_insensitive(std::string_view s);
    
    // Split sem cópia
    static std::vector<std::string_view> split_view(std::string_view s, char delimitador);
    static std::vector<std::string_view> split_view(std::string_view s, std::string_view delimitador);
};

#endif // STRING_POOL_H
