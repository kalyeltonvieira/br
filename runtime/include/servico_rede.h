// Serviço de Rede Nativo BR v2.0
#ifndef SERVICO_REDE_H
#define SERVICO_REDE_H

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <functional>
#include <memory>

struct RespostaHTTP {
    int codigo;
    std::string conteudo;
    std::map<std::string, std::string> cabecalhos;
    bool falhou;
    std::string erro;
    
    RespostaHTTP() : codigo(0), falhou(true) {}
};

// Estrutura para requisição HTTP recebida pelo servidor
struct RequisicaoHTTP {
    std::string metodo;
    std::string caminho;
    std::string protocolo;
    std::string corpo;
    std::map<std::string, std::string> cabecalhos;
    std::map<std::string, std::string> parametros;  // Parâmetros de rota (:id)
    std::map<std::string, std::string> query;       // Parâmetros de query string (?nome=valor)
    
    RequisicaoHTTP() {}
};

// Estrutura para resposta HTTP do servidor
struct RespostaServidor {
    int codigo;
    std::string conteudo;
    std::string tipo_conteudo;
    std::map<std::string, std::string> cabecalhos;
    
    RespostaServidor() : codigo(200), tipo_conteudo("text/html; charset=utf-8") {}
    
    static RespostaServidor ok(const std::string& corpo, const std::string& tipo = "text/html; charset=utf-8") {
        RespostaServidor r; r.codigo = 200; r.conteudo = corpo; r.tipo_conteudo = tipo; return r;
    }
    
    static RespostaServidor json(const std::string& corpo) {
        RespostaServidor r; r.codigo = 200; r.conteudo = corpo; r.tipo_conteudo = "application/json"; return r;
    }
    
    static RespostaServidor erro(int codigo, const std::string& mensagem) {
        RespostaServidor r; r.codigo = codigo; r.conteudo = mensagem; return r;
    }
    
    static RespostaServidor naoEncontrado() {
        return erro(404, "<h1>404 - Página não encontrada</h1>");
    }
    
    static RespostaServidor erroInterno() {
        return erro(500, "<h1>500 - Erro interno do servidor</h1>");
    }
};

// Handler de rota: recebe requisição, retorna resposta
typedef std::function<RespostaServidor(const RequisicaoHTTP&)> HandlerHTTP;

// Middleware: processa antes do handler
typedef std::function<bool(const RequisicaoHTTP&, RespostaServidor&)> MiddlewareHTTP;

class ServicoRede {
public:
    // HTTP Client
    static RespostaHTTP http_get(const std::string& url);
    static RespostaHTTP http_post(const std::string& url, const std::string& dados);
    static RespostaHTTP http_put(const std::string& url, const std::string& dados);
    static RespostaHTTP http_delete(const std::string& url);
    static RespostaHTTP http_patch(const std::string& url, const std::string& dados);
    
    static RespostaHTTP http_request(const std::string& metodo, const std::string& url, 
                                     const std::string& dados = "", 
                                     const std::map<std::string, std::string>& cabecalhos = {});
    
    // Validação de URL
    static bool url_valida(const std::string& url);
    
    // Parse de URL
    static std::map<std::string, std::string> url_parsear(const std::string& url);
    
    // Codificação URL
    static std::string url_codificar(const std::string& texto);
    static std::string url_decodificar(const std::string& texto);
    
    // Servidor HTTP completo
    static int criar_servidor(int porta);
    static bool adicionar_rota(int servidor_id, const std::string& metodo, const std::string& caminho, HandlerHTTP handler);
    static bool adicionar_middleware(int servidor_id, MiddlewareHTTP middleware);
    static bool iniciar_servidor(int servidor_id);
    static bool parar_servidor(int servidor_id);
    static bool servidor_rodando(int servidor_id);
    static void parar_todos_servidores();
    
    // DNS
    static std::vector<std::string> dns_resolver(const std::string& hostname);
    static std::string dns_reverso(const std::string& ip);
    
    // Utilitários de rede
    static std::string ip_local();
    static std::string ip_publico();
    static bool verificar_conectividade();
    
    // Ping (simplificado)
    static bool ping(const std::string& host, int timeout_ms = 1000);
    
private:
    static RespostaHTTP fazer_requisicao_http(const std::string& metodo, const std::string& url,
                                               const std::string& dados,
                                               const std::map<std::string, std::string>& cabecalhos);
    
    static std::string extrair_host_porta(const std::string& url, int& porta);
    static std::string construir_cabecalhos_http(const std::map<std::string, std::string>& cabecalhos);
    static std::map<std::string, std::string> parsear_cabecalhos_http(const std::string& cabecalhos);
    
    static bool conectar_socket(const std::string& host, int porta, int& socket_fd);
    static bool enviar_socket(int socket_fd, const std::string& dados);
    static std::string receber_socket(int socket_fd, int timeout_ms);
    static void fechar_socket(int socket_fd);
    
public:
    // Utilitários de socket (públicos para uso interno do servidor)
    static bool enviar_socket_publico(int socket_fd, const std::string& dados) {
        return enviar_socket(socket_fd, dados);
    }
    
    // Cache de DNS simples
    static std::map<std::string, std::vector<std::string>> cache_dns;
    static std::mutex cache_mutex;
};

#endif // SERVICO_REDE_H