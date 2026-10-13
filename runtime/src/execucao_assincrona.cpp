// Implementação do Sistema de Execução Assíncrona para BR v2.0
#include "execucao_assincrona.h"
#include <iostream>
#include <chrono>

// PoolAssincrono
PoolAssincrono::PoolAssincrono(size_t threads) 
    : parar(false), num_threads(threads) {
    
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
    }
    
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] { worker_loop(); });
    }
}

PoolAssincrono::~PoolAssincrono() {
    parar_pool();
}

void PoolAssincrono::worker_loop() {
    while (true) {
        std::function<void()> tarefa;
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] { 
                return parar || !tarefas.empty(); 
            });
            
            if (parar && tarefas.empty()) {
                return;
            }
            
            tarefa = std::move(tarefas.front());
            tarefas.pop();
        }
        
        tarefa();
    }
}

void PoolAssincrono::aguardar_todas() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { 
        return tarefas.empty(); 
    });
}

void PoolAssincrono::parar_pool() {
    {
        std::unique_lock<std::mutex> lock(mutex);
        parar = true;
    }
    
    cv.notify_all();
    
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    workers.clear();
}

// GerenciadorAssincrono
GerenciadorAssincrono::GerenciadorAssincrono() {
    pool = std::make_unique<PoolAssincrono>();
}

GerenciadorAssincrono::~GerenciadorAssincrono() {
    if (pool) {
        pool->parar_pool();
    }
}

EstadoTarefa GerenciadorAssincrono::estado_tarefa(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_tarefas);
    auto it = tarefas_ativas.find(id);
    if (it != tarefas_ativas.end()) {
        // Como não temos tipo, retornamos PENDENTE como fallback
        return PENDENTE;
    }
    return ERRO; // Tarefa não encontrada
}

bool GerenciadorAssincrono::cancelar_tarefa(const std::string& id) {
    std::lock_guard<std::mutex> lock(mutex_tarefas);
    auto it = tarefas_ativas.find(id);
    if (it != tarefas_ativas.end()) {
        tarefas_ativas.erase(it);
        return true;
    }
    return false;
}

void GerenciadorAssincrono::limpar_concluidas() {
    std::lock_guard<std::mutex> lock(mutex_tarefas);
    // Em uma implementação completa, verificaríamos o estado de cada tarefa
    // e removeríamos as concluídas
}

void GerenciadorAssincrono::dormir_assincrono(int milissegundos, std::function<void()> callback) {
    pool->executar([milissegundos, callback]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(milissegundos));
        if (callback) {
            callback();
        }
    });
}

// Funções auxiliares AsyncBR
namespace AsyncBR {
    template<typename F>
    TarefaAssincrono<decltype(std::declval<F>()())> criar_tarefa(F&& funcao) {
        using return_type = decltype(funcao());
        
        auto promessa = std::make_shared<std::promise<return_type>>();
        auto futuro = promessa->get_future();
        
        std::thread([funcao, promessa]() {
            try {
                auto resultado = funcao();
                promessa->set_value(resultado);
            } catch (...) {
                // promessa->set_exception(std::current_exception());
            }
        }).detach();
        
        return TarefaAssincrono<return_type>(promessa);
    }
    
    template<typename T>
    T aguardar_tarefa(TarefaAssincrono<T>& tarefa) {
        return tarefa.aguardar();
    }
    
    void dormir(int milissegundos) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milissegundos));
    }
    
    void executar_apos(int milissegundos, std::function<void()> callback) {
        std::thread([milissegundos, callback]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(milissegundos));
            if (callback) {
                callback();
            }
        }).detach();
    }
    
    void executar_periodicamente(int intervalo_ms, std::function<void()> callback) {
        std::thread([intervalo_ms, callback]() {
            while (true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(intervalo_ms));
                if (callback) {
                    callback();
                }
            }
        }).detach();
    }
}
