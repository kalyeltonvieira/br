// Implementação do Gerenciador de Pacotes BR
#include "gerenciador_pacotes.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <random>

namespace fs = std::filesystem;

GerenciadorPacotes::GerenciadorPacotes() {
    // Carregar configurações padrão
    config.diretorio_cache = expandir_caminho_usuario(config.diretorio_cache);
    config.diretorio_instalacao = expandir_caminho_usuario(config.diretorio_instalacao);
    
    // Criar diretórios se não existirem
    criar_diretorio(config.diretorio_cache);
    criar_diretorio(config.diretorio_instalacao);
    
    // Carregar pacotes locais
    carregar_pacotes_locais();
}

GerenciadorPacotes::~GerenciadorPacotes() {
    salvar_pacotes_locais();
}

void GerenciadorPacotes::definir_configuracao(const ConfiguracaoPacotes& cfg) {
    config = cfg;
    config.diretorio_cache = expandir_caminho_usuario(config.diretorio_cache);
    config.diretorio_instalacao = expandir_caminho_usuario(config.diretorio_instalacao);
    
    criar_diretorio(config.diretorio_cache);
    criar_diretorio(config.diretorio_instalacao);
}

ConfiguracaoPacotes GerenciadorPacotes::obter_configuracao() const {
    return config;
}

bool GerenciadorPacotes::instalar(const std::string& nome_pacote, const std::string& versao) {
    std::cout << "[Gerenciador] Instalando pacote: " << nome_pacote;
    if (!versao.empty()) {
        std::cout << " versão " << versao;
    }
    std::cout << std::endl;
    
    // Verificar se já está instalado
    if (pacotes_locais.find(nome_pacote) != pacotes_locais.end()) {
        std::cout << "[Gerenciador] Pacote já instalado" << std::endl;
        return true;
    }
    
    // Sincronizar registro remoto
    if (!sincronizar_registro()) {
        std::cerr << "[Gerenciador] Falha ao sincronizar registro remoto" << std::endl;
    }
    
    // Buscar pacote no registro
    RegistroPacote registro;
    bool encontrado = false;
    
    for (auto& [id, reg] : registro_remoto) {
        if (reg.pacote.nome == nome_pacote) {
            if (versao.empty() || reg.pacote.versao == versao) {
                registro = reg;
                encontrado = true;
                break;
            }
        }
    }
    
    if (!encontrado) {
        std::cerr << "[Gerenciador] Pacote não encontrado: " << nome_pacote << std::endl;
        return false;
    }
    
    // Resolver dependências
    auto dependencias = resolver_dependencias(nome_pacote);
    for (const auto& dep : dependencias) {
        if (pacotes_locais.find(dep) == pacotes_locais.end()) {
            std::cout << "[Gerenciador] Instalando dependência: " << dep << std::endl;
            if (!instalar(dep)) {
                std::cerr << "[Gerenciador] Falha ao instalar dependência: " << dep << std::endl;
                return false;
            }
        }
    }
    
    // Baixar pacote
    std::string caminho_cache = config.diretorio_cache + "/" + nome_pacote + "-" + registro.pacote.versao + ".brp";
    
    if (!baixar_pacote(registro.pacote, caminho_cache)) {
        std::cerr << "[Gerenciador] Falha ao baixar pacote" << std::endl;
        return false;
    }
    
    // Verificar integridade
    if (config.verificar_checksum) {
        if (!verificar_integridade(registro.pacote)) {
            std::cerr << "[Gerenciador] Falha na verificação de integridade" << std::endl;
            return false;
        }
    }
    
    // Extrair pacote
    std::string caminho_instalacao = config.diretorio_instalacao + "/" + nome_pacote;
    
    if (!extrair_pacote(caminho_cache, caminho_instalacao)) {
        std::cerr << "[Gerenciador] Falha ao extrair pacote" << std::endl;
        return false;
    }
    
    // Adicionar aos pacotes locais
    pacotes_locais[nome_pacote] = registro.pacote;
    salvar_pacotes_locais();
    
    std::cout << "[Gerenciador] Pacote instalado com sucesso: " << nome_pacote << std::endl;
    return true;
}

bool GerenciadorPacotes::desinstalar(const std::string& nome_pacote) {
    std::cout << "[Gerenciador] Desinstalando pacote: " << nome_pacote << std::endl;
    
    auto it = pacotes_locais.find(nome_pacote);
    if (it == pacotes_locais.end()) {
        std::cerr << "[Gerenciador] Pacote não instalado: " << nome_pacote << std::endl;
        return false;
    }
    
    // Verificar se outros pacotes dependem deste
    for (const auto& [nome, pacote] : pacotes_locais) {
        if (nome != nome_pacote) {
            for (const auto& dep : pacote.dependencias) {
                if (dep == nome_pacote) {
                    std::cerr << "[Gerenciador] Pacote " << nome << " depende de " << nome_pacote << std::endl;
                    return false;
                }
            }
        }
    }
    
    // Remover arquivos
    std::string caminho_instalacao = config.diretorio_instalacao + "/" + nome_pacote;
    
    try {
        if (fs::exists(caminho_instalacao)) {
            fs::remove_all(caminho_instalacao);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Gerenciador] Erro ao remover arquivos: " << e.what() << std::endl;
        return false;
    }
    
    // Remover dos pacotes locais
    pacotes_locais.erase(it);
    salvar_pacotes_locais();
    
    std::cout << "[Gerenciador] Pacote desinstalado: " << nome_pacote << std::endl;
    return true;
}

bool GerenciadorPacotes::atualizar(const std::string& nome_pacote) {
    std::cout << "[Gerenciador] Atualizando pacote: " << nome_pacote << std::endl;
    
    auto it = pacotes_locais.find(nome_pacote);
    if (it == pacotes_locais.end()) {
        std::cerr << "[Gerenciador] Pacote não instalado: " << nome_pacote << std::endl;
        return false;
    }
    
    // Sincronizar registro
    sincronizar_registro();
    
    // Buscar versão mais recente
    RegistroPacote registro_mais_recente;
    bool encontrado = false;
    
    for (auto& [id, reg] : registro_remoto) {
        if (reg.pacote.nome == nome_pacote) {
            if (!encontrado || reg.pacote.versao > registro_mais_recente.pacote.versao) {
                registro_mais_recente = reg;
                encontrado = true;
            }
        }
    }
    
    if (!encontrado) {
        std::cerr << "[Gerenciador] Pacote não encontrado no registro" << std::endl;
        return false;
    }
    
    // Verificar se já está na versão mais recente
    if (it->second.versao == registro_mais_recente.pacote.versao) {
        std::cout << "[Gerenciador] Pacote já está na versão mais recente" << std::endl;
        return true;
    }
    
    // Desinstalar versão antiga e instalar nova
    if (!desinstalar(nome_pacote)) {
        return false;
    }
    
    return instalar(nome_pacote, registro_mais_recente.pacote.versao);
}

bool GerenciadorPacotes::atualizar_todos() {
    std::cout << "[Gerenciador] Atualizando todos os pacotes..." << std::endl;
    
    auto desatualizados = listar_desatualizados();
    
    if (desatualizados.empty()) {
        std::cout << "[Gerenciador] Todos os pacotes estão atualizados" << std::endl;
        return true;
    }
    
    bool todos_sucesso = true;
    for (const auto& reg : desatualizados) {
        if (!atualizar(reg.pacote.nome)) {
            todos_sucesso = false;
        }
    }
    
    return todos_sucesso;
}

std::vector<RegistroPacote> GerenciadorPacotes::buscar(const std::string& termo) {
    std::vector<RegistroPacote> resultados;
    
    std::string termo_lower = termo;
    std::transform(termo_lower.begin(), termo_lower.end(), termo_lower.begin(), ::tolower);
    
    for (const auto& [id, reg] : registro_remoto) {
        std::string nome_lower = reg.pacote.nome;
        std::transform(nome_lower.begin(), nome_lower.end(), nome_lower.begin(), ::tolower);
        
        std::string desc_lower = reg.pacote.descricao;
        std::transform(desc_lower.begin(), desc_lower.end(), desc_lower.begin(), ::tolower);
        
        if (nome_lower.find(termo_lower) != std::string::npos ||
            desc_lower.find(termo_lower) != std::string::npos) {
            resultados.push_back(reg);
        }
    }
    
    return resultados;
}

std::vector<RegistroPacote> GerenciadorPacotes::listar_instalados() {
    std::vector<RegistroPacote> instalados;
    
    for (const auto& [nome, pacote] : pacotes_locais) {
        RegistroPacote reg;
        reg.pacote = pacote;
        reg.id = nome;
        instalados.push_back(reg);
    }
    
    return instalados;
}

std::vector<RegistroPacote> GerenciadorPacotes::listar_disponiveis() {
    std::vector<RegistroPacote> disponiveis;
    
    for (const auto& [id, reg] : registro_remoto) {
        disponiveis.push_back(reg);
    }
    
    return disponiveis;
}

std::vector<RegistroPacote> GerenciadorPacotes::listar_desatualizados() {
    std::vector<RegistroPacote> desatualizados;
    
    for (const auto& [nome, pacote_local] : pacotes_locais) {
        for (const auto& [id, reg_remoto] : registro_remoto) {
            if (reg_remoto.pacote.nome == nome) {
                if (reg_remoto.pacote.versao > pacote_local.versao) {
                    desatualizados.push_back(reg_remoto);
                }
                break;
            }
        }
    }
    
    return desatualizados;
}

RegistroPacote GerenciadorPacotes::obter_info(const std::string& nome_pacote) {
    // Primeiro buscar nos locais
    auto it_local = pacotes_locais.find(nome_pacote);
    if (it_local != pacotes_locais.end()) {
        RegistroPacote reg;
        reg.pacote = it_local->second;
        reg.id = nome_pacote;
        return reg;
    }
    
    // Buscar no registro remoto
    for (const auto& [id, reg] : registro_remoto) {
        if (reg.pacote.nome == nome_pacote) {
            return reg;
        }
    }
    
    return RegistroPacote();
}

bool GerenciadorPacotes::sincronizar_registro() {
    std::cout << "[Gerenciador] Sincronizando registro remoto..." << std::endl;
    
    return baixar_registro_remoto();
}

bool GerenciadorPacotes::publicar_pacote(const std::string& caminho_manifesto) {
    std::cout << "[Gerenciador] Publicando pacote: " << caminho_manifesto << std::endl;
    
    // Validar manifesto
    if (!validar_manifesto(caminho_manifesto)) {
        std::cerr << "[Gerenciador] Manifesto inválido" << std::endl;
        return false;
    }
    
    // Ler manifesto
    std::ifstream arquivo(caminho_manifesto);
    if (!arquivo.is_open()) {
        std::cerr << "[Gerenciador] Não foi possível abrir manifesto" << std::endl;
        return false;
    }
    
    // Em produção, fazer upload para servidor
    std::cout << "[Gerenciador] Nota: Publicação requer servidor de registro configurado" << std::endl;
    
    return true;
}

bool GerenciadorPacotes::avaliar_pacote(const std::string& nome_pacote, float nota) {
    std::cout << "[Gerenciador] Avaliando pacote " << nome_pacote << " com nota " << nota << std::endl;
    
    // Em produção, enviar avaliação para servidor
    return true;
}

std::vector<std::string> GerenciadorPacotes::resolver_dependencias(const std::string& nome_pacote) {
    std::vector<std::string> dependencias;
    
    // Buscar no registro remoto
    for (const auto& [id, reg] : registro_remoto) {
        if (reg.pacote.nome == nome_pacote) {
            dependencias = reg.pacote.dependencias;
            break;
        }
    }
    
    return dependencias;
}

bool GerenciadorPacotes::verificar_dependencias(const std::string& nome_pacote) {
    auto dependencias = resolver_dependencias(nome_pacote);
    
    for (const auto& dep : dependencias) {
        if (pacotes_locais.find(dep) == pacotes_locais.end()) {
            return false;
        }
    }
    
    return true;
}

void GerenciadorPacotes::limpar_cache() {
    std::cout << "[Gerenciador] Limpando cache..." << std::endl;
    
    try {
        if (fs::exists(config.diretorio_cache)) {
            for (const auto& entrada : fs::directory_iterator(config.diretorio_cache)) {
                fs::remove_all(entrada.path());
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Gerenciador] Erro ao limpar cache: " << e.what() << std::endl;
    }
}

StatusInstalacao GerenciadorPacotes::verificar_status(const std::string& nome_pacote) {
    auto it = pacotes_locais.find(nome_pacote);
    if (it == pacotes_locais.end()) {
        return NAO_INSTALADO;
    }
    
    // Verificar se há versão mais recente
    for (const auto& [id, reg] : registro_remoto) {
        if (reg.pacote.nome == nome_pacote) {
            if (reg.pacote.versao > it->second.versao) {
                return DESATUALIZADO;
            }
            break;
        }
    }
    
    return INSTALADO;
}

std::string GerenciadorPacotes::obter_caminho_instalacao(const std::string& nome_pacote) {
    return config.diretorio_instalacao + "/" + nome_pacote;
}

bool GerenciadorPacotes::verificar_integridade(const PacoteBR& pacote) {
    std::string caminho_cache = config.diretorio_cache + "/" + pacote.nome + "-" + pacote.versao + ".brp";
    
    std::string checksum_calculado = calcular_checksum(caminho_cache);
    
    return checksum_calculado == pacote.checksum_sha256;
}

// Métodos privados
bool GerenciadorPacotes::carregar_pacotes_locais() {
    std::string caminho_arquivo = config.diretorio_instalacao + "/pacotes_locais.json";
    
    std::ifstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        return true; // Arquivo pode não existir ainda
    }
    
    // Parsear JSON (simplificado)
    // Em produção, usar biblioteca JSON real
    
    return true;
}

bool GerenciadorPacotes::salvar_pacotes_locais() {
    std::string caminho_arquivo = config.diretorio_instalacao + "/pacotes_locais.json";
    
    std::ofstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        return false;
    }
    
    // Escrever JSON (simplificado)
    arquivo << "{\n";
    
    bool primeiro = true;
    for (const auto& [nome, pacote] : pacotes_locais) {
        if (!primeiro) arquivo << ",\n";
        arquivo << "  \"" << nome << "\": {\n";
        arquivo << "    \"versao\": \"" << pacote.versao << "\",\n";
        arquivo << "    \"descricao\": \"" << pacote.descricao << "\"\n";
        arquivo << "  }";
        primeiro = false;
    }
    
    arquivo << "\n}\n";
    
    return true;
}

bool GerenciadorPacotes::baixar_pacote(const PacoteBR& pacote, const std::string& destino) {
    std::cout << "[Gerenciador] Baixando pacote de: " << pacote.repositorio << std::endl;
    
    // Em produção, usar libcurl ou similar para download
    // Por enquanto, apenas simulamos
    
    std::ofstream arquivo(destino);
    if (!arquivo.is_open()) {
        return false;
    }
    
    // Simular download
    arquivo << "conteudo_simulado_do_pacote";
    arquivo.close();
    
    return true;
}

bool GerenciadorPacotes::extrair_pacote(const std::string& caminho_arquivo, const std::string& destino) {
    std::cout << "[Gerenciador] Extraindo pacote para: " << destino << std::endl;
    
    // Criar diretório de destino
    criar_diretorio(destino);
    
    // Em produção, usar biblioteca de compressão (zlib, etc.)
    // Por enquanto, apenas criamos um arquivo de exemplo
    
    std::ofstream arquivo(destino + "/README.br");
    arquivo << "# Pacote " << destino << "\n";
    arquivo.close();
    
    return true;
}

std::string GerenciadorPacotes::fazer_requisicao_http(const std::string& url) {
    // Em produção, usar libcurl
    // Por enquanto, retorna string vazia
    return "";
}

bool GerenciadorPacotes::baixar_registro_remoto() {
    std::string url = config.url_registro + "/registro.json";
    
    std::string resposta = fazer_requisicao_http(url);
    
    if (resposta.empty()) {
        // Usar registro de exemplo
        RegistroPacote reg_exemplo;
        reg_exemplo.id = "exemplo";
        reg_exemplo.pacote.nome = "exemplo";
        reg_exemplo.pacote.versao = "1.0.0";
        reg_exemplo.pacote.descricao = "Pacote de exemplo";
        reg_exemplo.pacote.autor = "BR Team";
        reg_exemplo.pacote.repositorio = "https://github.com/brlang/exemplo";
        reg_exemplo.pacote.licenca = "MIT";
        reg_exemplo.downloads = 1000;
        reg_exemplo.avaliacao = 4.5f;
        reg_exemplo.verificado = true;
        
        registro_remoto["exemplo"] = reg_exemplo;
        
        return true;
    }
    
    // Parsear JSON da resposta
    // Em produção, usar biblioteca JSON
    
    return true;
}

bool GerenciadorPacotes::validar_manifesto(const std::string& caminho) {
    // Validar estrutura do manifesto
    // Em produção, validação completa
    return true;
}

std::string GerenciadorPacotes::calcular_checksum(const std::string& caminho_arquivo) {
    // Calcular SHA256
    // Em produção, usar OpenSSL ou similar
    // Por enquanto, retorna hash simulado
    return "checksum_simulado_sha256";
}

std::string GerenciadorPacotes::expandir_caminho_usuario(const std::string& caminho) {
    std::string resultado = caminho;
    
    // Expandir ~
    if (resultado.size() > 0 && resultado[0] == '~') {
        const char* home = std::getenv("HOME");
        if (!home) {
            home = std::getenv("USERPROFILE");
        }
        if (home) {
            resultado = home + resultado.substr(1);
        }
    }
    
    return resultado;
}

bool GerenciadorPacotes::criar_diretorio(const std::string& caminho) {
    try {
        if (!fs::exists(caminho)) {
            fs::create_directories(caminho);
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Gerenciador] Erro ao criar diretório: " << e.what() << std::endl;
        return false;
    }
}
