// Sistema de Execução Assíncrona para BR v2.0
// Implementa async/await (assincrono, aguardar)
#ifndef EXECUCAO_ASSINCRONA_H
#define EXECUCAO_ASSINCRONA_H

#include <string>
#include <vector>
#include <future>
#include <memory>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <atomic>

// Estado de uma tarefa assíncrona
enum EstadoTarefa {
    PENDENTE,
    EXECUTANDO,
    CONCLUIDA,
    CANCELADA,
    ERRO
};

// Resultado de uma tarefa assíncrona
template<typename T>
struct ResultadoAssincrono {
    T valor;
    std::string erro;
    EstadoTarefa estado;
    
    ResultadoAssincrono() : estado(PENDENTE) {}
    ResultadoAssincrono(T v) : valor(v), estado(CONCLUIDA) {}
    ResultadoAssincrono(const std::string& e) : erro(e), estado(ERRO) {}
    
    bool pronto() const { return estado == CONCLUIDA || estado == ERRO; }
    bool sucesso() const { return estado == CONCLUIDA; }
};

// Tarefa assíncrona genérica
template<typename T>
class TarefaAssincrona {
private:
    std::future<T> futuro;
    std::shared_ptr<std::promise<T>> promessa;
    EstadoTarefa estado;
    std::mutex mutex;
    std::condition_variable cv;
    
public:
    TarefaAssincrona() : estado(PENDENTE) {}
    
    TarefaAssincrona(std::future<T> f) 
        : futuro(std::move(f)), estado(EXECUTANDO) {}
    
    TarefaAssincrona(std::shared_ptr<std::promise<T>> p)
        : promessa(p), estado(PENDENTE) {}
    
    // Aguardar conclusão
    T aguardar() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { 
            return estado == CONCLUIDA || estado == ERRO; 
        });
        
        if (estado == CONCLUIDA) {
            return valor_obtido();
        }
        throw std::runtime_error("Tarefa terminou com erro");
    }
    
    // Aguardar com timeout
    bool aguardar_por(int milissegundos) {
        std::unique_lock<std::mutex> lock(mutex);
        if (cv.wait_for(lock, std::chrono::milliseconds(milissegundos),
                       [this] { return estado == CONCLUIDA || estado == ERRO; })) {
            return true;
        }
        return false;
    }
    
    // Obter valor (bloqueia se não pronto)
    T valor_obtido() {
        if (futuro.valid()) {
            estado = CONCLUIDA;
            cv.notify_all();
            return futuro.get();
        }
        throw std::runtime_error("Tarefa não tem resultado válido");
    }
    
    // Definir valor (para tarefas manuais)
    void definir_valor(T valor) {
        std::lock_guard<std::mutex> lock(mutex);
        if (promessa) {
            promessa->set_value(valor);
        }
        estado = CONCLUIDA;
        cv.notify_all();
    }
    
    // Definir erro
    void definir_erro(const std::string& erro) {
        std::lock_guard<std::mutex> lock(mutex);
        if (promessa) {
            // promessa->set_exception(std::make_exception_ptr(...));
        }
        estado = ERRO;
        cv.notify_all();
    }
    
    // Verificar estado
    EstadoTarefa obter_estado() const { return estado; }
    bool esta_pronto() const { return estado == CONCLUIDA || estado == ERRO; }
    
    // Cancelar
    void cancelar() {
        std::lock_guard<std::mutex> lock(mutex);
        estado = CANCELADA;
        cv.notify_all();
    }
};

// Pool de threads para execução assíncrona
class PoolAssincrono {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tarefas;
    std::mutex mutex;
    std::condition_variable cv;
    std::atomic<bool> parar;
    size_t num_threads;
    
public:
    PoolAssincrono(size_t threads = std::thread::hardware_concurrency());
    ~PoolAssincrono();
    
    // Executar tarefa assíncrona
    template<typename F, typename... Args>
    auto executar(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type> {
        
        using return_type = typename std::result_of<F(Args...)>::type;
        
        auto tarefa = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> resultado = tarefa->get_future();
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (parar) {
                throw std::runtime_error("Pool está parando");
            }
            tarefas.emplace([tarefa]() { (*tarefa)(); });
        }
        
        cv.notify_one();
        return resultado;
    }
    
    // Aguardar todas as tarefas concluírem
    void aguardar_todas();
    
    // Parar pool
    void parar_pool();
    
private:
    void worker_loop();
};

// Gerenciador de execução assíncrona
class GerenciadorAssincrono {
private:
    std::unique_ptr<PoolAssincrono> pool;
    std::map<std::string, std::shared_ptr<void>> tarefas_ativas;
    std::mutex mutex_tarefas;
    
public:
    GerenciadorAssincrono();
    ~GerenciadorAssincrono();
    
    // Criar tarefa assíncrona
    template<typename F>
    auto assincrono(F&& funcao, const std::string& id = "")
        -> TarefaAssincrona<decltype(funcao())> {
        
        using return_type = decltype(funcao());
        
        auto futuro = pool->executar(std::forward<F>(funcao));
        
        TarefaAssincrono<return_type> tarefa(std::move(futuro));
        
        if (!id.empty()) {
            std::lock_guard<std::mutex> lock(mutex_tarefas);
            tarefas_ativas[id] = std::make_shared<TarefaAssincrono<return_type>>(tarefa);
        }
        
        return tarefa;
    }
    
    // Aguardar tarefa por ID
    template<typename T>
    T aguardar(const std::string& id) {
        std::lock_guard<std::mutex> lock(mutex_tarefas);
        auto it = tarefas_ativas.find(id);
        if (it != tarefas_ativas.end()) {
            auto tarefa = std::static_pointer_cast<TarefaAssincrona<T>>(it->second);
            return tarefa->aguardar();
        }
        throw std::runtime_error("Tarefa não encontrada: " + id);
    }
    
    // Aguardar múltiplas tarefas
    template<typename T>
    std::vector<T> aguardar_todas(const std::vector<std::string>& ids) {
        std::vector<T> resultados;
        for (const auto& id : ids) {
            resultados.push_back(aguardar<T>(id));
        }
        return resultados;
    }
    
    // Aguardar qualquer tarefa
    template<typename T>
    T aguardar_qualquer(const std::vector<std::string>& ids) {
        // Implementação simplificada - aguarda a primeira
        for (const auto& id : ids) {
            try {
                return aguardar<T>(id);
            } catch (...) {
                continue;
            }
        }
        throw std::runtime_error("Nenhuma tarefa concluída");
    }
    
    // Verificar estado da tarefa
    EstadoTarefa estado_tarefa(const std::string& id);
    
    // Cancelar tarefa
    bool cancelar_tarefa(const std::string& id);
    
    // Limpar tarefas concluídas
    void limpar_concluidas();
    
    // Sleep assíncrono
    void dormir_assincrono(int milissegundos, std::function<void()> callback);
};

// Funções auxiliares para bytecode
namespace AsyncBR {
    // Criar tarefa assíncrona de função
    template<typename F>
    TarefaAssincrono<decltype(std::declval<F>()())> criar_tarefa(F&& funcao);
    
    // Aguardar tarefa
    template<typename T>
    T aguardar_tarefa(TarefaAssincrono<T>& tarefa);
    
    // Sleep não-bloqueante
    void dormir(int milissegundos);
    
    // Executar após delay
    void executar_apos(int milissegundos, std::function<void()> callback);
    
    // Executar periodicamente
    void executar_periodicamente(int intervalo_ms, std::function<void()> callback);
}

#endif // EXECUCAO_ASSINCRONA_H
