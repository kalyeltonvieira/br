// Implementação do Compilador BR para Bytecode BRC1
#include "compilador_bytecode.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstring>

CompiladorBytecode::CompiladorBytecode() 
    : contador_variaveis(0), permissoes(0) {
    bytecode.clear();
    constantes.clear();
    funcoes.clear();
    classes.clear();
    tabela_variaveis.clear();
    erros.clear();
}

bool CompiladorBytecode::compilar(const std::string& codigo_br) {
    bytecode.clear();
    constantes.clear();
    funcoes.clear();
    classes.clear();
    tabela_variaveis.clear();
    contador_variaveis = 0;
    erros.clear();
    
    return parsear_codigo(codigo_br);
}

bool CompiladorBytecode::compilar_arquivo(const std::string& caminho_arquivo) {
    std::ifstream arquivo(caminho_arquivo);
    if (!arquivo.is_open()) {
        adicionar_erro("Não foi possível abrir o arquivo: " + caminho_arquivo);
        return false;
    }
    
    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    std::string codigo = buffer.str();
    arquivo.close();
    
    return compilar(codigo);
}

bool CompiladorBytecode::salvar_bytecode(const std::string& caminho_saida) {
    std::ofstream arquivo(caminho_saida, std::ios::binary);
    if (!arquivo.is_open()) {
        adicionar_erro("Não foi possível criar o arquivo de saída: " + caminho_saida);
        return false;
    }
    
    // Escrever cabeçalho
    CabecalhoBRC cabecalho;
    cabecalho.tamanho_constantes = constantes.size();
    cabecalho.tamanho_funcoes = funcoes.size();
    cabecalho.tamanho_classes = classes.size();
    cabecalho.tamanho_codigo = bytecode.size();
    cabecalho.flags_permissoes = permissoes;
    
    arquivo.write(reinterpret_cast<const char*>(&cabecalho), sizeof(CabecalhoBRC));
    
    // Escrever constantes
    uint32_t num_constantes = constantes.size();
    arquivo.write(reinterpret_cast<const char*>(&num_constantes), sizeof(uint32_t));
    
    for (const auto& constante : constantes) {
        arquivo.write(reinterpret_cast<const char*>(&constante.tipo), sizeof(TipoConstante));
        
        switch (constante.tipo) {
            case CONST_INTEIRO:
                arquivo.write(reinterpret_cast<const char*>(&constante.valor.inteiro), sizeof(int64_t));
                break;
            case CONST_DECIMAL:
                arquivo.write(reinterpret_cast<const char*>(&constante.valor.decimal), sizeof(double));
                break;
            case CONST_TEXTO: {
                uint32_t tamanho = constante.texto.size();
                arquivo.write(reinterpret_cast<const char*>(&tamanho), sizeof(uint32_t));
                arquivo.write(constante.texto.c_str(), tamanho);
                break;
            }
            case CONST_BOOLEANO:
                arquivo.write(reinterpret_cast<const char*>(&constante.booleano), sizeof(bool));
                break;
            case CONST_NULO:
                break;
        }
    }
    
    // Escrever funções
    uint32_t num_funcoes = funcoes.size();
    arquivo.write(reinterpret_cast<const char*>(&num_funcoes), sizeof(uint32_t));
    
    for (const auto& [nome, funcao] : funcoes) {
        escrever_string(nome);
        uint32_t num_params = funcao.parametros.size();
        arquivo.write(reinterpret_cast<const char*>(&num_params), sizeof(uint32_t));
        
        for (const auto& param : funcao.parametros) {
            escrever_string(param);
        }
        
        arquivo.write(reinterpret_cast<const char*>(&funcao.endereco), sizeof(int));
        arquivo.write(reinterpret_cast<const char*>(&funcao.tamanho), sizeof(int));
    }
    
    // Escrever classes
    uint32_t num_classes = classes.size();
    arquivo.write(reinterpret_cast<const char*>(&num_classes), sizeof(uint32_t));
    
    for (const auto& [nome, classe] : classes) {
        escrever_string(nome);
        escrever_string(classe.classe_mae);
        
        uint32_t num_campos = classe.campos.size();
        arquivo.write(reinterpret_cast<const char*>(&num_campos), sizeof(uint32_t));
        
        for (const auto& campo : classe.campos) {
            escrever_string(campo);
        }
        
        uint32_t num_metodos = classe.metodos.size();
        arquivo.write(reinterpret_cast<const char*>(&num_metodos), sizeof(uint32_t));
        
        for (const auto& [metodo_nome, metodo] : classe.metodos) {
            escrever_string(metodo_nome);
            uint32_t num_params = metodo.parametros.size();
            arquivo.write(reinterpret_cast<const char*>(&num_params), sizeof(uint32_t));
            
            for (const auto& param : metodo.parametros) {
                escrever_string(param);
            }
            
            arquivo.write(reinterpret_cast<const char*>(&metodo.endereco), sizeof(int));
            arquivo.write(reinterpret_cast<const char*>(&metodo.tamanho), sizeof(int));
        }
    }
    
    // Escrever bytecode
    uint32_t tamanho_bytecode = bytecode.size();
    arquivo.write(reinterpret_cast<const char*>(&tamanho_bytecode), sizeof(uint32_t));
    arquivo.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    
    arquivo.close();
    return true;
}

bool CompiladorBytecode::carregar_bytecode(const std::string& caminho_entrada) {
    std::ifstream arquivo(caminho_entrada, std::ios::binary);
    if (!arquivo.is_open()) {
        adicionar_erro("Não foi possível abrir o arquivo: " + caminho_entrada);
        return false;
    }
    
    // Ler cabeçalho
    CabecalhoBRC cabecalho;
    arquivo.read(reinterpret_cast<char*>(&cabecalho), sizeof(CabecalhoBRC));
    
    // Verificar magic number
    if (cabecalho.magic[0] != 'B' || cabecalho.magic[1] != 'R' || 
        cabecalho.magic[2] != 'C' || cabecalho.magic[3] != '1') {
        adicionar_erro("Arquivo não é um bytecode BRC1 válido");
        return false;
    }
    
    // Ler constantes
    uint32_t num_constantes;
    arquivo.read(reinterpret_cast<char*>(&num_constantes), sizeof(uint32_t));
    
    constantes.resize(num_constantes);
    for (uint32_t i = 0; i < num_constantes; i++) {
        arquivo.read(reinterpret_cast<char*>(&constantes[i].tipo), sizeof(TipoConstante));
        
        switch (constantes[i].tipo) {
            case CONST_INTEIRO:
                arquivo.read(reinterpret_cast<char*>(&constantes[i].valor.inteiro), sizeof(int64_t));
                break;
            case CONST_DECIMAL:
                arquivo.read(reinterpret_cast<char*>(&constantes[i].valor.decimal), sizeof(double));
                break;
            case CONST_TEXTO: {
                uint32_t tamanho;
                arquivo.read(reinterpret_cast<char*>(&tamanho), sizeof(uint32_t));
                constantes[i].texto.resize(tamanho);
                arquivo.read(&constantes[i].texto[0], tamanho);
                break;
            }
            case CONST_BOOLEANO:
                arquivo.read(reinterpret_cast<char*>(&constantes[i].booleano), sizeof(bool));
                break;
            case CONST_NULO:
                break;
        }
    }
    
    // Ler funções
    uint32_t num_funcoes;
    arquivo.read(reinterpret_cast<char*>(&num_funcoes), sizeof(uint32_t));
    
    for (uint32_t i = 0; i < num_funcoes; i++) {
        FuncaoBytecode funcao;
        funcao.nome = ler_string_arquivo(arquivo);
        
        uint32_t num_params;
        arquivo.read(reinterpret_cast<char*>(&num_params), sizeof(uint32_t));
        
        for (uint32_t j = 0; j < num_params; j++) {
            funcao.parametros.push_back(ler_string_arquivo(arquivo));
        }
        
        arquivo.read(reinterpret_cast<char*>(&funcao.endereco), sizeof(int));
        arquivo.read(reinterpret_cast<char*>(&funcao.tamanho), sizeof(int));
        
        funcoes[funcao.nome] = funcao;
    }
    
    // Ler classes
    uint32_t num_classes;
    arquivo.read(reinterpret_cast<char*>(&num_classes), sizeof(uint32_t));
    
    for (uint32_t i = 0; i < num_classes; i++) {
        ClasseBytecode classe;
        classe.nome = ler_string_arquivo(arquivo);
        classe.classe_mae = ler_string_arquivo(arquivo);
        
        uint32_t num_campos;
        arquivo.read(reinterpret_cast<char*>(&num_campos), sizeof(uint32_t));
        
        for (uint32_t j = 0; j < num_campos; j++) {
            classe.campos.push_back(ler_string_arquivo(arquivo));
        }
        
        uint32_t num_metodos;
        arquivo.read(reinterpret_cast<char*>(&num_metodos), sizeof(uint32_t));
        
        for (uint32_t j = 0; j < num_metodos; j++) {
            FuncaoBytecode metodo;
            metodo.nome = ler_string_arquivo(arquivo);
            
            uint32_t num_params;
            arquivo.read(reinterpret_cast<char*>(&num_params), sizeof(uint32_t));
            
            for (uint32_t k = 0; k < num_params; k++) {
                metodo.parametros.push_back(ler_string_arquivo(arquivo));
            }
            
            arquivo.read(reinterpret_cast<char*>(&metodo.endereco), sizeof(int));
            arquivo.read(reinterpret_cast<char*>(&metodo.tamanho), sizeof(int));
            
            classe.metodos[metodo.nome] = metodo;
        }
        
        classes[classe.nome] = classe;
    }
    
    // Ler bytecode
    uint32_t tamanho_bytecode;
    arquivo.read(reinterpret_cast<char*>(&tamanho_bytecode), sizeof(uint32_t));
    
    bytecode.resize(tamanho_bytecode);
    arquivo.read(reinterpret_cast<char*>(bytecode.data()), tamanho_bytecode);
    
    arquivo.close();
    return true;
}

void CompiladorBytecode::definir_permissao_internet(bool habilitar) {
    if (habilitar) permissoes |= 0x01;
    else permissoes &= ~0x01;
}

void CompiladorBytecode::definir_permissao_arquivos(bool habilitar) {
    if (habilitar) permissoes |= 0x02;
    else permissoes &= ~0x02;
}

void CompiladorBytecode::definir_permissao_rede(bool habilitar) {
    if (habilitar) permissoes |= 0x04;
    else permissoes &= ~0x04;
}

void CompiladorBytecode::definir_permissao_banco(bool habilitar) {
    if (habilitar) permissoes |= 0x08;
    else permissoes &= ~0x08;
}

// Métodos privados de escrita
void CompiladorBytecode::escrever_uint8(uint8_t valor) {
    bytecode.push_back(valor);
}

void CompiladorBytecode::escrever_uint16(uint16_t valor) {
    bytecode.push_back((valor >> 8) & 0xFF);
    bytecode.push_back(valor & 0xFF);
}

void CompiladorBytecode::escrever_uint32(uint32_t valor) {
    bytecode.push_back((valor >> 24) & 0xFF);
    bytecode.push_back((valor >> 16) & 0xFF);
    bytecode.push_back((valor >> 8) & 0xFF);
    bytecode.push_back(valor & 0xFF);
}

void CompiladorBytecode::escrever_int32(int32_t valor) {
    escrever_uint32(static_cast<uint32_t>(valor));
}

void CompiladorBytecode::escrever_int64(int64_t valor) {
    bytecode.push_back((valor >> 56) & 0xFF);
    bytecode.push_back((valor >> 48) & 0xFF);
    bytecode.push_back((valor >> 40) & 0xFF);
    bytecode.push_back((valor >> 32) & 0xFF);
    bytecode.push_back((valor >> 24) & 0xFF);
    bytecode.push_back((valor >> 16) & 0xFF);
    bytecode.push_back((valor >> 8) & 0xFF);
    bytecode.push_back(valor & 0xFF);
}

void CompiladorBytecode::escrever_double(double valor) {
    uint64_t* p = reinterpret_cast<uint64_t*>(&valor);
    escrever_uint64(*p);
}

void CompiladorBytecode::escrever_uint64(uint64_t valor) {
    bytecode.push_back((valor >> 56) & 0xFF);
    bytecode.push_back((valor >> 48) & 0xFF);
    bytecode.push_back((valor >> 40) & 0xFF);
    bytecode.push_back((valor >> 32) & 0xFF);
    bytecode.push_back((valor >> 24) & 0xFF);
    bytecode.push_back((valor >> 16) & 0xFF);
    bytecode.push_back((valor >> 8) & 0xFF);
    bytecode.push_back(valor & 0xFF);
}

void CompiladorBytecode::escrever_string(const std::string& texto) {
    escrever_uint32(texto.size());
    for (char c : texto) {
        bytecode.push_back(static_cast<uint8_t>(c));
    }
}

void CompiladorBytecode::escrever_opcode(OpcodeBR opcode) {
    bytecode.push_back(static_cast<uint8_t>(opcode));
}

// Gerenciamento de constantes
int CompiladorBytecode::adicionar_constante(const ConstanteBR& constante) {
    constantes.push_back(constante);
    return constantes.size() - 1;
}

int CompiladorBytecode::buscar_constante(const ConstanteBR& constante) {
    for (size_t i = 0; i < constantes.size(); i++) {
        if (constantes[i].tipo == constante.tipo) {
            switch (constante.tipo) {
                case CONST_INTEIRO:
                    if (constantes[i].valor.inteiro == constante.valor.inteiro)
                        return static_cast>(i);
                    break;
                case CONST_DECIMAL:
                    if (constantes[i].valor.decimal == constante.valor.decimal)
                        return static_cast>(i);
                    break;
                case CONST_TEXTO:
                    if (constantes[i].texto == constante.texto)
                        return static_cast>(i);
                    break;
                case CONST_BOOLEANO:
                    if (constantes[i].booleano == constante.booleano)
                        return static_cast>(i);
                    break;
                case CONST_NULO:
                    return static_cast>(i);
            }
        }
    }
    return adicionar_constante(constante);
}

// Gerenciamento de variáveis
int CompiladorBytecode::adicionar_variavel(const std::string& nome) {
    if (tabela_variaveis.find(nome) == tabela_variaveis.end()) {
        tabela_variaveis[nome] = contador_variaveis++;
    }
    return tabela_variaveis[nome];
}

int CompiladorBytecode::buscar_variavel(const std::string& nome) {
    auto it = tabela_variaveis.find(nome);
    if (it != tabela_variaveis.end()) {
        return it->second;
    }
    return adicionar_variavel(nome);
}

// Parser simplificado
bool CompiladorBytecode::parsear_codigo(const std::string& codigo) {
    std::stringstream ss(codigo);
    std::string linha;
    
    while (std::getline(ss, linha)) {
        linha = trim(linha);
        
        // Ignorar comentários e linhas vazias
        if (linha.empty() || linha[0] == '#') continue;
        
        // Tentar parsear diferentes tipos de linha
        if (!parsear_declaracao(linha) &&
            !parsear_atribuicao(linha) &&
            !parsear_mostre(linha) &&
            !parsear_se(linha) &&
            !parsear_enquanto(linha) &&
            !parsear_funcao(linha) &&
            !parsear_classe(linha) &&
            !parsear_importar(linha)) {
            adicionar_erro("Linha não reconhecida: " + linha);
        }
    }
    
    return erros.empty();
}

bool CompiladorBytecode::parsear_declaracao(const std::string& linha) {
    // "nome é valor"
    size_t pos = linha.find(" é ");
    if (pos == std::string::npos) return false;
    
    std::string nome = trim(linha.substr(0, pos));
    std::string valor_str = trim(linha.substr(pos + 3));
    
    // Parsear valor
    ConstanteBR constante;
    
    // Verificar se é número inteiro
    try {
        int64_t inteiro = std::stoll(valor_str);
        constante = ConstanteBR::criarInteiro(inteiro);
    } catch (...) {
        // Verificar se é decimal
        try {
            double decimal = std::stod(valor_str);
            constante = ConstanteBR::criarDecimal(decimal);
        } catch (...) {
            // Verificar se é texto (entre aspas)
            if (valor_str.size() >= 2 && valor_str[0] == '"' && valor_str.back() == '"') {
                constante = ConstanteBR::criarTexto(valor_str.substr(1, valor_str.size() - 2));
            }
            // Verificar se é booleano
            else if (valor_str == "verdadeiro") {
                constante = ConstanteBR::criarBooleano(true);
            }
            else if (valor_str == "falso") {
                constante = ConstanteBR::criarBooleano(false);
            }
            else if (valor_str == "nulo") {
                constante = ConstanteBR::criarNulo();
            }
            else {
                // Tratar como texto sem aspas
                constante = ConstanteBR::criarTexto(valor_str);
            }
        }
    }
    
    int idx_const = buscar_constante(constante);
    int idx_var = adicionar_variavel(nome);
    
    escrever_opcode(OP_EMPILHAR_CONST);
    escrever_uint32(idx_const);
    escrever_opcode(OP_DEFINIR_VAR);
    escrever_uint32(idx_var);
    
    return true;
}

bool CompiladorBytecode::parsear_atribuicao(const std::string& linha) {
    // "nome é valor" (já tratado em declaracao)
    // "nome = valor"
    size_t pos = linha.find(" = ");
    if (pos == std::string::npos) return false;
    
    std::string nome = trim(linha.substr(0, pos));
    std::string valor_str = trim(linha.substr(pos + 3));
    
    // Similar à declaração, mas variável já existe
    ConstanteBR constante = ConstanteBR::criarTexto(valor_str);
    int idx_const = buscar_constante(constante);
    int idx_var = buscar_variavel(nome);
    
    escrever_opcode(OP_EMPILHAR_CONST);
    escrever_uint32(idx_const);
    escrever_opcode(OP_DEFINIR_VAR);
    escrever_uint32(idx_var);
    
    return true;
}

bool CompiladorBytecode::parsear_mostre(const std::string& linha) {
    // "mostre expressao"
    if (!inicia_com(linha, "mostre ")) return false;
    
    std::string expr = trim(linha.substr(7));
    
    // Se for uma constante simples
    ConstanteBR constante = ConstanteBR::criarTexto(expr);
    int idx_const = buscar_constante(constante);
    
    escrever_opcode(OP_EMPILHAR_CONST);
    escrever_uint32(idx_const);
    escrever_opcode(OP_MOSTRE);
    
    return true;
}

bool CompiladorBytecode::parsear_se(const std::string& linha) {
    // "se condição então" (simplificado)
    if (!inicia_com(linha, "se ")) return false;
    
    // Parser completo de condicionais requer mais contexto
    // Por enquanto, apenas marcamos
    escrever_opcode(OP_SALTO_SE_FALSO);
    escrever_uint32(0); // Placeholder para endereço
    
    return true;
}

bool CompiladorBytecode::parsear_enquanto(const std::string& linha) {
    // "enquanto condição" (simplificado)
    if (!inicia_com(linha, "enquanto ")) return false;
    
    // Parser completo de loops requer mais contexto
    escrever_opcode(OP_SALTO);
    escrever_uint32(0); // Placeholder para endereço
    
    return true;
}

bool CompiladorBytecode::parsear_funcao(const std::string& linha) {
    // "função nome recebendo params"
    if (!inicia_com(linha, "função ") && !inicia_com(linha, "funcao ")) return false;
    
    size_t pos_func = linha.find("função ");
    if (pos_func == std::string::npos) pos_func = linha.find("funcao ");
    
    std::string resto = linha.substr(pos_func + 7);
    size_t pos_recebendo = resto.find(" recebendo ");
    
    if (pos_recebendo == std::string::npos) return false;
    
    std::string nome = trim(resto.substr(0, pos_recebendo));
    std::string params_str = trim(resto.substr(pos_recebendo + 10));
    
    FuncaoBytecode funcao;
    funcao.nome = nome;
    funcao.endereco = bytecode.size();
    
    // Parsear parâmetros
    auto params = dividir(params_str, ' ');
    for (const auto& param : params) {
        if (!param.empty() && param != "e") {
            funcao.parametros.push_back(param);
        }
    }
    
    funcoes[nome] = funcao;
    
    return true;
}

bool CompiladorBytecode::parsear_classe(const std::string& linha) {
    // "classe Nome"
    if (!inicia_com(linha, "classe ")) return false;
    
    std::string nome = trim(linha.substr(7));
    
    ClasseBytecode classe;
    classe.nome = nome;
    classes[nome] = classe;
    
    return true;
}

bool CompiladorBytecode::parsear_importar(const std::string& linha) {
    // "importar modulo"
    if (!inicia_com(linha, "importar ")) return false;
    
    std::string modulo = trim(linha.substr(9));
    
    ConstanteBR constante = ConstanteBR::criarTexto(modulo);
    int idx_const = buscar_constante(constante);
    
    escrever_opcode(OP_EMPILHAR_CONST);
    escrever_uint32(idx_const);
    escrever_opcode(OP_IMPORTAR);
    
    return true;
}

// Utilitários
void CompiladorBytecode::adicionar_erro(const std::string& erro) {
    erros.push_back(erro);
}

std::string CompiladorBytecode::trim(const std::string& s) const {
    size_t inicio = s.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fim = s.find_last_not_of(" \t\r\n");
    return s.substr(inicio, fim - inicio + 1);
}

std::vector<std::string> CompiladorBytecode::dividir(const std::string& s, char delim) const {
    std::vector<std::string> resultado;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        resultado.push_back(trim(item));
    }
    return resultado;
}

bool CompiladorBytecode::inicia_com(const std::string& s, const std::string& prefixo) const {
    if (s.size() < prefixo.size()) return false;
    return s.substr(0, prefixo.size()) == prefixo;
}

std::string CompiladorBytecode::ler_string_arquivo(std::ifstream& arquivo) {
    uint32_t tamanho;
    arquivo.read(reinterpret_cast<char*>(&tamanho), sizeof(uint32_t));
    
    std::string resultado;
    resultado.resize(tamanho);
    arquivo.read(&resultado[0], tamanho);
    
    return resultado;
}
