// Garbage Collector Básico (Mark-Sweep) para Linguagem BR
// Implementação simples com mark-sweep tracing
#ifndef GARBAGE_COLLECTOR_H
#define GARBAGE_COLLECTOR_H

#include <vector>
#include <map>
#include <set>
#include <mutex>
#include <atomic>
#include <functional>
#include <cstdint>

// Forward declarations
struct Valor;

// Tipos de objetos gerenciados
enum TipoObjetoGC {
    OBJ_VALOR,
    OBJ_LISTA,
    OBJ_REGISTRO,
    OBJ_FUNCAO,
    OBJ_CLASSE,
    OBJ_STRING
};

// Objeto gerenciado pelo GC
struct ObjetoGC {
    TipoObjetoGC tipo;
    bool marcado;
    size_t tamanho;
    void* dados;
    std::set<ObjetoGC*> referencias;
    
    ObjetoGC(TipoObjetoGC t, size_t tam) 
        : tipo(t), marcado(false), tamanho(tam), dados(nullptr) {}
    
    virtual ~ObjetoGC() {
        if (dados) {
            free(dados);
        }
    }
};

// Wrapper específico para Valor
struct ObjetoValor : ObjetoGC {
    Valor* valor;
    
    ObjetoValor(Valor* v) : ObjetoGC(OBJ_VALOR, sizeof(Valor)), valor(v) {
        dados = valor;
    }
};

// Estatísticas do GC
struct EstatisticasGC {
    size_t objetos_totais;
    size_t memoria_alocada;
    size_t memoria_liberada;
    size_t colecoes_executadas;
    size_t tempo_total_ms;
    
    EstatisticasGC() 
        : objetos_totais(0), memoria_alocada(0), 
          memoria_liberada(0), colecoes_executadas(0), 
          tempo_total_ms(0) {}
};

// Garbage Collector Principal
class GarbageCollector {
private:
    // Heap de objetos
    std::vector<ObjetoGC*> heap;
    
    // Root set (objetos acessíveis globalmente)
    std::set<ObjetoGC*> root_set;
    
    // Finalizadores
    std::map<ObjetoGC*, std::function<void()>> finalizadores;
    
    // Thread safety
    std::mutex mutex_gc;
    std::atomic<bool> gc_em_progresso;
    
    // Estatísticas
    EstatisticasGC estatisticas;
    
    // Configurações
    size_t threshold_coleta; // Coletar quando alocar X bytes
    size_t memoria_alocada_desde_ultima_coleta;
    
public:
    GarbageCollector();
    ~GarbageCollector();
    
    // Alocação
    ObjetoGC* alocar_objeto(TipoObjetoGC tipo, size_t tamanho);
    ObjetoGC* alocar_valor(Valor* valor);
    
    // Gerenciamento de roots
    void adicionar_root(ObjetoGC* obj);
    void remover_root(ObjetoGC* obj);
    
    // Finalizadores
    void registrar_finalizador(ObjetoGC* obj, std::function<void()> fn);
    
    // Coleta
    void coletar();
    void coletar_se_necessario();
    
    // Mark-Sweep
    void marcar();
    void varrer();
    
    // Tracing
    void marcar_objeto(ObjetoGC* obj);
    void marcar_referencias(ObjetoGC* obj);
    
    // Estatísticas
    EstatisticasGC obter_estatisticas();
    size_t obter_contagem_objetos();
    size_t obter_memoria_usada();
    
    // Configuração
    void definir_threshold(size_t bytes);
    void resetar_estatisticas();
    
private:
    // Utilitários
    bool esta_no_heap(ObjetoGC* obj);
    void liberar_objeto(ObjetoGC* obj);
    void executar_finalizadores(std::vector<ObjetoGC*> objetos_para_liberar);
};

#endif // GARBAGE_COLLECTOR_H
