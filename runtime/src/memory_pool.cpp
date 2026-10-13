// Memory Pool Simples (Free List) - Implementação
#include "memory_pool.h"
#include <iostream>
#include <stdexcept>

// Thread-local pool
thread_local MemoryPool* MemoryPool::local_pool = nullptr;

// ===================== FREE LIST IMPLEMENTATION =====================

FreeList::FreeList(size_t tamanho) 
    : cabeca(nullptr), tamanho_bloco(tamanho), 
      blocos_alocados(0), blocos_livres(0) {
    expandir(16); // Começa com 16 blocos
}

FreeList::~FreeList() {
    // Libera todos os blocos
    FreeListNode* atual = cabeca;
    while (atual) {
        FreeListNode* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}

void* FreeList::alocar() {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (!cabeca) {
        expandir(16); // Expande se não há blocos livres
    }
    
    if (!cabeca) {
        return nullptr; // Falha na alocação
    }
    
    FreeListNode* nodo = cabeca;
    cabeca = cabeca->proximo;
    
    blocos_alocados++;
    blocos_livres--;
    
    // Retorna o ponteiro para o bloco (após o header)
    return reinterpret_cast<uint8_t*>(nodo) + sizeof(FreeListNode);
}

void FreeList::liberar(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    // Calcula o endereço do nodo
    FreeListNode* nodo = reinterpret_cast<FreeListNode*>(
        reinterpret_cast<uint8_t*>(ptr) - sizeof(FreeListNode)
    );
    
    // Adiciona de volta à free list
    nodo->proximo = cabeca;
    cabeca = nodo;
    
    blocos_alocados--;
    blocos_livres++;
}

void FreeList::expandir(size_t quantidade) {
    for (size_t i = 0; i < quantidade; i++) {
        // Aloca bloco com espaço para o header
        size_t tamanho_total = tamanho_bloco + sizeof(FreeListNode);
        FreeListNode* nodo = static_cast<FreeListNode*>(malloc(tamanho_total));
        
        if (!nodo) {
            std::cerr << "[MemoryPool] Falha ao expandir free list" << std::endl;
            return;
        }
        
        nodo->tamanho = tamanho_bloco;
        nodo->proximo = cabeca;
        cabeca = nodo;
        
        blocos_livres++;
    }
}

void FreeList::reduzir() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Libera blocos excedentes (mantém alguns para reuso)
    size_t manter = blocos_livres / 2;
    
    while (blocos_livres > manter && cabeca) {
        FreeListNode* nodo = cabeca;
        cabeca = cabeca->proximo;
        free(nodo);
        blocos_livres--;
    }
}

// ===================== ARENA ALLOCATOR IMPLEMENTATION =====================

ArenaAllocator::ArenaAllocator(size_t tamanho_inicial) 
    : offset(0), tamanho_total(tamanho_inicial) {
    buffer.resize(tamanho_inicial);
}

ArenaAllocator::~ArenaAllocator() {
    buffer.clear();
}

void* ArenaAllocator::alocar(size_t tamanho, size_t alinhamento) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Calcula offset alinhado
    size_t offset_alinhado = (offset + alinhamento - 1) & ~(alinhamento - 1);
    
    // Verifica se precisa expandir
    if (offset_alinhado + tamanho > buffer.size()) {
        size_t novo_tamanho = buffer.size() * 2;
        while (offset_alinhado + tamanho > novo_tamanho) {
            novo_tamanho *= 2;
        }
        buffer.resize(novo_tamanho);
        tamanho_total = novo_tamanho;
    }
    
    void* ptr = &buffer[offset_alinhado];
    offset = offset_alinhado + tamanho;
    
    return ptr;
}

void* ArenaAllocator::alocar_array(size_t tamanho_elemento, size_t quantidade) {
    return alocar(tamanho_elemento * quantidade, alignof(std::max_align_t));
}

void ArenaAllocator::reset() {
    std::lock_guard<std::mutex> lock(mutex);
    offset = 0;
    checkpoints.clear();
}

size_t ArenaAllocator::criar_checkpoint() {
    std::lock_guard<std::mutex> lock(mutex);
    checkpoints.push_back(offset);
    return checkpoints.size() - 1;
}

void ArenaAllocator::restaurar_checkpoint(size_t checkpoint) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (checkpoint < checkpoints.size()) {
        offset = checkpoints[checkpoint];
        checkpoints.resize(checkpoint);
    }
}

// ===================== MEMORY POOL IMPLEMENTATION =====================

MemoryPool::MemoryPool() 
    : arena(new ArenaAllocator(1024 * 1024)), // 1MB arena
      total_alocado(0), total_liberado(0), total_falhas(0) {
    inicializar_pools_padrao();
}

MemoryPool::~MemoryPool() {
    // Libera todas as free lists
    for (auto& [tamanho, free_list] : free_lists) {
        delete free_list;
    }
    free_lists.clear();
    
    delete arena;
}

void* MemoryPool::alocar_rapido(size_t tamanho) {
    // Arredonda para tamanho suportado
    size_t tamanho_arredondado = arredondar_tamanho(tamanho);
    
    if (tamanho_suportado(tamanho_arredondado)) {
        FreeList* free_list = obter_ou_criar_free_list(tamanho_arredondado);
        void* ptr = free_list->alocar();
        
        if (ptr) {
            std::lock_guard<std::mutex> lock(mutex_estatisticas);
            total_alocado += tamanho;
            return ptr;
        }
    }
    
    // Fallback para malloc se não suportado ou falha
    std::lock_guard<std::mutex> lock(mutex_estatisticas);
    total_falhas++;
    return malloc(tamanho);
}

void MemoryPool::liberar_rapido(void* ptr, size_t tamanho) {
    if (!ptr) return;
    
    size_t tamanho_arredondado = arredondar_tamanho(tamanho);
    
    if (tamanho_suportado(tamanho_arredondado)) {
        auto it = free_lists.find(tamanho_arredondado);
        if (it != free_lists.end()) {
            it->second->liberar(ptr);
            std::lock_guard<std::mutex> lock(mutex_estatisticas);
            total_liberado += tamanho;
            return;
        }
    }
    
    // Fallback para free
    free(ptr);
    std::lock_guard<std::mutex> lock(mutex_estatisticas);
    total_liberado += tamanho;
}

template<typename T>
T* MemoryPool::alocar_objeto() {
    void* ptr = alocar_rapido(sizeof(T));
    if (!ptr) return nullptr;
    
    return new (ptr) T();
}

template<typename T>
void MemoryPool::liberar_objeto(T* obj) {
    if (!obj) return;
    
    obj->~T();
    liberar_rapido(obj, sizeof(T));
}

void* MemoryPool::alocar_arena(size_t tamanho) {
    return arena->alocar(tamanho);
}

void MemoryPool::resetar_arena() {
    arena->reset();
}

size_t MemoryPool::criar_checkpoint_arena() {
    return arena->criar_checkpoint();
}

void MemoryPool::restaurar_checkpoint_arena(size_t checkpoint) {
    arena->restaurar_checkpoint(checkpoint);
}

MemoryPool* MemoryPool::obter_local() {
    if (!local_pool) {
        local_pool = new MemoryPool();
    }
    return local_pool;
}

void MemoryPool::definir_local(MemoryPool* pool) {
    local_pool = pool;
}

size_t MemoryPool::obter_total_alocado() const {
    return total_alocado;
}

size_t MemoryPool::obter_total_liberado() const {
    return total_liberado;
}

size_t MemoryPool::obter_total_falhas() const {
    return total_falhas;
}

double MemoryPool::obter_taxa_falha() const {
    size_t total = total_alocado + total_falhas;
    if (total == 0) return 0.0;
    return (double)total_falhas / total * 100.0;
}

void MemoryPool::inicializar_pools_padrao() {
    free_lists[POOL_16] = new FreeList(POOL_16);
    free_lists[POOL_32] = new FreeList(POOL_32);
    free_lists[POOL_64] = new FreeList(POOL_64);
    free_lists[POOL_128] = new FreeList(POOL_128);
    free_lists[POOL_256] = new FreeList(POOL_256);
    free_lists[POOL_512] = new FreeList(POOL_512);
    free_lists[POOL_1024] = new FreeList(POOL_1024);
    free_lists[POOL_2048] = new FreeList(POOL_2048);
    free_lists[POOL_4096] = new FreeList(POOL_4096);
}

FreeList* MemoryPool::obter_ou_criar_free_list(size_t tamanho) {
    auto it = free_lists.find(tamanho);
    if (it != free_lists.end()) {
        return it->second;
    }
    
    // Cria nova free list se não existir
    FreeList* free_list = new FreeList(tamanho);
    free_lists[tamanho] = free_list;
    return free_list;
}

size_t MemoryPool::arredondar_tamanho(size_t tamanho) {
    // Arredonda para o próximo tamanho de potência de 2
    if (tamanho <= 16) return 16;
    if (tamanho <= 32) return 32;
    if (tamanho <= 64) return 64;
    if (tamanho <= 128) return 128;
    if (tamanho <= 256) return 256;
    if (tamanho <= 512) return 512;
    if (tamanho <= 1024) return 1024;
    if (tamanho <= 2048) return 2048;
    if (tamanho <= 4096) return 4096;
    
    // Para tamanhos maiores, arredonda para múltiplo de 4096
    return ((tamanho + 4095) / 4096) * 4096;
}

bool MemoryPool::tamanho_suportado(size_t tamanho) {
    return tamanho <= 4096;
}

// Instanciações explícitas de templates
template Valor* MemoryPool::alocar_objeto<Valor>();
template void MemoryPool::liberar_objeto<Valor>(Valor*);
