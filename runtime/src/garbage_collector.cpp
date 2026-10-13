// Garbage Collector Básico (Mark-Sweep) - Implementação
#include "garbage_collector.h"
#include <iostream>
#include <chrono>
#include <algorithm>

// ===================== GARBAGE COLLECTOR IMPLEMENTATION =====================

GarbageCollector::GarbageCollector() 
    : gc_em_progresso(false), threshold_coleta(1024 * 1024), // 1MB
      memoria_alocada_desde_ultima_coleta(0) {
}

GarbageCollector::~GarbageCollector() {
    // Coleta final antes de destruir
    coletar();
    
    // Libera todos os objetos restantes
    for (ObjetoGC* obj : heap) {
        liberar_objeto(obj);
    }
    
    heap.clear();
    root_set.clear();
}

// ===================== ALOCAÇÃO =====================

ObjetoGC* GarbageCollector::alocar_objeto(TipoObjetoGC tipo, size_t tamanho) {
    std::lock_guard<std::mutex> lock(mutex_gc);
    
    // Verifica se precisa coletar
    memoria_alocada_desde_ultima_coleta += tamanho;
    if (memoria_alocada_desde_ultima_coleta >= threshold_coleta) {
        coletar();
    }
    
    // Aloca objeto
    ObjetoGC* obj = nullptr;
    
    switch (tipo) {
        case OBJ_VALOR:
            obj = new ObjetoValor(nullptr);
            break;
        default:
            obj = new ObjetoGC(tipo, tamanho);
            break;
    }
    
    if (tamanho > 0) {
        obj->dados = malloc(tamanho);
        if (!obj->dados) {
            delete obj;
            return nullptr;
        }
    }
    
    heap.push_back(obj);
    estatisticas.objetos_totais++;
    estatisticas.memoria_alocada += tamanho;
    
    return obj;
}

ObjetoGC* GarbageCollector::alocar_valor(Valor* valor) {
    std::lock_guard<std::mutex> lock(mutex_gc);
    
    ObjetoValor* obj = new ObjetoValor(valor);
    heap.push_back(obj);
    
    estatisticas.objetos_totais++;
    estatisticas.memoria_alocada += sizeof(Valor);
    memoria_alocada_desde_ultima_coleta += sizeof(Valor);
    
    return obj;
}

// ===================== GERENCIAMENTO DE ROOTS =====================

void GarbageCollector::adicionar_root(ObjetoGC* obj) {
    std::lock_guard<std::mutex> lock(mutex_gc);
    
    if (esta_no_heap(obj)) {
        root_set.insert(obj);
    }
}

void GarbageCollector::remover_root(ObjetoGC* obj) {
    std::lock_guard<std::mutex> lock(mutex_gc);
    
    root_set.erase(obj);
}

// ===================== FINALIZADORES =====================

void GarbageCollector::registrar_finalizador(ObjetoGC* obj, std::function<void()> fn) {
    std::lock_guard<std::mutex> lock(mutex_gc);
    
    if (esta_no_heap(obj)) {
        finalizadores[obj] = fn;
    }
}

// ===================== COLEÇÃO =====================

void GarbageCollector::coletar() {
    if (gc_em_progresso.load()) {
        return; // Evita coleta recursiva
    }
    
    gc_em_progresso.store(true);
    auto inicio = std::chrono::high_resolution_clock::now();
    
    std::cout << "[GC] Iniciando coleta..." << std::endl;
    
    // Fase 1: Mark
    marcar();
    
    // Fase 2: Sweep
    varrer();
    
    // Reset
    memoria_alocada_desde_ultima_coleta = 0;
    
    auto fim = std::chrono::high_resolution_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
    
    estatisticas.colecoes_executadas++;
    estatisticas.tempo_total_ms += duracao.count();
    
    std::cout << "[GC] Coleta concluída em " << duracao.count() << "ms" << std::endl;
    std::cout << "[GC] Objetos restantes: " << heap.size() << std::endl;
    
    gc_em_progresso.store(false);
}

void GarbageCollector::coletar_se_necessario() {
    if (memoria_alocada_desde_ultima_coleta >= threshold_coleta) {
        coletar();
    }
}

// ===================== MARK-SWEEP =====================

void GarbageCollector::marcar() {
    // Desmarca todos os objetos
    for (ObjetoGC* obj : heap) {
        obj->marcado = false;
    }
    
    // Marca roots e seus descendentes
    for (ObjetoGC* root : root_set) {
        marcar_objeto(root);
    }
}

void GarbageCollector::varrer() {
    std::vector<ObjetoGC*> objetos_para_liberar;
    
    // Identifica objetos não marcados
    for (ObjetoGC* obj : heap) {
        if (!obj->marcado) {
            objetos_para_liberar.push_back(obj);
        }
    }
    
    // Executa finalizadores
    executar_finalizadores(objetos_para_liberar);
    
    // Libera objetos não marcados
    for (ObjetoGC* obj : objetos_para_liberar) {
        liberar_objeto(obj);
    }
    
    // Remove do heap
    auto it = std::remove_if(heap.begin(), heap.end(), 
        [](ObjetoGC* obj) { return !obj->marcado; });
    heap.erase(it, heap.end());
}

// ===================== TRACING =====================

void GarbageCollector::marcar_objeto(ObjetoGC* obj) {
    if (!obj || obj->marcado) {
        return;
    }
    
    obj->marcado = true;
    
    // Marca referências (tracing)
    marcar_referencias(obj);
}

void GarbageCollector::marcar_referencias(ObjetoGC* obj) {
    // Marca todos os objetos referenciados
    for (ObjetoGC* ref : obj->referencias) {
        marcar_objeto(ref);
    }
}

// ===================== ESTATÍSTICAS =====================

EstatisticasGC GarbageCollector::obter_estatisticas() {
    std::lock_guard<std::mutex> lock(mutex_gc);
    return estatisticas;
}

size_t GarbageCollector::obter_contagem_objetos() {
    std::lock_guard<std::mutex> lock(mutex_gc);
    return heap.size();
}

size_t GarbageCollector::obter_memoria_usada() {
    std::lock_guard<std::mutex> lock(mutex_gc);
    
    size_t total = 0;
    for (ObjetoGC* obj : heap) {
        total += obj->tamanho;
    }
    
    return total;
}

// ===================== CONFIGURAÇÃO =====================

void GarbageCollector::definir_threshold(size_t bytes) {
    std::lock_guard<std::mutex> lock(mutex_gc);
    threshold_coleta = bytes;
}

void GarbageCollector::resetar_estatisticas() {
    std::lock_guard<std::mutex> lock(mutex_gc);
    estatisticas = EstatisticasGC();
}

// ===================== UTILITÁRIOS =====================

bool GarbageCollector::esta_no_heap(ObjetoGC* obj) {
    auto it = std::find(heap.begin(), heap.end(), obj);
    return it != heap.end();
}

void GarbageCollector::liberar_objeto(ObjetoGC* obj) {
    if (!obj) return;
    
    estatisticas.memoria_liberada += obj->tamanho;
    
    // Remove finalizadores
    finalizadores.erase(obj);
    
    delete obj;
}

void GarbageCollector::executar_finalizadores(std::vector<ObjetoGC*> objetos_para_liberar) {
    for (ObjetoGC* obj : objetos_para_liberar) {
        auto it = finalizadores.find(obj);
        if (it != finalizadores.end()) {
            try {
                it->second();
            } catch (const std::exception& e) {
                std::cerr << "[GC] Erro no finalizador: " << e.what() << std::endl;
            }
        }
    }
}
