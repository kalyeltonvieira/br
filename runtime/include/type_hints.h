// Sistema de Type Hints Opcionais para BR v2.0
// Implementa anotações de tipo opcionais
#ifndef TYPE_HINTS_H
#define TYPE_HINTS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>

// Tipos suportados
enum TipoBR {
    TIPO_INTEIRO,
    TIPO_DECIMAL,
    TIPO_TEXTO,
    TIPO_BOOLEANO,
    TIPO_NULO,
    TIPO_LISTA,
    TIPO_REGISTRO,
    TIPO_FUNCAO,
    TIPO_UNION,
    TIPO_INTERSECCAO,
    TIPO_OPCIONAL,
    TIPO_GENERICO,
    TIPO_QUALQUER
};

// Estrutura de tipo
struct TipoHint {
    TipoBR tipo_base;
    std::vector<std::shared_ptr<TipoHint>> parametros_tipo; // Para genéricos
    std::shared_ptr<TipoHint> tipo_elemento; // Para listas
    std::map<std::string, std::shared_ptr<TipoHint>> campos; // Para registros
    std::string nome_generico; // Para tipos genéricos
    bool opcional;
    
    TipoHint() : tipo_base(TIPO_QUALQUER), opcional(false) {}
    
    static TipoHint inteiro() {
        TipoHint t;
        t.tipo_base = TIPO_INTEIRO;
        return t;
    }
    
    static TipoHint decimal() {
        TipoHint t;
        t.tipo_base = TIPO_DECIMAL;
        return t;
    }
    
    static TipoHint texto() {
        TipoHint t;
        t.tipo_base = TIPO_TEXTO;
        return t;
    }
    
    static TipoHint booleano() {
        TipoHint t;
        t.tipo_base = TIPO_BOOLEANO;
        return t;
    }
    
    static TipoHint nulo() {
        TipoHint t;
        t.tipo_base = TIPO_NULO;
        return t;
    }
    
    static TipoHint lista(std::shared_ptr<TipoHint> elemento) {
        TipoHint t;
        t.tipo_base = TIPO_LISTA;
        t.tipo_elemento = elemento;
        return t;
    }
    
    static TipoHint registro(const std::map<std::string, std::shared_ptr<TipoHint>>& campos) {
        TipoHint t;
        t.tipo_base = TIPO_REGISTRO;
        t.campos = campos;
        return t;
    }
    
    static TipoHint funcao(std::shared_ptr<TipoHint> retorno, 
                           const std::vector<std::shared_ptr<TipoHint>>& params) {
        TipoHint t;
        t.tipo_base = TIPO_FUNCAO;
        t.tipo_elemento = retorno;
        t.parametros_tipo = params;
        return t;
    }
    
    static TipoHint uniao(const std::vector<std::shared_ptr<TipoHint>>& tipos) {
        TipoHint t;
        t.tipo_base = TIPO_UNION;
        t.parametros_tipo = tipos;
        return t;
    }
    
    static TipoHint interseccao(const std::vector<std::shared_ptr<TipoHint>>& tipos) {
        TipoHint t;
        t.tipo_base = TIPO_INTERSECCAO;
        t.parametros_tipo = tipos;
        return t;
    }
    
    static TipoHint opcional(std::shared_ptr<TipoHint> tipo) {
        TipoHint t = *tipo;
        t.opcional = true;
        return t;
    }
    
    static TipoHint generico(const std::string& nome) {
        TipoHint t;
        t.tipo_base = TIPO_GENERICO;
        t.nome_generico = nome;
        return t;
    }
    
    static TipoHint qualquer() {
        TipoHint t;
        t.tipo_base = TIPO_QUALQUER;
        return t;
    }
    
    std::string para_string() const;
    bool compativel_com(const TipoHint& outro) const;
};

// Anotação de tipo para variável
struct AnotacaoVariavel {
    std::string nome;
    TipoHint tipo;
    bool inferido;
    
    AnotacaoVariavel() : inferido(false) {}
    AnotacaoVariavel(const std::string& n, const TipoHint& t, bool inf = false)
        : nome(n), tipo(t), inferido(inf) {}
};

// Anotação de tipo para função
struct AnotacaoFuncao {
    std::string nome;
    std::vector<AnotacaoVariavel> parametros;
    TipoHint retorno;
    bool inferido;
    
    AnotacaoFuncao() : inferido(false) {}
};

// Anotação de tipo para classe
struct AnotacaoClasse {
    std::string nome;
    std::vector<std::string> parametros_genericos;
    std::map<std::string, TipoHint> campos;
    std::map<std::string, AnotacaoFuncao> metodos;
    std::string classe_mae;
};

// Verificador de tipos
class VerificadorTipos {
private:
    std::map<std::string, AnotacaoVariavel> variaveis;
    std::map<std::string, AnotacaoFuncao> funcoes;
    std::map<std::string, AnotacaoClasse> classes;
    std::vector<std::string> erros;
    
public:
    VerificadorTipos();
    
    // Adicionar anotações
    void adicionar_variavel(const AnotacaoVariavel& var);
    void adicionar_funcao(const AnotacaoFuncao& fun);
    void adicionar_classe(const AnotacaoClasse& cls);
    
    // Verificar tipos
    bool verificar_atribuicao(const std::string& nome_var, const TipoHint& tipo_valor);
    bool verificar_chamada_funcao(const std::string& nome_fun, 
                                   const std::vector<TipoHint>& args);
    bool verificar_retorno(const TipoHint& tipo_retorno);
    
    // Inferência de tipos
    TipoHint inferir_tipo(const std::string& expressao);
    TipoHint inferir_tipo_literal(const std::string& literal);
    
    // Obter erros
    std::vector<std::string> obter_erros() const { return erros; }
    bool tem_erros() const { return !erros.empty(); }
    
    // Limpar
    void limpar();
    
private:
    void adicionar_erro(const std::string& erro);
    bool tipos_compativel(const TipoHint& esperado, const TipoHint& recebido);
};

// Parser de type hints
class ParserTypeHints {
public:
    static TipoHint parsear(const std::string& texto_tipo);
    static std::string normalizar(const std::string& texto);
    
private:
    static size_t encontrar_fim_parentese(const std::string& s, size_t inicio);
    static std::vector<std::string> dividir_tipos(const std::string& s);
};

// Gerador de código com type hints
class GeradorTypeHints {
public:
    static std::string gerar_validacao_cpp(const TipoHint& tipo, const std::string& variavel);
    static std::string gerar_assert_tipo(const TipoHint& tipo, const std::string& expressao);
    static std::string gerar_cast(const TipoHint& tipo_destino, const std::string& expressao);
};

#endif // TYPE_HINTS_H
