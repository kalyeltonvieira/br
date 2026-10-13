// Memory Pool Simples (Free List) para Linguagem BR
// Implementação com free lists para alocação rápida
#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <vector>
#include <map>
#include <mutex>
#include <cstdint>
#include <cstring>

// Tamanhos de pool pré-definidos
enum TamanhoPool {
    POOL_16 = 16,
    POOL_32 = 32,
    POOL_64 = 64,
    POOL_128 = 128,
    POOL_256 = 256,
    POOL_512 = 512,
    POOL_1024 = 1024,
    POOL_2048 = 2048,
    POOL_4096 = 4096
};

// Nó da free list
struct FreeListNode {
    FreeListNode* proximo;
    size_t tamanho;
    
    FreeListNode(size_t tam) : proximo(nullptr), tamanho(tam) {}
};

// Free List para um tamanho específico
class FreeList {
private:
    FreeListNode* cabeca;
    size_t tamanho_bloco;
    size_t blocos_alocados;
    size_t blocos_livres;
    std::mutex mutex;
    
public:
    FreeList(size_t tamanho);
    ~FreeList();
    
    void* alocar();
    void liberar(void* ptr);
    
    size_t obter_tamanho_bloco() const { return tamanho_bloco; }
    size_t obter_blocos_alocados() const { return blocos_alocados; }
    size_t obter_blocos_livres() const { return blocos_livres; }
    
    void expandir(size_t quantidade);
    void reduzir();
};

// Arena Allocator para alocações temporárias
class ArenaAllocator {
private:
    std::vector<uint8_t> buffer;
    size_t offset;
    size_t tamanho_total;
    std::vector<size_t> checkpoints;
    std::mutex mutex;
    
public:
    ArenaAllocator(size_t tamanho_inicial = 1024 * 1024); // 1MB padrão
    ~ArenaAllocator();
    
    void* alocar(size_t tamanho, size_t alinhamento = 8);
    void* alocar_array(size_t tamanho_elemento, size_t quantidade);
    
    void reset();
    size_t criar_checkpoint();
    void restaurar_checkpoint(size_t checkpoint);
    
    size_t obter_usado() const { return offset; }
    size_t obter_livre() const { return tamanho_total - offset; }
    size_t obter_tamanho_total() const { return tamanho_total; }
};

// Memory Pool Principal
class MemoryPool {
private:
    // Free lists por tamanho
    std::map<size_t, FreeList*> free_lists;
    
    // Arena allocator
    ArenaAllocator* arena;
    
    // Thread-local pools
    thread_local static MemoryPool* local_pool;
    
    // Estatísticas
    size_t total_alocado;
    size_t total_liberado;
    size_t total_falhas;
    std::mutex mutex_estatisticas;
    
public:
    MemoryPool();
    ~MemoryPool();
    
    // Alocação rápida
    void* alocar_rapido(size_t tamanho);
    void liberar_rapido(void* ptr, size_t tamanho);
    
    // Alocação de objetos
    template<typename T>
    T* alocar_objeto();
    
    template<typename T>
    void liberar_objeto(T* obj);
    
    // Arena allocation
    void* alocar_arena(size_t tamanho);
    void resetar_arena();
    size_t criar_checkpoint_arena();
    void restaurar_checkpoint_arena(size_t checkpoint);
    
    // Thread-local pool
    static MemoryPool* obter_local();
    static void definir_local(MemoryPool* pool);
    
    // Estatísticas
    size_t obter_total_alocado() const;
    size_t obter_total_liberado() const;
    size_t obter_total_falhas() const;
    double obter_taxa_falha() const;
    
    // Configuração
    void inicializar_pools_padrao();
    
private:
    FreeList* obter_ou_criar_free_list(size_t tamanho);
    size_t arredondar_tamanho(size_t tamanho);
    bool tamanho_suportado(size_t tamanho);
};

#endif // MEMORY_POOL_H
