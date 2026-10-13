// Compilador Cruzado (Cross-Compiler) para BR v2.0
// Compila código BR para diferentes plataformas e arquiteturas
#ifndef COMPILADOR_CRUZADO_H
#define COMPILADOR_CRUZADO_H

#include <string>
#include <vector>
#include <map>

// Plataformas alvo
enum PlataformaAlvo {
    PLATAFORMA_WINDOWS_X64,
    PLATAFORMA_WINDOWS_ARM64,
    PLATAFORMA_LINUX_X64,
    PLATAFORMA_LINUX_ARM64,
    PLATAFORMA_MACOS_X64,
    PLATAFORMA_MACOS_ARM64,
    PLATAFORMA_ANDROID_ARM64,
    PLATAFORMA_IOS_ARM64,
    PLATAFORMA_WASM32,
    PLATAFORMA_WASM64
};

// Arquiteturas
enum Arquitetura {
    ARQ_X86_64,
    ARQ_ARM64,
    ARQ_RISCV64,
    ARQ_WASM32,
    ARQ_WASM64
};

// Formatos de executável
enum FormatExecutavel {
    EXE_WINDOWS,
    ELF_LINUX,
    MACHO_MACOS,
    WASM,
    BIN_BRUTO
};

// Configuração de compilação cruzada
struct ConfiguracaoCruzada {
    PlataformaAlvo plataforma;
    Arquitetura arquitetura;
    FormatExecutavel formato;
    bool otimizar;
    int nivel_otimizacao; // 0-3
    bool depurar;
    bool linkar_estaticamente;
    std::string toolchain_prefixo;
    std::string sysroot;
    std::vector<std::string> bibliotecas;
    std::vector<std::string> flags_compilador;
    std::vector<std::string> flags_linker;
    
    ConfiguracaoCruzada() 
        : plataforma(PLATAFORMA_LINUX_X64),
          arquitetura(ARQ_X86_64),
          formato(ELF_LINUX),
          otimizar(true),
          nivel_otimizacao(2),
          depurar(false),
          linkar_estaticamente(false) {}
};

// Informação sobre a plataforma atual
struct InfoPlataforma {
    PlataformaAlvo plataforma;
    Arquitetura arquitetura;
    std::string sistema_operacional;
    std::string compilador_padrao;
    bool little_endian;
    int tamanho_ponteiro;
    
    InfoPlataforma() : little_endian(true), tamanho_ponteiro(8) {}
};

// Compilador Cruzado
class CompiladorCruzado {
private:
    ConfiguracaoCruzada config;
    InfoPlataforma plataforma_atual;
    std::map<std::string, std::string> toolchains;
    
public:
    CompiladorCruzado();
    
    // Configuração
    void definir_configuracao(const ConfiguracaoCruzada& cfg);
    ConfiguracaoCruzada obter_configuracao() const;
    
    // Detectar plataforma atual
    InfoPlataforma detectar_plataforma_atual();
    InfoPlataforma obter_plataforma_atual() const;
    
    // Configurar toolchains
    void adicionar_toolchain(const std::string& nome, const std::string& caminho);
    bool configurar_toolchain_automaticamente(PlataformaAlvo plataforma);
    
    // Compilação cruzada
    bool compilar_para_plataforma(const std::string& codigo_br, 
                                   const std::string& saida,
                                   PlataformaAlvo plataforma);
    
    bool compilar_para_arquitetura(const std::string& codigo_br,
                                   const std::string& saida,
                                   Arquitetura arq);
    
    bool compilar_para_wasm(const std::string& codigo_br,
                           const std::string& saida);
    
    // Geração de código intermediário
    std::string gerar_llvm_ir(const std::string& codigo_br);
    std::string gerar_assembly(const std::string& codigo_br, Arquitetura arq);
    
    // Validação
    bool validar_configuracao();
    std::vector<std::string> obter_avisos();
    
    // Utilitários
    static std::string nome_plataforma(PlataformaAlvo plat);
    static std::string nome_arquitetura(Arquitetura arq);
    static std::string nome_formato(FormatExecutavel fmt);
    
    static std::vector<PlataformaAlvo> plataformas_suportadas();
    static std::vector<Arquitetura> arquiteturas_suportadas();
    
private:
    std::string gerar_comando_compilacao(const std::string& entrada, const std::string& saida);
    std::string obter_compilador_cruzado();
    std::string obter_flags_compilador();
    std::string obter_flags_linker();
    
    bool executar_comando(const std::string& comando);
    std::vector<std::string> avisos;
};

// Gerenciador de Toolchains
class GerenciadorToolchains {
public:
    // Baixar toolchain
    static bool baixar_toolchain(PlataformaAlvo plataforma, const std::string& destino);
    
    // Verificar toolchain instalada
    static bool verificar_toolchain(PlataformaAlvo plataforma);
    
    // Listar toolchains disponíveis
    static std::vector<PlataformaAlvo> toolchains_disponiveis();
    
    // Configurar variáveis de ambiente
    static bool configurar_ambiente(PlataformaAlvo plataforma);
};

// Compilador para WebAssembly (WASM)
class CompiladorWASM {
public:
    static std::string compilar_br_para_wasm(const std::string& codigo_br);
    static std::string compilar_bytecode_para_wasm(const std::string& bytecode_brc);
    
    static std::string gerar_html_wrapper(const std::string& nome_modulo);
    static std::string gerar_js_glue(const std::string& nome_modulo);
    
private:
    static std::string traduzir_opcodes_para_wat(const std::vector<uint8_t>& bytecode);
};

#endif // COMPILADOR_CRUZADO_H
