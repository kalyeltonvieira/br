// Implementação do Serviço IA Nativo para BR v2.0
#include "servico_ia.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>
#include <chrono>

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
#endif

ServicoIA::ServicoIA() : cache_habilitado(true) {
    // Tentar ler API key de variável de ambiente
    const char* openai_key = std::getenv("OPENAI_API_KEY");
    if (openai_key) {
        config_padrao.api_key = openai_key;
    }
    
    const char* anthropic_key = std::getenv("ANTHROPIC_API_KEY");
    if (anthropic_key) {
        config_padrao.api_key = anthropic_key;
        config_padrao.provedor = ANTHROPIC;
        config_padrao.url_base = "https://api.anthropic.com/v1";
        config_padrao.modelo = "claude-3-sonnet-20240229";
    }
}

ServicoIA::~ServicoIA() {
    // Limpar agentes
    agentes.clear();
    cache_respostas.clear();
}

void ServicoIA::definir_configuracao(const ConfiguracaoIA& config) {
    config_padrao = config;
}

ConfiguracaoIA ServicoIA::obter_configuracao() const {
    return config_padrao;
}

RespostaIA ServicoIA::perguntar(const std::string& pergunta) {
    // Verificar cache
    if (cache_habilitado) {
        auto it = cache_respostas.find(pergunta);
        if (it != cache_respostas.end()) {
            RespostaIA resp;
            resp.texto = it->second;
            resp.sucesso = true;
            return resp;
        }
    }
    
    // Chamar provedor apropriado
    RespostaIA resposta;
    
    switch (config_padrao.provedor) {
        case OPENAI:
            resposta = chamar_openai(pergunta);
            break;
        case ANTHROPIC:
            resposta = chamar_anthropic(pergunta);
            break;
        case GOOGLE:
            resposta = chamar_google(pergunta);
            break;
        case OLLAMA:
            resposta = chamar_ollama(pergunta);
            break;
        case LOCAL:
            // Fallback para respostas locais simples
            resposta = resposta_local_simples(pergunta);
            break;
    }
    
    // Salvar no cache se sucesso
    if (resposta.sucesso && cache_habilitado) {
        cache_respostas[pergunta] = resposta.texto;
    }
    
    return resposta;
}

RespostaIA ServicoIA::perguntar_com_contexto(const std::string& pergunta, 
                                               const std::vector<std::string>& contexto) {
    std::string prompt_completo = "Contexto:\n";
    for (const auto& ctx : contexto) {
        prompt_completo += ctx + "\n";
    }
    prompt_completo += "\nPergunta: " + pergunta;
    
    return perguntar(prompt_completo);
}

std::string ServicoIA::criar_agente(const std::string& nome, const std::string& especialidade,
                                     const std::string& instrucao_sistema) {
    // Gerar ID único
    std::string id = "agente_" + nome + "_" + 
                     std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    
    AgenteIA agente(nome, especialidade);
    agente.id = id;
    agente.instrucao_sistema = instrucao_sistema;
    agente.config = config_padrao;
    
    agentes[id] = agente;
    
    return id;
}

AgenteIA* ServicoIA::obter_agente(const std::string& id) {
    auto it = agentes.find(id);
    if (it != agentes.end()) {
        return &it->second;
    }
    return nullptr;
}

RespostaIA ServicoIA::agente_perguntar(const std::string& id_agente, const std::string& pergunta) {
    AgenteIA* agente = obter_agente(id_agente);
    if (!agente) {
        RespostaIA resp;
        resp.sucesso = false;
        resp.erro = "Agente não encontrado: " + id_agente;
        return resp;
    }
    
    // Adicionar pergunta ao histórico
    agente->historico.push_back(MensagemIA(MensagemIA::USUARIO, pergunta));
    
    // Construir prompt com instrução do sistema
    std::string prompt_completo = construir_prompt_sistema(*agente) + "\n\n";
    
    // Adicionar histórico recente (últimas 10 mensagens)
    int inicio = std::max(0, (int)agente->historico.size() - 10);
    for (int i = inicio; i < (int)agente->historico.size(); i++) {
        prompt_completo += agente->historico[i].conteudo + "\n";
    }
    
    // Chamar provedor com contexto
    RespostaIA resposta;
    switch (agente->config.provedor) {
        case OPENAI:
            resposta = chamar_openai(pergunta, agente->historico);
            break;
        case ANTHROPIC:
            resposta = chamar_anthropic(pergunta, agente->historico);
            break;
        case GOOGLE:
            resposta = chamar_google(pergunta, agente->historico);
            break;
        case OLLAMA:
            resposta = chamar_ollama(pergunta, agente->historico);
            break;
        case LOCAL:
            resposta = resposta_local_simples(pergunta);
            break;
    }
    
    // Adicionar resposta ao histórico
    if (resposta.sucesso) {
        agente->historico.push_back(MensagemIA(MensagemIA::ASSISTENTE, resposta.texto));
    }
    
    return resposta;
}

void ServicoIA::limpar_historico_agente(const std::string& id_agente) {
    AgenteIA* agente = obter_agente(id_agente);
    if (agente) {
        agente->historico.clear();
    }
}

void ServicoIA::remover_agente(const std::string& id_agente) {
    agentes.erase(id_agente);
}

RespostaIA ServicoIA::chat(const std::vector<MensagemIA>& mensagens) {
    // Converter mensagens para formato do provedor
    std::string prompt;
    for (const auto& msg : mensagens) {
        switch (msg.tipo) {
            case MensagemIA::SISTEMA:
                prompt += "[SISTEMA] " + msg.conteudo + "\n";
                break;
            case MensagemIA::USUARIO:
                prompt += "[USUÁRIO] " + msg.conteudo + "\n";
                break;
            case MensagemIA::ASSISTENTE:
                prompt += "[ASSISTENTE] " + msg.conteudo + "\n";
                break;
        }
    }
    
    return perguntar(prompt);
}

void ServicoIA::perguntar_stream(const std::string& pergunta,
                                  std::function<void(const std::string&)> callback) {
    // Streaming seria implementado com HTTP chunked transfer
    // Por enquanto, chamamos normal e chamamos callback com resultado completo
    RespostaIA resposta = perguntar(pergunta);
    if (resposta.sucesso) {
        callback(resposta.texto);
    }
}

RespostaIA ServicoIA::classificar_texto(const std::string& texto,
                                         const std::vector<std::string>& categorias) {
    std::string prompt = "Classifique o seguinte texto em uma das categorias:\n\n";
    prompt += "Texto: " + texto + "\n\n";
    prompt += "Categorias:\n";
    for (const auto& cat : categorias) {
        prompt += "- " + cat + "\n";
    }
    prompt += "\nResponda apenas com o nome da categoria.";
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::resumir_texto(const std::string& texto, int max_caracteres) {
    std::string prompt = "Resuma o seguinte texto em no máximo " + 
                        std::to_string(max_caracteres) + " caracteres:\n\n" + texto;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::traduzir_texto(const std::string& texto, const std::string& idioma_destino) {
    std::string prompt = "Traduza o seguinte texto para " + idioma_destino + ":\n\n" + texto;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::extrair_entidades(const std::string& texto) {
    std::string prompt = "Extraia todas as entidades (nomes, lugares, organizações, datas, etc.) do seguinte texto:\n\n" + texto;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::analisar_sentimento(const std::string& texto) {
    std::string prompt = "Analise o sentimento do seguinte texto (positivo, negativo ou neutro):\n\n" + texto;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::gerar_codigo(const std::string& descricao, const std::string& linguagem) {
    std::string prompt = "Gere código em " + linguagem + " para:\n\n" + descricao;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::explicar_codigo(const std::string& codigo) {
    std::string prompt = "Explique o seguinte código:\n\n" + codigo;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::refatorar_codigo(const std::string& codigo, const std::string& objetivo) {
    std::string prompt = "Refatore o seguinte código";
    if (!objetivo.empty()) {
        prompt += " com o objetivo: " + objetivo;
    }
    prompt += ":\n\n" + codigo;
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::analisar_erro(const std::string& codigo, const std::string& erro) {
    std::string prompt = "Analise o seguinte erro no código:\n\n";
    prompt += "Código:\n" + codigo + "\n\n";
    prompt += "Erro:\n" + erro + "\n\n";
    prompt += "Explique a causa do erro e como corrigi-lo.";
    
    return perguntar(prompt);
}

RespostaIA ServicoIA::sugerir_correcao(const std::string& codigo, const std::string& erro) {
    std::string prompt = "Sugira uma correção para o seguinte erro:\n\n";
    prompt += "Código:\n" + codigo + "\n\n";
    prompt += "Erro:\n" + erro + "\n\n";
    prompt += "Forneça o código corrigido.";
    
    return perguntar(prompt);
}

void ServicoIA::habilitar_cache(bool habilitar) {
    cache_habilitado = habilitar;
    if (!habilitar) {
        cache_respostas.clear();
    }
}

void ServicoIA::limpar_cache() {
    cache_respostas.clear();
}

// Implementações específicas de provedores
RespostaIA ServicoIA::chamar_openai(const std::string& prompt,
                                      const std::vector<MensagemIA>& historico) {
    RespostaIA resposta;
    
    if (config_padrao.api_key.empty()) {
        resposta.sucesso = false;
        resposta.erro = "API key do OpenAI não configurada. Defina OPENAI_API_KEY.";
        return resposta;
    }
    
    // Construir JSON da requisição
    std::string json_body = "{";
    json_body += "\"model\": \"" + config_padrao.modelo + "\",";
    json_body += "\"messages\": [";
    
    // Adicionar histórico
    for (const auto& msg : historico) {
        json_body += "{";
        if (msg.tipo == MensagemIA::SISTEMA) {
            json_body += "\"role\": \"system\", \"content\": \"" + msg.conteudo + "\"";
        } else if (msg.tipo == MensagemIA::USUARIO) {
            json_body += "\"role\": \"user\", \"content\": \"" + msg.conteudo + "\"";
        } else {
            json_body += "\"role\": \"assistant\", \"content\": \"" + msg.conteudo + "\"";
        }
        json_body += "},";
    }
    
    // Adicionar prompt atual
    json_body += "{\"role\": \"user\", \"content\": \"" + prompt + "\"}";
    json_body += "],";
    json_body += "\"max_tokens\": " + std::to_string(config_padrao.max_tokens) + ",";
    json_body += "\"temperature\": " + std::to_string(config_padrao.temperatura);
    json_body += "}";
    
    // Fazer requisição HTTP
    std::string url = config_padrao.url_base + "/chat/completions";
    
    std::map<std::string, std::string> cabecalhos;
    cabecalhos["Content-Type"] = "application/json";
    cabecalhos["Authorization"] = "Bearer " + config_padrao.api_key;
    
    std::string resposta_http = fazer_requisicao_http(url, json_body, cabecalhos);
    
    if (resposta_http.empty()) {
        resposta.sucesso = false;
        resposta.erro = "Falha na requisição HTTP";
        return resposta;
    }
    
    // Parsear resposta JSON (simplificado)
    resposta.texto = parsear_resposta_json(resposta_http);
    resposta.modelo = config_padrao.modelo;
    resposta.sucesso = !resposta.texto.empty();
    
    return resposta;
}

RespostaIA ServicoIA::chamar_anthropic(const std::string& prompt,
                                         const std::vector<MensagemIA>& historico) {
    RespostaIA resposta;
    
    if (config_padrao.api_key.empty()) {
        resposta.sucesso = false;
        resposta.erro = "API key do Anthropic não configurada. Defina ANTHROPIC_API_KEY.";
        return resposta;
    }
    
    // Implementação similar ao OpenAI mas com formato da Anthropic
    std::string json_body = "{";
    json_body += "\"model\": \"" + config_padrao.modelo + "\",";
    json_body += "\"max_tokens\": " + std::to_string(config_padrao.max_tokens) + ",";
    json_body += "\"messages\": [";
    
    for (const auto& msg : historico) {
        json_body += "{";
        if (msg.tipo == MensagemIA::USUARIO) {
            json_body += "\"role\": \"user\", \"content\": \"" + msg.conteudo + "\"";
        } else {
            json_body += "\"role\": \"assistant\", \"content\": \"" + msg.conteudo + "\"";
        }
        json_body += "},";
    }
    
    json_body += "{\"role\": \"user\", \"content\": \"" + prompt + "\"}";
    json_body += "]}";
    
    std::string url = config_padrao.url_base + "/messages";
    
    std::map<std::string, std::string> cabecalhos;
    cabecalhos["Content-Type"] = "application/json";
    cabecalhos["x-api-key"] = config_padrao.api_key;
    cabecalhos["anthropic-version"] = "2023-06-01";
    
    std::string resposta_http = fazer_requisicao_http(url, json_body, cabecalhos);
    
    resposta.texto = parsear_resposta_json(resposta_http);
    resposta.modelo = config_padrao.modelo;
    resposta.sucesso = !resposta.texto.empty();
    
    return resposta;
}

RespostaIA ServicoIA::chamar_google(const std::string& prompt,
                                      const std::vector<MensagemIA>& historico) {
    // Placeholder para Google AI (Gemini)
    RespostaIA resposta;
    resposta.sucesso = false;
    resposta.erro = "Google AI não implementado ainda";
    return resposta;
}

RespostaIA ServicoIA::chamar_ollama(const std::string& prompt,
                                      const std::vector<MensagemIA>& historico) {
    // Placeholder para Ollama (local)
    RespostaIA resposta;
    resposta.sucesso = false;
    resposta.erro = "Ollama não implementado ainda";
    return resposta;
}

RespostaIA ServicoIA::resposta_local_simples(const std::string& pergunta) {
    // Fallback com respostas simples locais
    RespostaIA resposta;
    resposta.sucesso = true;
    resposta.modelo = "local";
    
    std::string p_lower = pergunta;
    std::transform(p_lower.begin(), p_lower.end(), p_lower.begin(), ::tolower);
    
    if (p_lower.find("buraco negro") != std::string::npos) {
        resposta.texto = "Um buraco negro é uma região do espaço-tempo com gravidade tão forte que nada, nem mesmo a luz, pode escapar.";
    } else if (p_lower.find("linguagem br") != std::string::npos || p_lower.find("br ") != std::string::npos) {
        resposta.texto = "BR é uma linguagem de programação brasileira com sintaxe em português, IA nativa e runtime C++.";
    } else if (p_lower.find("como compilar") != std::string::npos) {
        resposta.texto = "Para compilar código BR, use: br_compilar arquivo.br -o saida.brc";
    } else if (p_lower.find("como executar") != std::string::npos) {
        resposta.texto = "Para executar código BR, use: bin/br.exe arquivo.br ou bin/br.exe arquivo.brc";
    } else {
        resposta.texto = "Configure OPENAI_API_KEY ou ANTHROPIC_API_KEY para usar IA real. Sem API key, apenas respostas básicas estão disponíveis.";
    }
    
    return resposta;
}

// Utilitários
std::string ServicoIA::construir_prompt_sistema(const AgenteIA& agente) {
    std::string prompt = "Você é um agente de IA especializado em " + agente.especialidade + ".\n";
    if (!agente.instrucao_sistema.empty()) {
        prompt += agente.instrucao_sistema + "\n";
    }
    prompt += "Seu nome é " + agente.nome + ".";
    return prompt;
}

std::string ServicoIA::formatar_mensagens_json(const std::vector<MensagemIA>& mensagens) {
    std::string json = "[";
    for (size_t i = 0; i < mensagens.size(); i++) {
        json += "{";
        if (mensagens[i].tipo == MensagemIA::SISTEMA) {
            json += "\"role\": \"system\"";
        } else if (mensagens[i].tipo == MensagemIA::USUARIO) {
            json += "\"role\": \"user\"";
        } else {
            json += "\"role\": \"assistant\"";
        }
        json += ", \"content\": \"" + mensagens[i].conteudo + "\"}";
        if (i < mensagens.size() - 1) json += ",";
    }
    json += "]";
    return json;
}

std::string ServicoIA::fazer_requisicao_http(const std::string& url,
                                               const std::string& corpo,
                                               const std::map<std::string, std::string>& cabecalhos) {
    // Implementação simplificada de HTTP
    // Em produção, usar libcurl ou similar
    
    // Placeholder - retorna string vazia
    return "";
}

std::string ServicoIA::parsear_resposta_json(const std::string& json) {
    // Parser JSON simplificado
    // Em produção, usar uma biblioteca JSON real
    
    // Procurar por "content": "..."
    size_t pos = json.find("\"content\":");
    if (pos == std::string::npos) return "";
    
    pos = json.find("\"", pos + 10);
    if (pos == std::string::npos) return "";
    
    size_t inicio = pos + 1;
    pos = json.find("\"", inicio);
    if (pos == std::string::npos) return "";
    
    return json.substr(inicio, pos - inicio);
}
