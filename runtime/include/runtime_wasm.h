// Runtime WebAssembly para BR v2.0
// Executa código BR compilado para WebAssembly
#ifndef RUNTIME_WASM_H
#define RUNTIME_WASM_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

// Estado da VM WASM
enum EstadoWASM {
    WASM_NAO_INICIADO,
    WASM_COMPILANDO,
    WASM_PRONTO,
    WASM_EXECUTANDO,
    WASM_PAUSADO,
    WASM_TERMINADO,
    WASM_ERRO
};

// Módulo WASM
struct ModuloWASM {
    std::vector<uint8_t> bytes;
    std::map<std::string, size_t> exportacoes;
    std::map<std::string, std::function<void()>> imports;
    bool compilado;
    
    ModuloWASM() : compilado(false) {}
};

// Instância WASM
struct InstanciaWASM {
    std::shared_ptr<ModuloWASM> modulo;
    std::vector<uint8_t> memoria;
    std::map<std::string, std::function<void()>> funcoes_exportadas;
    EstadoWASM estado;
    
    InstanciaWASM() : estado(WASM_NAO_INICIADO) {}
};

// Runtime WASM para BR
class RuntimeWASM {
private:
    std::map<std::string, std::shared_ptr<ModuloWASM>> modulos;
    std::map<std::string, std::shared_ptr<InstanciaWASM>> instancias;
    std::map<std::string, std::function<void()>> funcoes_host;
    
public:
    RuntimeWASM();
    ~RuntimeWASM();
    
    // Carregar módulo
    std::string carregar_modulo(const std::string& nome, const std::string& caminho_wasm);
    std::string carregar_modulo_bytes(const std::string& nome, const std::vector<uint8_t>& bytes);
    
    // Compilar módulo
    bool compilar_modulo(const std::string& nome);
    
    // Criar instância
    std::string criar_instancia(const std::string& nome_instancia, const std::string& nome_modulo);
    
    // Executar
    bool executar(const std::string& nome_instancia, const std::string& funcao = "main");
    bool executar_com_args(const std::string& nome_instancia, const std::string& funcao,
                           const std::vector<int>& args);
    
    // Memória
    std::vector<uint8_t> obter_memoria(const std::string& nome_instancia);
    bool definir_memoria(const std::string& nome_instancia, const std::vector<uint8_t>& dados);
    uint8_t ler_memoria(const std::string& nome_instancia, size_t endereco);
    void escrever_memoria(const std::string& nome_instancia, size_t endereco, uint8_t valor);
    
    // Funções host (importações)
    void registrar_funcao_host(const std::string& nome, std::function<void()> funcao);
    void remover_funcao_host(const std::string& nome);
    
    // Estado
    EstadoWASM obter_estado(const std::string& nome_instancia);
    bool esta_pronto(const std::string& nome_instancia);
    
    // Utilitários
    std::vector<std::string> listar_modulos();
    std::vector<std::string> listar_instancias();
    void remover_modulo(const std::string& nome);
    void remover_instancia(const std::string& nome);
    void limpar_tudo();
    
    // Compilação BR para WASM
    static std::vector<uint8_t> compilar_br_para_wasm(const std::string& codigo_br);
    static std::vector<uint8_t> compilar_bytecode_para_wasm(const std::vector<uint8_t>& bytecode);
};

// Bridge JavaScript-WASM para navegador
class BridgeWASM {
public:
    // Inicializar runtime no navegador
    static std::string inicializar_runtime_js();
    
    // Carregar e executar módulo
    static std::string carregar_e_executar_js(const std::string& nome_modulo);
    
    // Comunicação entre JS e WASM
    static std::string criar_bridge_js(const std::string& nome_modulo);
    
    // Expor funções BR para JavaScript
    static std::string gerar_bindings_js(const std::vector<std::string>& funcoes);
};

// Compilador BR para WASM
class CompiladorBR_WASM {
public:
    // Compilar código BR para WASM
    static std::vector<uint8_t> compilar(const std::string& codigo_br);
    
    // Compilar bytecode BRC para WASM
    static std::vector<uint8_t> compilar_bytecode(const std::vector<uint8_t>& bytecode);
    
    // Gerar código Wat (WebAssembly Text)
    static std::string gerar_wat(const std::string& codigo_br);
    
private:
    // Tradução de opcodes BR para instruções WASM
    static uint8_t traduzir_opcode(uint8_t opcode_br);
    
    // Gerar header WASM
    static std::vector<uint8_t> gerar_header();
    
    // Gerar seção de tipos
    static std::vector<uint8_t> gerar_secao_tipos();
    
    // Gerar seção de funções
    static std::vector<uint8_t> gerar_secao_funcoes();
    
    // Gerar seção de exportações
    static std::vector<uint8_t> gerar_secao_exportacoes();
    
    // Gerar seção de código
    static std::vector<uint8_t> gerar_secao_codigo(const std::string& codigo_br);
};

// Sandbox de segurança para WASM
class SandboxWASM {
public:
    // Configurar permissões
    void permitir_acesso_rede(bool permitir);
    void permitir_acesso_arquivos(bool permitir);
    void permitir_acesso_dom(bool permitir);
    
    // Validar módulo
    bool validar_modulo(const std::vector<uint8_t>& bytes);
    
    // Verificar se módulo é seguro
    bool verificar_seguranca(const std::vector<uint8_t>& bytes);
    
private:
    bool permitir_rede;
    bool permitir_arquivos;
    bool permitir_dom;
};

#endif // RUNTIME_WASM_H
