// Implementação do Serviço de Rede Nativo BR
#include "servico_rede.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <regex>
#include <thread>
#include <chrono>
#include <atomic>
#include <unordered_map>
#include <memory>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#endif

// Cache DNS
std::map<std::string, std::vector<std::string>> ServicoRede::cache_dns;
std::mutex ServicoRede::cache_mutex;

namespace {
    bool inicializado_winsock = false;

#ifdef _WIN32
    bool iniciar_winsock() {
        if (!inicializado_winsock) {
            WSADATA wsa;
            if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
                return false;
            }
            inicializado_winsock = true;
        }
        return true;
    }
#endif

    void fechar_socket_plataforma(int fd) {
#ifdef _WIN32
        closesocket(fd);
#else
        close(fd);
#endif
    }
}

// ============ HTTP CLIENT ============

RespostaHTTP ServicoRede::http_get(const std::string& url) {
    return fazer_requisicao_http("GET", url, "", {});
}

RespostaHTTP ServicoRede::http_post(const std::string& url, const std::string& dados) {
    return fazer_requisicao_http("POST", url, dados, {});
}

RespostaHTTP ServicoRede::http_put(const std::string& url, const std::string& dados) {
    return fazer_requisicao_http("PUT", url, dados, {});
}

RespostaHTTP ServicoRede::http_delete(const std::string& url) {
    return fazer_requisicao_http("DELETE", url, "", {});
}

RespostaHTTP ServicoRede::http_patch(const std::string& url, const std::string& dados) {
    return fazer_requisicao_http("PATCH", url, dados, {});
}

RespostaHTTP ServicoRede::http_request(const std::string& metodo, const std::string& url,
                                        const std::string& dados,
                                        const std::map<std::string, std::string>& cabecalhos) {
    return fazer_requisicao_http(metodo, url, dados, cabecalhos);
}

// ============ VALIDAÇÃO DE URL ============

bool ServicoRede::url_valida(const std::string& url) {
    // Regex simples para validar URL
    std::regex url_regex(
        R"(^(https?:\/\/)?([\da-z\.-]+)\.([a-z\.]{2,6})([\/\w \.-]*)*\/?$)",
        std::regex::icase
    );
    return std::regex_match(url, url_regex);
}

std::map<std::string, std::string> ServicoRede::url_parsear(const std::string& url) {
    std::map<std::string, std::string> partes;
    
    std::regex url_regex(
        R"(^((https?)://)?([^:/?#]+)(:(\d+))?([^?#]*)(\?([^#]*))?(#(.*))?)",
        std::regex::icase
    );
    
    std::smatch match;
    if (std::regex_match(url, match, url_regex)) {
        partes["protocolo"] = match[2].str();
        partes["host"] = match[3].str();
        partes["porta"] = match[5].str();
        partes["caminho"] = match[6].str();
        partes["query"] = match[8].str();
        partes["fragmento"] = match[10].str();
        
        if (partes["protocolo"].empty()) partes["protocolo"] = "http";
        if (partes["porta"].empty()) {
            partes["porta"] = partes["protocolo"] == "https" ? "443" : "80";
        }
    }
    
    return partes;
}

// ============ CODIFICAÇÃO URL ============

std::string ServicoRede::url_codificar(const std::string& texto) {
    std::string resultado;
    
    for (unsigned char c : texto) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            resultado += c;
        } else {
            char buf[4];
            snprintf(buf, sizeof(buf), "%%%02X", c);
            resultado += buf;
        }
    }
    
    return resultado;
}

std::string ServicoRede::url_decodificar(const std::string& texto) {
    std::string resultado;
    
    for (size_t i = 0; i < texto.length(); ++i) {
        if (texto[i] == '%' && i + 2 < texto.length()) {
            char hex[3] = {texto[i+1], texto[i+2], '\0'};
            char* endptr;
            long val = strtol(hex, &endptr, 16);
            if (*endptr == '\0') {
                resultado += static_cast<char>(val);
                i += 2;
                continue;
            }
        }
        resultado += texto[i];
    }
    
    return resultado;
}

// ============ DNS ============

std::vector<std::string> ServicoRede::dns_resolver(const std::string& hostname) {
    // Verificar cache primeiro
    {
        std::lock_guard<std::mutex> lock(cache_mutex);
        auto it = cache_dns.find(hostname);
        if (it != cache_dns.end()) {
            return it->second;
        }
    }
    
    std::vector<std::string> ips;
    
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    int rc = getaddrinfo(hostname.c_str(), nullptr, &hints, &result);
    if (rc == 0) {
        for (struct addrinfo* p = result; p != nullptr; p = p->ai_next) {
            char ip[INET6_ADDRSTRLEN];
            
            if (p->ai_family == AF_INET) {
                struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
                inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
                ips.push_back(ip);
            } else if (p->ai_family == AF_INET6) {
                struct sockaddr_in6* addr = (struct sockaddr_in6*)p->ai_addr;
                inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
                ips.push_back(ip);
            }
        }
        freeaddrinfo(result);
    }
    
    // Atualizar cache
    if (!ips.empty()) {
        std::lock_guard<std::mutex> lock(cache_mutex);
        cache_dns[hostname] = ips;
    }
    
    return ips;
}

std::string ServicoRede::dns_reverso(const std::string& ip) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    
    char host[NI_MAXHOST];
    int rc = getnameinfo((struct sockaddr*)&addr, sizeof(addr), host, sizeof(host), nullptr, 0, 0);
    
    if (rc == 0) {
        return host;
    }
    
    return "";
}

// ============ UTILITÁRIOS DE REDE ============

std::string ServicoRede::ip_local() {
    char hostname[1024];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        struct addrinfo hints, *result;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        
        if (getaddrinfo(hostname, nullptr, &hints, &result) == 0) {
            for (struct addrinfo* p = result; p != nullptr; p = p->ai_next) {
                if (p->ai_family == AF_INET) {
                    struct sockaddr_in* addr = (struct sockaddr_in*)p->ai_addr;
                    char ip[INET_ADDRSTRLEN];
                    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
                    
                    // Ignorar loopback
                    if (std::string(ip) != "127.0.0.1") {
                        freeaddrinfo(result);
                        return ip;
                    }
                }
            }
            freeaddrinfo(result);
        }
    }
    
    return "127.0.0.1";
}

std::string ServicoRede::ip_publico() {
    // Tentar obter IP público via API externa
    RespostaHTTP resp = http_get("http://api.ipify.org");
    if (!resp.falhou && resp.codigo == 200) {
        return resp.conteudo;
    }
    
    return "";
}

bool ServicoRede::verificar_conectividade() {
    RespostaHTTP resp = http_get("http://google.com");
    return !resp.falhou;
}

bool ServicoRede::ping(const std::string& host, int timeout_ms) {
#ifdef _WIN32
    // No Windows, ping requer CreateProcess
    std::string cmd = "ping -n 1 -w " + std::to_string(timeout_ms) + " " + host + " > nul 2>&1";
    return system(cmd.c_str()) == 0;
#else
    std::string cmd = "ping -c 1 -W " + std::to_string(timeout_ms / 1000) + " " + host + " > /dev/null 2>&1";
    return system(cmd.c_str()) == 0;
#endif
}

// ============ MÉTODOS PRIVADOS ============

RespostaHTTP ServicoRede::fazer_requisicao_http(const std::string& metodo, const std::string& url,
                                                 const std::string& dados,
                                                 const std::map<std::string, std::string>& cabecalhos) {
    RespostaHTTP resposta;
    
    int porta;
    std::string host = extrair_host_porta(url, porta);
    
    if (host.empty()) {
        resposta.falhou = true;
        resposta.erro = "URL inválida: " + url;
        return resposta;
    }
    
    int socket_fd;
    if (!conectar_socket(host, porta, socket_fd)) {
        resposta.falhou = true;
        resposta.erro = "Não foi possível conectar a " + host + ":" + std::to_string(porta);
        return resposta;
    }
    
    // Construir requisição HTTP
    std::string caminho = "/";
    size_t pos = url.find(host);
    if (pos != std::string::npos) {
        pos += host.length();
        if (url[pos] == ':') {
            // Pular porta
            pos = url.find('/', pos);
        }
        if (pos != std::string::npos && pos < url.length()) {
            caminho = url.substr(pos);
        } else {
            caminho = "/";
        }
    }
    
    std::stringstream req;
    req << metodo << " " << caminho << " HTTP/1.1\r\n";
    req << "Host: " << host << "\r\n";
    
    for (const auto& [chave, valor] : cabecalhos) {
        req << chave << ": " << valor << "\r\n";
    }
    
    if (!dados.empty()) {
        req << "Content-Length: " << dados.length() << "\r\n";
        req << "Content-Type: application/json\r\n";
    }
    
    req << "Connection: close\r\n\r\n";
    
    if (!dados.empty()) {
        req << dados;
    }
    
    if (!enviar_socket(socket_fd, req.str())) {
        resposta.falhou = true;
        resposta.erro = "Erro ao enviar requisição";
        fechar_socket_plataforma(socket_fd);
        return resposta;
    }
    
    std::string resposta_raw = receber_socket(socket_fd, 5000);
    fechar_socket_plataforma(socket_fd);
    
    if (resposta_raw.empty()) {
        resposta.falhou = true;
        resposta.erro = "Resposta vazia";
        return resposta;
    }
    
    // Parsear resposta HTTP
    size_t linha_end = resposta_raw.find("\r\n");
    if (linha_end == std::string::npos) {
        resposta.falhou = true;
        resposta.erro = "Resposta HTTP inválida";
        return resposta;
    }
    
    std::string linha_status = resposta_raw.substr(0, linha_end);
    sscanf(linha_status.c_str(), "HTTP/1.%*d %d", &resposta.codigo);
    
    size_t cabecalhos_end = resposta_raw.find("\r\n\r\n");
    if (cabecalhos_end != std::string::npos) {
        std::string cabecalhos_str = resposta_raw.substr(linha_end + 2, cabecalhos_end - linha_end - 2);
        resposta.cabecalhos = parsear_cabecalhos_http(cabecalhos_str);
        resposta.conteudo = resposta_raw.substr(cabecalhos_end + 4);
    } else {
        resposta.conteudo = resposta_raw;
    }
    
    resposta.falhou = false;
    return resposta;
}

std::string ServicoRede::extrair_host_porta(const std::string& url, int& porta) {
    porta = 80;
    
    // Remover protocolo
    std::string url_limpa = url;
    size_t pos = url.find("://");
    if (pos != std::string::npos) {
        if (url.substr(0, pos) == "https") porta = 443;
        url_limpa = url.substr(pos + 3);
    }
    
    // Extrair host (até /, : ou fim)
    size_t path_start = url_limpa.find('/');
    if (path_start == 0) path_start = std::string::npos;
    
    size_t porta_pos = url_limpa.find(':');
    size_t host_end = url_limpa.find_first_of("/:");
    
    std::string host;
    if (host_end == std::string::npos) {
        host = url_limpa;
    } else {
        host = url_limpa.substr(0, host_end);
    }
    
    // Extrair porta explícita
    if (porta_pos != std::string::npos) {
        size_t porta_end = url_limpa.find('/', porta_pos);
        if (porta_end == std::string::npos) {
            porta_end = url_limpa.length();
        }
        std::string porta_str = url_limpa.substr(porta_pos + 1, porta_end - porta_pos - 1);
        try {
            porta = std::stoi(porta_str);
        } catch (...) {}
    }
    
    return host;
}

std::string ServicoRede::construir_cabecalhos_http(const std::map<std::string, std::string>& cabecalhos) {
    std::string resultado;
    for (const auto& [chave, valor] : cabecalhos) {
        resultado += chave + ": " + valor + "\r\n";
    }
    return resultado;
}

std::map<std::string, std::string> ServicoRede::parsear_cabecalhos_http(const std::string& cabecalhos) {
    std::map<std::string, std::string> resultado;
    std::stringstream ss(cabecalhos);
    std::string linha;
    
    while (std::getline(ss, linha, '\n')) {
        if (linha.back() == '\r') linha.pop_back();
        
        size_t colon = linha.find(':');
        if (colon != std::string::npos) {
            std::string chave = linha.substr(0, colon);
            std::string valor = linha.substr(colon + 1);
            
            // Remover espaços
            chave.erase(0, chave.find_first_not_of(" \t"));
            chave.erase(chave.find_last_not_of(" \t") + 1);
            valor.erase(0, valor.find_first_not_of(" \t"));
            valor.erase(valor.find_last_not_of(" \t") + 1);
            
            resultado[chave] = valor;
        }
    }
    
    return resultado;
}

bool ServicoRede::conectar_socket(const std::string& host, int porta, int& socket_fd) {
#ifdef _WIN32
    if (!iniciar_winsock()) return false;
#endif
    
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    std::string porta_str = std::to_string(porta);
    int rc = getaddrinfo(host.c_str(), porta_str.c_str(), &hints, &result);
    if (rc != 0) return false;
    
    for (struct addrinfo* p = result; p != nullptr; p = p->ai_next) {
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_fd < 0) continue;
        
        // Timeout de conexão
#ifdef _WIN32
        u_long mode = 1;
        ioctlsocket(socket_fd, FIONBIO, &mode);
#else
        int flags = fcntl(socket_fd, F_GETFL, 0);
        fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
#endif
        
        if (connect(socket_fd, p->ai_addr, p->ai_addrlen) < 0) {
#ifdef _WIN32
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
                fechar_socket_plataforma(socket_fd);
                continue;
            }
#else
            if (errno != EINPROGRESS) {
                fechar_socket_plataforma(socket_fd);
                continue;
            }
#endif
            
            // Aguardar conexão
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(socket_fd, &fdset);
            
            struct timeval tv;
            tv.tv_sec = 5;
            tv.tv_usec = 0;
            
            if (select(socket_fd + 1, nullptr, &fdset, nullptr, &tv) <= 0) {
                fechar_socket_plataforma(socket_fd);
                continue;
            }
            
            int so_error;
            socklen_t len = sizeof(so_error);
            getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, (char*)&so_error, &len);
            if (so_error != 0) {
                fechar_socket_plataforma(socket_fd);
                continue;
            }
        }
        
        // Voltar para modo bloqueante
#ifdef _WIN32
        u_long mode_block = 0;
        ioctlsocket(socket_fd, FIONBIO, &mode_block);
#else
        fcntl(socket_fd, F_SETFL, flags & ~O_NONBLOCK);
#endif
        
        freeaddrinfo(result);
        return true;
    }
    
    freeaddrinfo(result);
    fechar_socket_plataforma(socket_fd);
    return false;
}

bool ServicoRede::enviar_socket(int socket_fd, const std::string& dados) {
    size_t total = 0;
    size_t len = dados.length();
    
    while (total < len) {
        int sent = send(socket_fd, dados.c_str() + total, len - total, 0);
        if (sent < 0) return false;
        total += sent;
    }
    
    return true;
}

std::string ServicoRede::receber_socket(int socket_fd, int timeout_ms) {
    std::string resultado;
    char buffer[4096];
    
    // Configurar timeout
#ifdef _WIN32
    int timeout = timeout_ms;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif
    
    while (true) {
        int bytes = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes < 0) {
#ifdef _WIN32
            if (WSAGetLastError() == WSAETIMEDOUT) break;
#else
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
#endif
            break;
        }
        if (bytes == 0) break;
        
        buffer[bytes] = '\0';
        resultado += buffer;
    }
    
    return resultado;
}

// ============ SERVIDOR HTTP COMPLETO ============

// Estruturas internas do servidor
struct RotaHTTP {
    std::string metodo;
    std::string caminho;
    std::vector<std::string> parametros;
    HandlerHTTP handler;
};

struct ServidorHTTPInterno {
    int id;
    int porta;
    int socket_servidor;
    std::atomic<bool> rodando;
    std::vector<RotaHTTP> rotas;
    std::vector<MiddlewareHTTP> middlewares;
    std::thread thread_aceitacao;
    
    ServidorHTTPInterno(int pid, int pporta) : id(pid), porta(pporta), socket_servidor(-1), rodando(false) {}
};

static std::unordered_map<int, std::shared_ptr<ServidorHTTPInterno>> servidores;
static std::mutex servidores_mutex;
static std::atomic<int> proximo_id_servidor{1};

// Funções auxiliares do servidor
namespace {
    std::vector<std::string> dividir(const std::string& s, char delim) {
        std::vector<std::string> resultado;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            if (!item.empty()) resultado.push_back(item);
        }
        return resultado;
    }
    
    std::string trim(const std::string& s) {
        size_t inicio = s.find_first_not_of(" \t\r\n");
        if (inicio == std::string::npos) return "";
        size_t fim = s.find_last_not_of(" \t\r\n");
        return s.substr(inicio, fim - inicio + 1);
    }
    
    std::string paraMinusculas(const std::string& s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(), ::tolower);
        return r;
    }
    
    std::map<std::string, std::string> parsearQueryString(const std::string& query) {
        std::map<std::string, std::string> resultado;
        if (query.empty()) return resultado;
        
        std::string q = query;
        if (q[0] == '?') q = q.substr(1);
        
        for (auto& par : dividir(q, '&')) {
            size_t igual = par.find('=');
            if (igual != std::string::npos) {
                std::string chave = ServicoRede::url_decodificar(par.substr(0, igual));
                std::string valor = ServicoRede::url_decodificar(par.substr(igual + 1));
                resultado[chave] = valor;
            } else {
                resultado[ServicoRede::url_decodificar(par)] = "";
            }
        }
        
        return resultado;
    }
    
    bool corresponderRota(const std::string& caminho_rota, const std::string& caminho_req,
                          std::map<std::string, std::string>& parametros_valores) {
        auto partes_rota = dividir(caminho_rota, '/');
        auto partes_req = dividir(caminho_req, '/');
        
        if (partes_rota.size() != partes_req.size()) return false;
        
        parametros_valores.clear();
        for (size_t i = 0; i < partes_rota.size(); i++) {
            if (!partes_rota[i].empty() && partes_rota[i][0] == ':') {
                std::string nome = partes_rota[i].substr(1);
                parametros_valores[nome] = partes_req[i];
            } else if (partes_rota[i] != partes_req[i]) {
                return false;
            }
        }
        
        return true;
    }
    
    std::string lerRequisicaoCompleta(int socket) {
        std::string resultado;
        char buffer[8192];
        
        while (true) {
            int bytes = recv(socket, buffer, sizeof(buffer) - 1, 0);
            if (bytes <= 0) break;
            buffer[bytes] = '\0';
            resultado += buffer;
            
            size_t headers_end = resultado.find("\r\n\r\n");
            if (headers_end != std::string::npos) {
                std::string headers = resultado.substr(0, headers_end);
                size_t cl_pos = paraMinusculas(headers).find("content-length:");
                if (cl_pos != std::string::npos) {
                    size_t valor_start = headers.find(':', cl_pos) + 1;
                    size_t valor_end = headers.find('\r', valor_start);
                    int content_length = atoi(headers.substr(valor_start, valor_end - valor_start).c_str());
                    size_t corpo_inicio = headers_end + 4;
                    if (resultado.length() - corpo_inicio >= (size_t)content_length) {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
        
        return resultado;
    }
    
    RequisicaoHTTP parsearRequisicao(const std::string& raw) {
        RequisicaoHTTP req;
        
        std::stringstream ss(raw);
        std::string linha;
        
        if (std::getline(ss, linha)) {
            if (linha.back() == '\r') linha.pop_back();
            std::stringstream linha_ss(linha);
            linha_ss >> req.metodo >> req.caminho >> req.protocolo;
            
            size_t q_pos = req.caminho.find('?');
            if (q_pos != std::string::npos) {
                std::string query_str = req.caminho.substr(q_pos);
                req.caminho = req.caminho.substr(0, q_pos);
                req.query = parsearQueryString(query_str);
            }
        }
        
        bool corpo_iniciado = false;
        while (std::getline(ss, linha)) {
            if (linha.back() == '\r') linha.pop_back();
            if (linha.empty()) {
                corpo_iniciado = true;
                continue;
            }
            
            if (!corpo_iniciado) {
                size_t colon = linha.find(':');
                if (colon != std::string::npos) {
                    std::string chave = trim(linha.substr(0, colon));
                    std::string valor = trim(linha.substr(colon + 1));
                    req.cabecalhos[chave] = valor;
                }
            } else {
                req.corpo += linha + "\n";
            }
        }
        
        if (!req.corpo.empty() && req.corpo.back() == '\n') {
            req.corpo.pop_back();
        }
        
        return req;
    }
    
    std::string construirRespostaHTTP(const RespostaServidor& resp) {
        std::stringstream ss;
        ss << "HTTP/1.1 " << resp.codigo << " ";
        
        switch (resp.codigo) {
            case 200: ss << "OK"; break;
            case 201: ss << "Created"; break;
            case 204: ss << "No Content"; break;
            case 301: ss << "Moved Permanently"; break;
            case 302: ss << "Found"; break;
            case 400: ss << "Bad Request"; break;
            case 401: ss << "Unauthorized"; break;
            case 403: ss << "Forbidden"; break;
            case 404: ss << "Not Found"; break;
            case 405: ss << "Method Not Allowed"; break;
            case 500: ss << "Internal Server Error"; break;
            case 501: ss << "Not Implemented"; break;
            default: ss << "Unknown"; break;
        }
        
        ss << "\r\n";
        ss << "Content-Type: " << resp.tipo_conteudo << "\r\n";
        ss << "Content-Length: " << resp.conteudo.size() << "\r\n";
        ss << "Access-Control-Allow-Origin: *\r\n";
        ss << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, PATCH, OPTIONS\r\n";
        ss << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
        ss << "Connection: close\r\n";
        
        for (auto& [chave, valor] : resp.cabecalhos) {
            ss << chave << ": " << valor << "\r\n";
        }
        
        ss << "\r\n";
        ss << resp.conteudo;
        
        return ss.str();
    }
    
    void processarConexao(std::shared_ptr<ServidorHTTPInterno> servidor, int socket_cliente) {
        std::string raw = lerRequisicaoCompleta(socket_cliente);
        if (raw.empty()) {
            fechar_socket_plataforma(socket_cliente);
            return;
        }
        
        RequisicaoHTTP req = parsearRequisicao(raw);
        
        if (req.metodo == "OPTIONS") {
            RespostaServidor resp;
            resp.codigo = 204;
            resp.conteudo = "";
            std::string resposta = construirRespostaHTTP(resp);
            ServicoRede::enviar_socket_publico(socket_cliente, resposta);
            fechar_socket_plataforma(socket_cliente);
            return;
        }
        
        RespostaServidor resposta;
        bool rota_encontrada = false;
        
        for (auto& middleware : servidor->middlewares) {
            if (!middleware(req, resposta)) {
                std::string resposta_str = construirRespostaHTTP(resposta);
                ServicoRede::enviar_socket_publico(socket_cliente, resposta_str);
                fechar_socket_plataforma(socket_cliente);
                return;
            }
        }
        
        for (auto& rota : servidor->rotas) {
            if (paraMinusculas(rota.metodo) != paraMinusculas(req.metodo)) continue;
            
            std::map<std::string, std::string> params;
            if (corresponderRota(rota.caminho, req.caminho, params)) {
                req.parametros = params;
                try {
                    resposta = rota.handler(req);
                } catch (const std::exception& e) {
                    resposta = RespostaServidor::erroInterno();
                    resposta.conteudo = "<h1>500 - Erro interno</h1><p>" + std::string(e.what()) + "</p>";
                }
                rota_encontrada = true;
                break;
            }
        }
        
        if (!rota_encontrada) {
            resposta = RespostaServidor::naoEncontrado();
        }
        
        std::string resposta_str = construirRespostaHTTP(resposta);
        ServicoRede::enviar_socket_publico(socket_cliente, resposta_str);
        fechar_socket_plataforma(socket_cliente);
    }
    
    void threadAceitacao(std::shared_ptr<ServidorHTTPInterno> servidor) {
        while (servidor->rodando) {
            sockaddr_in cliente_endereco;
            socklen_t cliente_tamanho = sizeof(cliente_endereco);
            int socket_cliente = accept(servidor->socket_servidor, (struct sockaddr*)&cliente_endereco, &cliente_tamanho);
            
            if (socket_cliente < 0) {
                if (!servidor->rodando) break;
                continue;
            }
            
            std::thread([servidor, socket_cliente]() {
                processarConexao(servidor, socket_cliente);
            }).detach();
        }
    }
}

// API pública do servidor HTTP
int ServicoRede::criar_servidor(int porta) {
#ifdef _WIN32
    if (!iniciar_winsock()) return -1;
#endif
    
    int id = proximo_id_servidor++;
    auto servidor = std::make_shared<ServidorHTTPInterno>(id, porta);
    
    servidor->socket_servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor->socket_servidor < 0) {
        std::cerr << "[Rede BR] Erro ao criar socket do servidor" << std::endl;
        return -1;
    }
    
    int opt = 1;
    setsockopt(servidor->socket_servidor, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    
    sockaddr_in endereco;
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(porta);
    
    if (bind(servidor->socket_servidor, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
        std::cerr << "[Rede BR] Erro ao fazer bind na porta " << porta << std::endl;
        fechar_socket_plataforma(servidor->socket_servidor);
        return -1;
    }
    
    if (listen(servidor->socket_servidor, 10) < 0) {
        std::cerr << "[Rede BR] Erro ao escutar na porta " << porta << std::endl;
        fechar_socket_plataforma(servidor->socket_servidor);
        return -1;
    }
    
    {
        std::lock_guard<std::mutex> lock(servidores_mutex);
        servidores[id] = servidor;
    }
    
    return id;
}

bool ServicoRede::adicionar_rota(int servidor_id, const std::string& metodo, const std::string& caminho, HandlerHTTP handler) {
    std::lock_guard<std::mutex> lock(servidores_mutex);
    auto it = servidores.find(servidor_id);
    if (it == servidores.end()) return false;
    
    RotaHTTP rota;
    rota.metodo = metodo;
    rota.caminho = caminho;
    rota.handler = handler;
    
    for (auto& parte : dividir(caminho, '/')) {
        if (!parte.empty() && parte[0] == ':') {
            rota.parametros.push_back(parte.substr(1));
        }
    }
    
    it->second->rotas.push_back(rota);
    return true;
}

bool ServicoRede::adicionar_middleware(int servidor_id, MiddlewareHTTP middleware) {
    std::lock_guard<std::mutex> lock(servidores_mutex);
    auto it = servidores.find(servidor_id);
    if (it == servidores.end()) return false;
    
    it->second->middlewares.push_back(middleware);
    return true;
}

bool ServicoRede::iniciar_servidor(int servidor_id) {
    std::lock_guard<std::mutex> lock(servidores_mutex);
    auto it = servidores.find(servidor_id);
    if (it == servidores.end()) return false;
    
    auto& servidor = it->second;
    if (servidor->rodando) return true;
    
    servidor->rodando = true;
    servidor->thread_aceitacao = std::thread(threadAceitacao, servidor);
    
    std::cout << "[Rede BR] Servidor HTTP rodando em http://localhost:" << servidor->porta << std::endl;
    return true;
}

bool ServicoRede::parar_servidor(int servidor_id) {
    std::lock_guard<std::mutex> lock(servidores_mutex);
    auto it = servidores.find(servidor_id);
    if (it == servidores.end()) return false;
    
    auto& servidor = it->second;
    servidor->rodando = false;
    
    if (servidor->socket_servidor >= 0) {
        fechar_socket_plataforma(servidor->socket_servidor);
        servidor->socket_servidor = -1;
    }
    
    if (servidor->thread_aceitacao.joinable()) {
        servidor->thread_aceitacao.join();
    }
    
    servidores.erase(it);
    return true;
}

bool ServicoRede::servidor_rodando(int servidor_id) {
    std::lock_guard<std::mutex> lock(servidores_mutex);
    auto it = servidores.find(servidor_id);
    if (it == servidores.end()) return false;
    return it->second->rodando;
}

void ServicoRede::parar_todos_servidores() {
    std::lock_guard<std::mutex> lock(servidores_mutex);
    for (auto& [id, servidor] : servidores) {
        servidor->rodando = false;
        if (servidor->socket_servidor >= 0) {
            fechar_socket_plataforma(servidor->socket_servidor);
            servidor->socket_servidor = -1;
        }
        if (servidor->thread_aceitacao.joinable()) {
            servidor->thread_aceitacao.join();
        }
    }
    servidores.clear();
}
