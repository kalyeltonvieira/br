// Serviço IA Nativo para BR v2.0
// Integração com LLMs (OpenAI, Anthropic, etc.)
#ifndef SERVICO_IA_H
#define SERVICO_IA_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// Estrutura de mensagem para chat
struct MensagemIA {
    enum Tipo { USUARIO, ASSISTENTE, SISTEMA };
    
    Tipo tipo;
    std::string conteudo;
    std::map<std::string, std::string> metadados;
    
    MensagemIA(Tipo t = USUARIO, const std::string& c = "") 
        : tipo(t), conteudo(c) {}
};

// Estrutura de resposta da IA
struct RespostaIA {
    std::string texto;
    std::string modelo;
    int tokens_usados;
    bool sucesso;
    std::string erro;
    
    RespostaIA() : tokens_usados(0), sucesso(false) {}
};

// Configuração do provedor de IA
enum ProvedorIA { OPENAI, ANTHROPIC, GOOGLE, LOCAL, OLLAMA };

struct ConfiguracaoIA {
    ProvedorIA provedor;
    std::string api_key;
    std::string modelo;
    std::string url_base;
    double temperatura;
    int max_tokens;
    int timeout_segundos;
    
    ConfiguracaoIA() 
        : provedor(OPENAI), modelo("gpt-3.5-turbo"), 
          temperatura(0.7), max_tokens(1000), timeout_segundos(30) {
        url_base = "https://api.openai.com/v1";
    }
};

// Agente de IA persistente
struct AgenteIA {
    std::string id;
    std::string nome;
    std::string especialidade;
    std::string instrucao_sistema;
    std::vector<MensagemIA> historico;
    ConfiguracaoIA config;
    
    AgenteIA() {}
    AgenteIA(const std::string& n, const std::string& esp) 
        : nome(n), especialidade(esp) {}
};

// Serviço de IA
class ServicoIA {
private:
    ConfiguracaoIA config_padrao;
    std::map<std::string, AgenteIA> agentes;
    std::map<std::string, std::string> cache_respostas;
    
public:
    ServicoIA();
    ~ServicoIA();
    
    // Configuração
    void definir_configuracao(const ConfiguracaoIA& config);
    ConfiguracaoIA obter_configuracao() const;
    
    // Perguntas simples (stateless)
    RespostaIA perguntar(const std::string& pergunta);
    RespostaIA perguntar_com_contexto(const std::string& pergunta, 
                                       const std::vector<std::string>& contexto);
    
    // Agentes persistentes
    std::string criar_agente(const std::string& nome, const std::string& especialidade,
                             const std::string& instrucao_sistema = "");
    AgenteIA* obter_agente(const std::string& id);
    RespostaIA agente_perguntar(const std::string& id_agente, const std::string& pergunta);
    void limpar_historico_agente(const std::string& id_agente);
    void remover_agente(const std::string& id_agente);
    
    // Chat com contexto
    RespostaIA chat(const std::vector<MensagemIA>& mensagens);
    
    // Streaming (callback para cada chunk)
    void perguntar_stream(const std::string& pergunta, 
                         std::function<void(const std::string&)> callback);
    
    // Funções especializadas
    RespostaIA classificar_texto(const std::string& texto, 
                                  const std::vector<std::string>& categorias);
    RespostaIA resumir_texto(const std::string& texto, int max_caracteres = 200);
    RespostaIA traduzir_texto(const std::string& texto, const std::string& idioma_destino);
    RespostaIA extrair_entidades(const std::string& texto);
    RespostaIA analisar_sentimento(const std::string& texto);
    
    // Geração de código
    RespostaIA gerar_codigo(const std::string& descricao, const std::string& linguagem = "br");
    RespostaIA explicar_codigo(const std::string& codigo);
    RespostaIA refatorar_codigo(const std::string& codigo, const std::string& objetivo = "");
    
    // Debugging inteligente
    RespostaIA analisar_erro(const std::string& codigo, const std::string& erro);
    RespostaIA sugerir_correcao(const std::string& codigo, const std::string& erro);
    
    // Cache
    void habilitar_cache(bool habilitar);
    void limpar_cache();
    
private:
    // Implementações específicas de provedores
    RespostaIA chamar_openai(const std::string& prompt, 
                              const std::vector<MensagemIA>& historico = {});
    RespostaIA chamar_anthropic(const std::string& prompt,
                                 const std::vector<MensagemIA>& historico = {});
    RespostaIA chamar_google(const std::string& prompt,
                             const std::vector<MensagemIA>& historico = {});
    RespostaIA chamar_ollama(const std::string& prompt,
                              const std::vector<MensagemIA>& historico = {});
    
    // Utilitários
    std::string construir_prompt_sistema(const AgenteIA& agente);
    std::string formatar_mensagens_json(const std::vector<MensagemIA>& mensagens);
    std::string fazer_requisicao_http(const std::string& url, 
                                      const std::string& corpo,
                                      const std::map<std::string, std::string>& cabecalhos);
    std::string parsear_resposta_json(const std::string& json);
    
    bool cache_habilitado;
};

#endif // SERVICO_IA_H
