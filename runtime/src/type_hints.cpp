// Implementação do Sistema de Type Hints Opcionais para BR v2.0
#include "type_hints.h"
#include <sstream>
#include <algorithm>

// TipoHint
std::string TipoHint::para_string() const {
    std::stringstream ss;
    
    switch (tipo_base) {
        case TIPO_INTEIRO:
            ss << "inteiro";
            break;
        case TIPO_DECIMAL:
            ss << "decimal";
            break;
        case TIPO_TEXTO:
            ss << "texto";
            break;
        case TIPO_BOOLEANO:
            ss << "booleano";
            break;
        case TIPO_NULO:
            ss << "nulo";
            break;
        case TIPO_LISTA:
            ss << "lista<";
            if (tipo_elemento) {
                ss << tipo_elemento->para_string();
            } else {
                ss << "?";
            }
            ss << ">";
            break;
        case TIPO_REGISTRO:
            ss << "registro{";
            bool primeiro = true;
            for (const auto& [nome, tipo] : campos) {
                if (!primeiro) ss << ", ";
                ss << nome << ": " << tipo->para_string();
                primeiro = false;
            }
            ss << "}";
            break;
        case TIPO_FUNCAO:
            ss << "funcao(";
            for (size_t i = 0; i < parametros_tipo.size(); i++) {
                if (i > 0) ss << ", ";
                ss << parametros_tipo[i]->para_string();
            }
            ss << ") -> ";
            if (tipo_elemento) {
                ss << tipo_elemento->para_string();
            } else {
                ss << "?";
            }
            break;
        case TIPO_UNION:
            ss << "uniao(";
            for (size_t i = 0; i < parametros_tipo.size(); i++) {
                if (i > 0) ss << " | ";
                ss << parametros_tipo[i]->para_string();
            }
            ss << ")";
            break;
        case TIPO_INTERSECCAO:
            ss << "interseccao(";
            for (size_t i = 0; i < parametros_tipo.size(); i++) {
                if (i > 0) ss << " & ";
                ss << parametros_tipo[i]->para_string();
            }
            ss << ")";
            break;
        case TIPO_GENERICO:
            ss << nome_generico;
            break;
        case TIPO_QUALQUER:
            ss << "qualquer";
            break;
    }
    
    if (opcional) {
        ss << "?";
    }
    
    return ss.str();
}

bool TipoHint::compativel_com(const TipoHint& outro) const {
    // Se um dos tipos é QUALQUER, sempre compatível
    if (tipo_base == TIPO_QUALQUER || outro.tipo_base == TIPO_QUALQUER) {
        return true;
    }
    
    // Se ambos são o mesmo tipo base
    if (tipo_base == outro.tipo_base) {
        // Verificar opcional
        if (opcional && !outro.opcional) {
            // Opcional pode ser atribuído a não-opcional se não for nulo
            return true;
        }
        if (!opcional && outro.opcional) {
            // Não-opcional pode ser atribuído a opcional
            return true;
        }
        
        // Para tipos complexos, verificar recursivamente
        if (tipo_base == TIPO_LISTA) {
            if (tipo_elemento && outro.tipo_elemento) {
                return tipo_elemento->compativel_com(*outro.tipo_elemento);
            }
            return true;
        }
        
        if (tipo_base == TIPO_REGISTRO) {
            // Verificar se campos são compatíveis
            for (const auto& [nome, tipo] : campos) {
                auto it = outro.campos.find(nome);
                if (it != outro.campos.end()) {
                    if (!tipo->compativel_com(*it->second)) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        if (tipo_base == TIPO_FUNCAO) {
            // Verificar parâmetros e retorno
            if (parametros_tipo.size() != outro.parametros_tipo.size()) {
                return false;
            }
            for (size_t i = 0; i < parametros_tipo.size(); i++) {
                if (!parametros_tipo[i]->compativel_com(*outro.parametros_tipo[i])) {
                    return false;
                }
            }
            if (tipo_elemento && outro.tipo_elemento) {
                return tipo_elemento->compativel_com(*outro.tipo_elemento);
            }
            return true;
        }
        
        return true;
    }
    
    // NULO é compatível com qualquer tipo opcional
    if (tipo_base == TIPO_NULO && outro.opcional) {
        return true;
    }
    if (outro.tipo_base == TIPO_NULO && opcional) {
        return true;
    }
    
    // Verificar uniões
    if (tipo_base == TIPO_UNION) {
        for (const auto& tipo : parametros_tipo) {
            if (tipo->compativel_com(outro)) {
                return true;
            }
        }
        return false;
    }
    
    if (outro.tipo_base == TIPO_UNION) {
        for (const auto& tipo : outro.parametros_tipo) {
            if (compativel_com(*tipo)) {
                return true;
            }
        }
        return false;
    }
    
    return false;
}

// VerificadorTipos
VerificadorTipos::VerificadorTipos() {
    // Adicionar tipos básicos
}

void VerificadorTipos::adicionar_variavel(const AnotacaoVariavel& var) {
    variaveis[var.nome] = var;
}

void VerificadorTipos::adicionar_funcao(const AnotacaoFuncao& fun) {
    funcoes[fun.nome] = fun;
}

void VerificadorTipos::adicionar_classe(const AnotacaoClasse& cls) {
    classes[cls.nome] = cls;
}

bool VerificadorTipos::verificar_atribuicao(const std::string& nome_var, const TipoHint& tipo_valor) {
    auto it = variaveis.find(nome_var);
    if (it == variaveis.end()) {
        // Variável não existe, pode ser nova
        return true;
    }
    
    const TipoHint& tipo_esperado = it->second.tipo;
    
    if (!tipos_compativel(tipo_esperado, tipo_valor)) {
        std::stringstream ss;
        ss << "Tipo incompatível: esperado " << tipo_esperado.para_string()
           << ", recebido " << tipo_valor.para_string();
        adicionar_erro(ss.str());
        return false;
    }
    
    return true;
}

bool VerificadorTipos::verificar_chamada_funcao(const std::string& nome_fun,
                                                  const std::vector<TipoHint>& args) {
    auto it = funcoes.find(nome_fun);
    if (it == funcoes.end()) {
        // Função não conhecida, assumir correta
        return true;
    }
    
    const AnotacaoFuncao& fun = it->second;
    
    if (fun.parametros.size() != args.size()) {
        std::stringstream ss;
        ss << "Número de argumentos incorreto: esperado " << fun.parametros.size()
           << ", recebido " << args.size();
        adicionar_erro(ss.str());
        return false;
    }
    
    for (size_t i = 0; i < args.size(); i++) {
        const TipoHint& tipo_esperado = fun.parametros[i].tipo;
        const TipoHint& tipo_recebido = args[i];
        
        if (!tipos_compativel(tipo_esperado, tipo_recebido)) {
            std::stringstream ss;
            ss << "Argumento " << i << " tipo incompatível: esperado "
               << tipo_esperado.para_string() << ", recebido "
               << tipo_recebido.para_string();
            adicionar_erro(ss.str());
            return false;
        }
    }
    
    return true;
}

bool VerificadorTipos::verificar_retorno(const TipoHint& tipo_retorno) {
    // Em uma implementação completa, verificaríamos o tipo de retorno da função atual
    return true;
}

TipoHint VerificadorTipos::inferir_tipo(const std::string& expressao) {
    // Inferência simplificada
    if (expressao.empty()) {
        return TipoHint::qualquer();
    }
    
    // Literais
    if (expressao[0] == '"' || expressao[0] == '\'') {
        return TipoHint::texto();
    }
    
    // Números
    bool eh_numero = true;
    bool ponto_decimal = false;
    for (char c : expressao) {
        if (c == '.') {
            if (ponto_decimal) {
                eh_numero = false;
                break;
            }
            ponto_decimal = true;
        } else if (!isdigit(c)) {
            eh_numero = false;
            break;
        }
    }
    
    if (eh_numero) {
        if (ponto_decimal) {
            return TipoHint::decimal();
        }
        return TipoHint::inteiro();
    }
    
    // Palavras-chave booleanas
    if (expressao == "verdadeiro" || expressao == "falso") {
        return TipoHint::booleano();
    }
    
    if (expressao == "nulo") {
        return TipoHint::nulo();
    }
    
    // Variável conhecida
    auto it = variaveis.find(expressao);
    if (it != variaveis.end()) {
        return it->second.tipo;
    }
    
    return TipoHint::qualquer();
}

TipoHint VerificadorTipos::inferir_tipo_literal(const std::string& literal) {
    return inferir_tipo(literal);
}

void VerificadorTipos::limpar() {
    variaveis.clear();
    funcoes.clear();
    classes.clear();
    erros.clear();
}

void VerificadorTipos::adicionar_erro(const std::string& erro) {
    erros.push_back(erro);
}

bool VerificadorTipos::tipos_compativel(const TipoHint& esperado, const TipoHint& recebido) {
    return esperado.compativel_com(recebido);
}

// ParserTypeHints
TipoHint ParserTypeHints::parsear(const std::string& texto_tipo) {
    std::string tipo = normalizar(texto_tipo);
    
    // Tipos básicos
    if (tipo == "inteiro" || tipo == "int") {
        return TipoHint::inteiro();
    }
    if (tipo == "decimal" || tipo == "float" || tipo == "double") {
        return TipoHint::decimal();
    }
    if (tipo == "texto" || tipo == "string" || tipo == "str") {
        return TipoHint::texto();
    }
    if (tipo == "booleano" || tipo == "bool") {
        return TipoHint::booleano();
    }
    if (tipo == "nulo" || tipo == "null") {
        return TipoHint::nulo();
    }
    if (tipo == "qualquer" || tipo == "any") {
        return TipoHint::qualquer();
    }
    
    // Lista
    if (tipo.find("lista<") == 0 || tipo.find("list<") == 0) {
        size_t inicio = tipo.find('<');
        size_t fim = encontrar_fim_parentese(tipo, inicio);
        if (fim != std::string::npos) {
            std::string elemento = tipo.substr(inicio + 1, fim - inicio - 1);
            return TipoHint::lista(std::make_shared<TipoHint>(parsear(elemento)));
        }
    }
    
    // Opcional
    if (tipo.back() == '?') {
        std::string base = tipo.substr(0, tipo.size() - 1);
        return TipoHint::opcional(std::make_shared<TipoHint>(parsear(base)));
    }
    
    // Função
    if (tipo.find("funcao(") == 0 || tipo.find("func(") == 0) {
        size_t inicio_parentese = tipo.find('(');
        size_t fim_parentese = encontrar_fim_parentese(tipo, inicio_parentese);
        
        if (fim_parentese != std::string::npos) {
            size_t seta = tipo.find("->", fim_parentese);
            if (seta != std::string::npos) {
                std::string params_str = tipo.substr(inicio_parentese + 1, fim_parentese - inicio_parentese - 1);
                std::string retorno_str = tipo.substr(seta + 2);
                
                std::vector<std::shared_ptr<TipoHint>> params;
                if (!params_str.empty()) {
                    auto tipos_param = dividir_tipos(params_str);
                    for (const auto& t : tipos_param) {
                        params.push_back(std::make_shared<TipoHint>(parsear(t)));
                    }
                }
                
                return TipoHint::funcao(std::make_shared<TipoHint>(parsear(retorno_str)), params);
            }
        }
    }
    
    // Genérico
    if (isupper(tipo[0])) {
        return TipoHint::generico(tipo);
    }
    
    // Default: qualquer
    return TipoHint::qualquer();
}

std::string ParserTypeHints::normalizar(const std::string& texto) {
    std::string resultado = texto;
    
    // Remover espaços extras
    std::string::size_type pos = 0;
    while ((pos = resultado.find("  ", pos)) != std::string::npos) {
        resultado.replace(pos, 2, " ");
    }
    
    // Trim
    resultado.erase(0, resultado.find_first_not_of(" \t\r\n"));
    resultado.erase(resultado.find_last_not_of(" \t\r\n") + 1);
    
    return resultado;
}

size_t ParserTypeHints::encontrar_fim_parentese(const std::string& s, size_t inicio) {
    int profundidade = 0;
    char abertura = s[inicio];
    char fechamento = (abertura == '<') ? '>' : ')';
    
    for (size_t i = inicio; i < s.size(); i++) {
        if (s[i] == abertura) {
            profundidade++;
        } else if (s[i] == fechamento) {
            profundidade--;
            if (profundidade == 0) {
                return i;
            }
        }
    }
    
    return std::string::npos;
}

std::vector<std::string> ParserTypeHints::dividir_tipos(const std::string& s) {
    std::vector<std::string> resultado;
    std::string atual;
    int profundidade = 0;
    
    for (char c : s) {
        if (c == '<' || c == '(') {
            profundidade++;
            atual += c;
        } else if (c == '>' || c == ')') {
            profundidade--;
            atual += c;
        } else if (c == ',' && profundidade == 0) {
            resultado.push_back(atual);
            atual.clear();
        } else {
            atual += c;
        }
    }
    
    if (!atual.empty()) {
        resultado.push_back(atual);
    }
    
    return resultado;
}

// GeradorTypeHints
std::string GeradorTypeHints::gerar_validacao_cpp(const TipoHint& tipo, const std::string& variavel) {
    std::stringstream ss;
    
    switch (tipo.tipo_base) {
        case TIPO_INTEIRO:
            ss << "if (!std::holds_alternative<int64_t>(" << variavel << ")) { throw std::runtime_error(\"Tipo incorreto\"); }";
            break;
        case TIPO_DECIMAL:
            ss << "if (!std::holds_alternative<double>(" << variavel << ")) { throw std::runtime_error(\"Tipo incorreto\"); }";
            break;
        case TIPO_TEXTO:
            ss << "if (!std::holds_alternative<std::string>(" << variavel << ")) { throw std::runtime_error(\"Tipo incorreto\"); }";
            break;
        case TIPO_BOOLEANO:
            ss << "if (!std::holds_alternative<bool>(" << variavel << ")) { throw std::runtime_error(\"Tipo incorreto\"); }";
            break;
        default:
            // Para tipos complexos, validação seria mais elaborada
            ss << "// Validação de tipo complexa";
            break;
    }
    
    return ss.str();
}

std::string GeradorTypeHints::gerar_assert_tipo(const TipoHint& tipo, const std::string& expressao) {
    return "assert(" + gerar_validacao_cpp(tipo, expressao) + ");";
}

std::string GeradorTypeHints::gerar_cast(const TipoHint& tipo_destino, const std::string& expressao) {
    std::stringstream ss;
    
    switch (tipo_destino.tipo_base) {
        case TIPO_INTEIRO:
            ss << "std::get<int64_t>(" << expressao << ")";
            break;
        case TIPO_DECIMAL:
            ss << "std::get<double>(" << expressao << ")";
            break;
        case TIPO_TEXTO:
            ss << "std::get<std::string>(" << expressao << ")";
            break;
        case TIPO_BOOLEANO:
            ss << "std::get<bool>(" << expressao << ")";
            break;
        default:
            ss << expressao;
            break;
    }
    
    return ss.str();
}
