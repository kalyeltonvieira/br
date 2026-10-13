// Inline Caching (Monomorphic/Polymorphic IC) - Implementação
#include "inline_cache.h"
#include <iostream>
#include <algorithm>

// ===================== INLINE CACHE IMPLEMENTATION =====================

InlineCache::InlineCache(const std::string& n) 
    : nome(n), tipo(TipoInlineCache::MONOMORPHIC), 
      total_lookups(0), total_hits(0), total_misses(0) {
}

InlineCache::~InlineCache() {
}

// ===================== LOOKUP =====================

void* InlineCache::lookup(Valor* receiver) {
    total_lookups++;
    
    // Extrai tipo do receiver
    std::string tipo = "desconhecido"; // Placeholder - na prática extrairia tipo real
    
    void* resultado = nullptr;
    
    switch (tipo) {
        case TipoInlineCache::MONOMORPHIC:
            resultado = lookup_monomorfico(tipo);
            break;
        case TipoInlineCache::POLYMORPHIC:
            resultado = lookup_polimorfico(tipo);
            break;
        case TipoInlineCache::MEGAMORPHIC:
            resultado = lookup_megamorfico(tipo);
            break;
    }
    
    if (resultado) {
        total_hits++;
    } else {
        total_misses++;
    }
    
    return resultado;
}

void* InlineCache::lookup_tipo(const std::string& tipo) {
    total_lookups++;
    
    void* resultado = nullptr;
    
    switch (tipo) {
        case TipoInlineCache::MONOMORPHIC:
            resultado = lookup_monomorfico(tipo);
            break;
        case TipoInlineCache::POLYMORPHIC:
            resultado = lookup_polimorfico(tipo);
            break;
        case TipoInlineCache::MEGAMORPHIC:
            resultado = lookup_megamorfico(tipo);
            break;
    }
    
    if (resultado) {
        total_hits++;
    } else {
        total_misses++;
    }
    
    return resultado;
}

void* InlineCache::lookup_monomorfico(const std::string& tipo) {
    if (monomorfica.tipo_recebedor == tipo) {
        monomorfica.hits++;
        return monomorfica.codigo_compilado;
    }
    
    monomorfica.misses++;
    
    // Verifica se deve promover para polimórfico
    if (deve_promover_para_polimorfico()) {
        promover_para_polimorfico();
    }
    
    return nullptr;
}

void* InlineCache::lookup_polimorfico(const std::string& tipo) {
    return polimorfica.buscar(tipo);
}

void* InlineCache::lookup_megamorfico(const std::string& tipo) {
    auto it = megamorfica.find(tipo);
    if (it != megamorfica.end()) {
        return it->second;
    }
    return nullptr;
}

// ===================== UPDATE =====================

void InlineCache::update(Valor* receiver, void* codigo) {
    std::string tipo = "desconhecido"; // Placeholder
    update_tipo(tipo, codigo);
}

void InlineCache::update_tipo(const std::string& tipo, void* codigo) {
    switch (tipo) {
        case TipoInlineCache::MONOMORPHIC:
            if (monomorfica.tipo_recebedor.empty()) {
                monomorfica.tipo_recebedor = tipo;
                monomorfica.codigo_compilado = codigo;
            } else if (monomorfica.tipo_recebedor != tipo) {
                // Tipo diferente - promover para polimórfico
                promover_para_polimorfico();
                polimorfica.adicionar(monomorfica.tipo_recebedor, monomorfica.codigo_compilado);
                polimorfica.adicionar(tipo, codigo);
            }
            break;
            
        case TipoInlineCache::POLYMORPHIC:
            if (!polimorfica.adicionar(tipo, codigo)) {
                // Cache cheio - promover para megamórfico
                promover_para_megamorfico();
                megamorfica[tipo] = codigo;
            }
            break;
            
        case TipoInlineCache::MEGAMORPHIC:
            megamorfica[tipo] = codigo;
            break;
    }
}

// ===================== PROMOÇÃO DE TIPO =====================

void InlineCache::promover_para_polimorfico() {
    if (tipo == TipoInlineCache::MONOMORPHIC) {
        tipo = TipoInlineCache::POLYMORPHIC;
        
        // Move entrada monomórfica para polimórfica
        if (!monomorfica.tipo_recebedor.empty()) {
            polimorfica.adicionar(monomorfica.tipo_recebedor, monomorfica.codigo_compilado);
        }
        
        std::cout << "[InlineCache] Promovido para polimórfico: " << nome << std::endl;
    }
}

void InlineCache::promover_para_megamorfico() {
    if (tipo != TipoInlineCache::MEGAMORPHIC) {
        tipo = TipoInlineCache::MEGAMORPHIC;
        
        // Move entradas polimórficas para megamórfica
        for (int i = 0; i < polimorfica.num_entradas; i++) {
            megamorfica[polimorfica.entradas[i].tipo_recebedor] = 
                polimorfica.entradas[i].codigo_compilado;
        }
        
        std::cout << "[InlineCache] Promovido para megamórfico: " << nome << std::endl;
    }
}

// ===================== ESTATÍSTICAS =====================

double InlineCache::hit_rate() const {
    int total = total_hits + total_misses;
    return total > 0 ? (double)total_hits / total * 100.0 : 0.0;
}

// ===================== LIMPEZA =====================

void InlineCache::invalidar() {
    monomorfica = EntradaMonomorfica();
    polimorfica = EntradaPolimorfica();
    megamorfica.clear();
    tipo = TipoInlineCache::MONOMORPHIC;
    resetar_estatisticas();
}

void InlineCache::invalidar_tipo(const std::string& tipo) {
    switch (tipo) {
        case TipoInlineCache::MONOMORPHIC:
            if (monomorfica.tipo_recebedor == tipo) {
                monomorfica = EntradaMonomorfica();
            }
            break;
        case TipoInlineCache::POLYMORPHIC:
            // Remove entrada polimórfica
            for (int i = 0; i < polimorfica.num_entradas; i++) {
                if (polimorfica.entradas[i].tipo_recebedor == tipo) {
                    // Shift remaining entries
                    for (int j = i; j < polimorfica.num_entradas - 1; j++) {
                        polimorfica.entradas[j] = polimorfica.entradas[j + 1];
                    }
                    polimorfica.num_entradas--;
                    break;
                }
            }
            break;
        case TipoInlineCache::MEGAMORPHIC:
            megamorfica.erase(tipo);
            break;
    }
}

void InlineCache::resetar_estatisticas() {
    total_lookups = 0;
    total_hits = 0;
    total_misses = 0;
}

// ===================== UTILITÁRIOS =====================

bool InlineCache::deve_promover_para_polimorfico() {
    // Promover se houver muitos misses no cache monomórfico
    return monomorfica.misses > 10 && monomorfica.hit_rate() < 80.0;
}

bool InlineCache::deve_promover_para_megamorfico() {
    // Promover se cache polimórfico estiver cheio e com baixo hit rate
    return tipo == TipoInlineCache::POLYMORPHIC && 
           polimorfica.num_entradas >= EntradaPolimorfica::MAX_ENTRADAS &&
           polimorfica.hit_rate() < 70.0;
}

// ===================== INLINE CACHE MANAGER =====================

InlineCacheManager::InlineCacheManager() 
    : total_caches_criados(0), total_caches_promovidos_polimorfico(0),
      total_caches_promovidos_megamorfico(0) {
}

InlineCacheManager::~InlineCacheManager() {
    for (auto& [nome, cache] : caches) {
        delete cache;
    }
    caches.clear();
}

InlineCache* InlineCacheManager::obter_ou_criar_cache(const std::string& nome) {
    auto it = caches.find(nome);
    if (it != caches.end()) {
        return it->second;
    }
    
    // Cria novo cache
    InlineCache* cache = new InlineCache(nome);
    caches[nome] = cache;
    total_caches_criados++;
    
    return cache;
}

void InlineCacheManager::remover_cache(const std::string& nome) {
    auto it = caches.find(nome);
    if (it != caches.end()) {
        delete it->second;
        caches.erase(it);
    }
}

void InlineCacheManager::invalidar_todos() {
    for (auto& [nome, cache] : caches) {
        cache->invalidar();
    }
}

double InlineCacheManager::hit_rate_medio() const {
    if (caches.empty()) return 0.0;
    
    double total_hit_rate = 0.0;
    for (const auto& [nome, cache] : caches) {
        total_hit_rate += cache->hit_rate();
    }
    
    return total_hit_rate / caches.size();
}

void InlineCacheManager::relatorio_estatisticas() {
    std::cout << "=== Relatório de Inline Caches ===" << std::endl;
    std::cout << "Total de caches: " << caches.size() << std::endl;
    std::cout << "Caches criados: " << total_caches_criados << std::endl;
    std::cout << "Promoções para polimórfico: " << total_caches_promovidos_polimorfico << std::endl;
    std::cout << "Promoções para megamórfico: " << total_caches_promovidos_megamorfico << std::endl;
    std::cout << "Hit rate médio: " << hit_rate_medio() << "%" << std::endl;
    
    std::cout << "\n--- Caches Individuais ---" << std::endl;
    for (const auto& [nome, cache] : caches) {
        std::string tipo_str;
        switch (cache->obter_tipo()) {
            case TipoInlineCache::MONOMORPHIC: tipo_str = "Monomórfico"; break;
            case TipoInlineCache::POLYMORPHIC: tipo_str = "Polimórfico"; break;
            case TipoInlineCache::MEGAMORPHIC: tipo_str = "Megamórfico"; break;
        }
        
        std::cout << nome << " (" << tipo_str << "): "
                  << "lookups=" << cache->obter_total_lookups()
                  << ", hits=" << cache->obter_total_hits()
                  << ", misses=" << cache->obter_total_misses()
                  << ", hit_rate=" << cache->hit_rate() << "%" << std::endl;
    }
}

std::string InlineCacheManager::extrair_nome_metodo(const std::string& nome_completo) {
    // Extrai nome do método de "objeto.metodo"
    size_t pos = nome_completo.find('.');
    if (pos != std::string::npos) {
        return nome_completo.substr(pos + 1);
    }
    return nome_completo;
}

std::string InlineCacheManager::extrair_nome_propriedade(const std::string& nome_completo) {
    // Extrai nome da propriedade de "objeto.propriedade"
    size_t pos = nome_completo.find('.');
    if (pos != std::string::npos) {
        return nome_completo.substr(pos + 1);
    }
    return nome_completo;
}

// ===================== PROPERTY INLINE CACHE =====================

PropertyInlineCache::PropertyInlineCache(const std::string& nome_propriedade)
    : InlineCache(nome_propriedade) {
}

void* PropertyInlineCache::lookup_propriedade(Valor* objeto) {
    return lookup(objeto);
}

void PropertyInlineCache::update_propriedade(Valor* objeto, size_t offset) {
    // Placeholder - na prática armazenaria offset da propriedade
    update(objeto, (void*)offset);
}

// ===================== METHOD INLINE CACHE =====================

MethodInlineCache::MethodInlineCache(const std::string& nome_metodo)
    : InlineCache(nome_metodo) {
}

void* MethodInlineCache::lookup_metodo(Valor* objeto, std::vector<Valor> args) {
    return lookup(objeto);
}

void MethodInlineCache::update_metodo(Valor* objeto, void* codigo) {
    update(objeto, codigo);
}
