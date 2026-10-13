// Inline Caching (Monomorphic/Polymorphic IC) para Linguagem BR
// Cache de dispatch para chamadas de método e propriedades
#ifndef INLINE_CACHE_H
#define INLINE_CACHE_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cstdint>

// Forward declarations
struct Valor;

// Tipo de cache inline
enum class TipoInlineCache {
    MONOMORPHIC,    // 1 tipo de recebedor (99% dos casos)
    POLYMORPHIC,    // 2-4 tipos de recebedor
    MEGAMORPHIC     // Fallback para hash table
};

// Entrada de cache monomórfico (1 tipo)
struct EntradaMonomorfica {
    std::string tipo_recebedor;
    void* codigo_compilado;
    int hits;
    int misses;
    
    EntradaMonomorfica() : codigo_compilado(nullptr), hits(0), misses(0) {}
    
    double hit_rate() const {
        int total = hits + misses;
        return total > 0 ? (double)hits / total * 100.0 : 0.0;
    }
};

// Entrada de cache polimórfico (2-4 tipos)
struct EntradaPolimorfica {
    static const int MAX_ENTRADAS = 4;
    
    struct Entrada {
        std::string tipo_recebedor;
        void* codigo_compilado;
        int hits;
        int misses;
    };
    
    Entrada entradas[MAX_ENTRADAS];
    int num_entradas;
    
    EntradaPolimorfica() : num_entradas(0) {
        for (int i = 0; i < MAX_ENTRADAS; i++) {
            entradas[i] = Entrada();
        }
    }
    
    bool adicionar(const std::string& tipo, void* codigo) {
        if (num_entradas >= MAX_ENTRADAS) return false;
        
        entradas[num_entradas].tipo_recebedor = tipo;
        entradas[num_entradas].codigo_compilado = codigo;
        num_entradas++;
        return true;
    }
    
    void* buscar(const std::string& tipo) {
        for (int i = 0; i < num_entradas; i++) {
            if (entradas[i].tipo_recebedor == tipo) {
                entradas[i].hits++;
                return entradas[i].codigo_compilado;
            }
            entradas[i].misses++;
        }
        return nullptr;
    }
    
    double hit_rate() const {
        int total_hits = 0;
        int total_misses = 0;
        
        for (int i = 0; i < num_entradas; i++) {
            total_hits += entradas[i].hits;
            total_misses += entradas[i].misses;
        }
        
        int total = total_hits + total_misses;
        return total > 0 ? (double)total_hits / total * 100.0 : 0.0;
    }
};

// Cache inline principal
class InlineCache {
private:
    std::string nome; // Nome do método ou propriedade
    TipoInlineCache tipo;
    
    // Cache monomórfico
    EntradaMonomorfica monomorfica;
    
    // Cache polimórfico
    EntradaPolimorfica polimorfica;
    
    // Cache megamórfico (fallback)
    std::map<std::string, void*> megamorfica;
    
    // Estatísticas
    int total_lookups;
    int total_hits;
    int total_misses;
    
public:
    InlineCache(const std::string& n);
    ~InlineCache();
    
    // Lookup
    void* lookup(Valor* receiver);
    void* lookup_tipo(const std::string& tipo);
    
    // Update
    void update(Valor* receiver, void* codigo);
    void update_tipo(const std::string& tipo, void* codigo);
    
    // Transição de tipo de cache
    void promover_para_polimorfico();
    void promover_para_megamorfico();
    
    // Estatísticas
    double hit_rate() const;
    int obter_total_lookups() const { return total_lookups; }
    int obter_total_hits() const { return total_hits; }
    int obter_total_misses() const { return total_misses; }
    
    TipoInlineCache obter_tipo() const { return tipo; }
    std::string obter_nome() const { return nome; }
    
    // Limpeza
    void invalidar();
    void invalidar_tipo(const std::string& tipo);
    void resetar_estatisticas();
    
private:
    void* lookup_monomorfico(const std::string& tipo);
    void* lookup_polimorfico(const std::string& tipo);
    void* lookup_megamorfico(const std::string& tipo);
    
    bool deve_promover_para_polimorfico();
    bool deve_promover_para_megamorfico();
};

// Gerenciador de inline caches
class InlineCacheManager {
private:
    std::map<std::string, InlineCache*> caches;
    
    // Estatísticas globais
    int total_caches_criados;
    int total_caches_promovidos_polimorfico;
    int total_caches_promovidos_megamorfico;
    
public:
    InlineCacheManager();
    ~InlineCacheManager();
    
    // Obter ou criar cache
    InlineCache* obter_ou_criar_cache(const std::string& nome);
    
    // Remover cache
    void remover_cache(const std::string& nome);
    void invalidar_todos();
    
    // Estatísticas globais
    int obter_total_caches() const { return caches.size(); }
    int obter_caches_criados() const { return total_caches_criados; }
    int obter_promocoes_polimorfico() const { return total_caches_promovidos_polimorfico; }
    int obter_promocoes_megamorfico() const { return total_caches_promovidos_megamorfico; }
    
    double hit_rate_medio() const;
    
    // Relatório detalhado
    void relatorio_estatisticas();
    
private:
    std::string extrair_nome_metodo(const std::string& nome_completo);
    std::string extrair_nome_propriedade(const std::string& nome_completo);
};

// Inline cache para propriedades (acesso .propriedade)
class PropertyInlineCache : public InlineCache {
public:
    PropertyInlineCache(const std::string& nome_propriedade);
    
    // Lookup específico para propriedades
    void* lookup_propriedade(Valor* objeto);
    void update_propriedade(Valor* objeto, size_t offset);
};

// Inline cache para chamadas de método (objeto.metodo())
class MethodInlineCache : public InlineCache {
public:
    MethodInlineCache(const std::string& nome_metodo);
    
    // Lookup específico para métodos
    void* lookup_metodo(Valor* objeto, std::vector<Valor> args);
    void update_metodo(Valor* objeto, void* codigo);
};

#endif // INLINE_CACHE_H
