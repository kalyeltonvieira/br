// Gerenciador de Pacotes BR com Registro Online
#ifndef GERENCIADOR_PACOTES_H
#define GERENCIADOR_PACOTES_H

#include <string>
#include <vector>
#include <map>
#include <functional>

// Estrutura de pacote
struct PacoteBR {
    std::string nome;
    std::string versao;
    std::string descricao;
    std::string autor;
    std::string repositorio;
    std::string licenca;
    std::vector<std::string> dependencias;
    std::vector<std::string> arquivos;
    std::string checksum_sha256;
    int64_t tamanho_bytes;
    std::string data_publicacao;
    
    PacoteBR() : tamanho_bytes(0) {}
};

// Estrutura de registro de pacote (no servidor)
struct RegistroPacote {
    std::string id;
    PacoteBR pacote;
    int downloads;
    float avaliacao;
    std::vector<std::string> tags;
    bool verificado;
    
    RegistroPacote() : downloads(0), avaliacao(0.0f), verificado(false) {}
};

// Configuração do gerenciador
struct ConfiguracaoPacotes {
    std::string url_registro;
    std::string diretorio_cache;
    std::string diretorio_instalacao;
    bool usar_cache;
    bool verificar_checksum;
    
    ConfiguracaoPacotes() 
        : url_registro("https://pacotes.brlang.org"),
          diretorio_cache("~/.br/cache"),
          diretorio_instalacao("~/.br/pacotes"),
          usar_cache(true),
          verificar_checksum(true) {}
};

// Status de instalação
enum StatusInstalacao {
    INSTALADO,
    NAO_INSTALADO,
    DESATUALIZADO,
    ERRO_DEPENDENCIA
};

// Gerenciador de Pacotes
class GerenciadorPacotes {
private:
    ConfiguracaoPacotes config;
    std::map<std::string, PacoteBR> pacotes_locais;
    std::map<std::string, RegistroPacote> registro_remoto;
    
public:
    GerenciadorPacotes();
    ~GerenciadorPacotes();
    
    // Configuração
    void definir_configuracao(const ConfiguracaoPacotes& cfg);
    ConfiguracaoPacotes obter_configuracao() const;
    
    // Operações de pacotes
    bool instalar(const std::string& nome_pacote, const std::string& versao = "");
    bool desinstalar(const std::string& nome_pacote);
    bool atualizar(const std::string& nome_pacote);
    bool atualizar_todos();
    
    // Busca e listagem
    std::vector<RegistroPacote> buscar(const std::string& termo);
    std::vector<RegistroPacote> listar_instalados();
    std::vector<RegistroPacote> listar_disponiveis();
    std::vector<RegistroPacote> listar_desatualizados();
    RegistroPacote obter_info(const std::string& nome_pacote);
    
    // Registro online
    bool sincronizar_registro();
    bool publicar_pacote(const std::string& caminho_manifesto);
    bool avaliar_pacote(const std::string& nome_pacote, float nota);
    
    // Dependências
    std::vector<std::string> resolver_dependencias(const std::string& nome_pacote);
    bool verificar_dependencias(const std::string& nome_pacote);
    
    // Cache
    void limpar_cache();
    StatusInstalacao verificar_status(const std::string& nome_pacote);
    
    // Utilitários
    std::string obter_caminho_instalacao(const std::string& nome_pacote);
    bool verificar_integridade(const PacoteBR& pacote);
    
private:
    // Operações de arquivo
    bool carregar_pacotes_locais();
    bool salvar_pacotes_locais();
    bool baixar_pacote(const PacoteBR& pacote, const std::string& destino);
    bool extrair_pacote(const std::string& caminho_arquivo, const std::string& destino);
    
    // Operações de rede
    std::string fazer_requisicao_http(const std::string& url);
    bool baixar_registro_remoto();
    
    // Validação
    bool validar_manifesto(const std::string& caminho);
    std::string calcular_checksum(const std::string& caminho_arquivo);
    
    // Utilitários
    std::string expandir_caminho_usuario(const std::string& caminho);
    bool criar_diretorio(const std::string& caminho);
};

#endif // GERENCIADOR_PACOTES_H
