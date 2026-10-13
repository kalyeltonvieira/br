// Serviço SQLite nativo para BR v2.0
// Implementação embutida sem dependências externas
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <mutex>
#include <algorithm>
#include <cctype>
#include <fstream>

// Estrutura para armazenar resultados de queries
struct ResultadoSQLite {
    std::vector<std::string> colunas;
    std::vector<std::vector<std::string>> linhas;
    std::string erro;
    bool sucesso;
    int linhas_afetadas;
    int64_t ultimo_id;
    
    ResultadoSQLite() : sucesso(false), linhas_afetadas(0), ultimo_id(0) {}
    
    std::string paraTexto() const {
        if (!sucesso) return erro;
        
        std::stringstream ss;
        ss << "[SQLite] Colunas: ";
        for (size_t i = 0; i < colunas.size(); i++) {
            if (i > 0) ss << ", ";
            ss << colunas[i];
        }
        ss << "\n";
        
        for (const auto& linha : linhas) {
            ss << "[SQLite] ";
            for (size_t i = 0; i < linha.size(); i++) {
                if (i > 0) ss << " | ";
                ss << linha[i];
            }
            ss << "\n";
        }
        
        if (linhas_afetadas > 0) {
            ss << "[SQLite] Linhas afetadas: " << linhas_afetadas << "\n";
        }
        if (ultimo_id > 0) {
            ss << "[SQLite] Último ID: " << ultimo_id << "\n";
        }
        
        return ss.str();
    }
    
    // Converter para formato JSON
    std::string paraJSON() const {
        if (!sucesso) return "{\"erro\": \"" + erro + "\"}";
        
        std::stringstream ss;
        ss << "{\"sucesso\": true";
        if (linhas_afetadas > 0) ss << ", \"linhas_afetadas\": " << linhas_afetadas;
        if (ultimo_id > 0) ss << ", \"ultimo_id\": " << ultimo_id;
        
        if (!colunas.empty()) {
            ss << ", \"colunas\": [";
            for (size_t i = 0; i < colunas.size(); i++) {
                if (i > 0) ss << ", ";
                ss << "\"" << colunas[i] << "\"";
            }
            ss << "]";
        }
        
        if (!linhas.empty()) {
            ss << ", \"linhas\": [";
            for (size_t i = 0; i < linhas.size(); i++) {
                if (i > 0) ss << ", ";
                ss << "[";
                for (size_t j = 0; j < linhas[i].size(); j++) {
                    if (j > 0) ss << ", ";
                    ss << "\"" << linhas[i][j] << "\"";
                }
                ss << "]";
            }
            ss << "]";
        }
        
        ss << "}";
        return ss.str();
    }
};

// Estrutura de coluna
struct ColunaTabela {
    std::string nome;
    std::string tipo;
    bool chave_primaria;
    bool auto_incremento;
    
    ColunaTabela() : chave_primaria(false), auto_incremento(false) {}
};

// Estrutura de tabela
struct TabelaBR {
    std::string nome;
    std::vector<ColunaTabela> colunas;
    std::vector<std::vector<std::string>> linhas;
    int64_t proximo_id;
    
    TabelaBR() : proximo_id(1) {}
    
    int indiceColuna(const std::string& nome) const {
        for (size_t i = 0; i < colunas.size(); i++) {
            if (colunas[i].nome == nome) return (int)i;
        }
        return -1;
    }
};

// Gerenciador de bancos de dados
class GerenciadorSQLite {
private:
    std::map<std::string, std::map<std::string, TabelaBR>> bancos;
    std::mutex mutex;
    
    void salvarBanco(const std::string& nome_banco) {
        if (nome_banco == ":memory:") return;
        
        auto it = bancos.find(nome_banco);
        if (it == bancos.end()) return;
        
        std::ofstream out(nome_banco, std::ios::binary);
        if (!out) return;
        
        out << "BRDB1\n";
        for (auto& [nome_tabela, tabela] : it->second) {
            out << "TABELA:" << nome_tabela << "\n";
            out << "COLUNAS:" << tabela.colunas.size() << "\n";
            for (auto& col : tabela.colunas) {
                out << col.nome << "|" << col.tipo << "|"
                    << (col.chave_primaria ? "1" : "0") << "|"
                    << (col.auto_incremento ? "1" : "0") << "\n";
            }
            out << "LINHAS:" << tabela.linhas.size() << "\n";
            for (auto& linha : tabela.linhas) {
                out << linha.size();
                for (auto& valor : linha) out << "|" << valor;
                out << "\n";
            }
            out << "PROXIMO_ID:" << tabela.proximo_id << "\n";
        }
        out.close();
    }
    
    void carregarBanco(const std::string& nome_banco) {
        if (nome_banco == ":memory:") return;
        
        std::ifstream in(nome_banco, std::ios::binary);
        if (!in) return;
        
        std::string linha;
        std::getline(in, linha);
        if (linha != "BRDB1") return;
        
        std::map<std::string, TabelaBR> tabelas;
        TabelaBR* tabela_atual = nullptr;
        
        while (std::getline(in, linha)) {
            if (linha.rfind("TABELA:", 0) == 0) {
                std::string nome = linha.substr(7);
                tabelas[nome] = TabelaBR();
                tabelas[nome].nome = nome;
                tabela_atual = &tabelas[nome];
            } else if (linha.rfind("COLUNAS:", 0) == 0 && tabela_atual) {
                int n = std::stoi(linha.substr(8));
                for (int i = 0; i < n; i++) {
                    std::getline(in, linha);
                    std::stringstream ss(linha);
                    ColunaTabela col;
                    std::string parte;
                    std::getline(ss, parte, '|'); col.nome = parte;
                    std::getline(ss, parte, '|'); col.tipo = parte;
                    std::getline(ss, parte, '|'); col.chave_primaria = parte == "1";
                    std::getline(ss, parte, '|'); col.auto_incremento = parte == "1";
                    tabela_atual->colunas.push_back(col);
                }
            } else if (linha.rfind("LINHAS:", 0) == 0 && tabela_atual) {
                int n = std::stoi(linha.substr(7));
                for (int i = 0; i < n; i++) {
                    std::getline(in, linha);
                    std::stringstream ss(linha);
                    std::string parte;
                    std::getline(ss, parte, '|');
                    int num_cols = std::stoi(parte);
                    std::vector<std::string> linha_dados;
                    for (int j = 0; j < num_cols; j++) {
                        std::getline(ss, parte, '|');
                        linha_dados.push_back(parte);
                    }
                    tabela_atual->linhas.push_back(linha_dados);
                }
            } else if (linha.rfind("PROXIMO_ID:", 0) == 0 && tabela_atual) {
                tabela_atual->proximo_id = std::stoll(linha.substr(11));
            }
        }
        
        bancos[nome_banco] = tabelas;
    }
    
public:
    std::map<std::string, TabelaBR>& obterBanco(const std::string& nome) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = bancos.find(nome);
        if (it == bancos.end()) {
            carregarBanco(nome);
            bancos[nome] = std::map<std::string, TabelaBR>();
        }
        return bancos[nome];
    }
    
    void salvar(const std::string& nome) {
        std::lock_guard<std::mutex> lock(mutex);
        salvarBanco(nome);
    }
    
    void fechar(const std::string& nome) {
        std::lock_guard<std::mutex> lock(mutex);
        salvarBanco(nome);
        bancos.erase(nome);
    }
    
    void fecharTodos() {
        std::lock_guard<std::mutex> lock(mutex);
        for (auto& [nome, _] : bancos) salvarBanco(nome);
        bancos.clear();
    }
};

static GerenciadorSQLite gerenciador;

// Utilitários
static std::string trim(const std::string& s) {
    size_t inicio = s.find_first_not_of(" \t\r\n");
    if (inicio == std::string::npos) return "";
    size_t fim = s.find_last_not_of(" \t\r\n");
    return s.substr(inicio, fim - inicio + 1);
}

static std::string paraMinusculas(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(), ::tolower);
    return r;
}

static std::vector<std::string> dividir(const std::string& s, char delim) {
    std::vector<std::string> resultado;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) resultado.push_back(trim(item));
    }
    return resultado;
}

static std::string removerAspas(const std::string& s) {
    std::string r = trim(s);
    if (r.size() >= 2 && ((r[0] == '\'' && r.back() == '\'') || (r[0] == '"' && r.back() == '"'))) {
        return r.substr(1, r.size() - 2);
    }
    return r;
}

// Converter query em português para SQL
static std::string traduzirQuery(const std::string& query) {
    std::string q = query;
    
    // Normalizar espaços
    std::stringstream ss;
    bool espaco = false;
    for (char c : q) {
        if (isspace(c)) {
            if (!espaco) ss << ' ';
            espaco = true;
        } else {
            ss << c;
            espaco = false;
        }
    }
    q = ss.str();
    
    // Remover espaços extras
    while (q.find("  ") != std::string::npos) {
        q.replace(q.find("  "), 2, " ");
    }
    
    // Converter palavras-chave em português para SQL
    // Nota: isso é uma tradução básica. Para queries complexas, o usuário pode usar SQL direto.
    std::string qLower = q;
    std::transform(qLower.begin(), qLower.end(), qLower.begin(), ::tolower);
    
    // Se já é SQL, retornar como está
    if (qLower.find("select ") == 0 || qLower.find("insert ") == 0 || 
        qLower.find("update ") == 0 || qLower.find("delete ") == 0 ||
        qLower.find("create ") == 0 || qLower.find("drop ") == 0 ||
        qLower.find("alter ") == 0) {
        return q;
    }
    
    // Tradução básica de comandos em português
    // "crie tabela usuarios (id inteiro, nome texto)" -> "CREATE TABLE usuarios (id INTEGER, nome TEXT)"
    if (qLower.find("crie tabela") == 0) {
        q = "CREATE TABLE " + q.substr(12);
    }
    else if (qLower.find("criar tabela") == 0) {
        q = "CREATE TABLE " + q.substr(13);
    }
    else if (qLower.find("insira em") == 0) {
        q = "INSERT INTO " + q.substr(10);
    }
    else if (qLower.find("inserir em") == 0) {
        q = "INSERT INTO " + q.substr(11);
    }
    else if (qLower.find("atualize") == 0) {
        q = "UPDATE " + q.substr(9);
    }
    else if (qLower.find("atualizar") == 0) {
        q = "UPDATE " + q.substr(10);
    }
    else if (qLower.find("remova de") == 0) {
        q = "DELETE FROM " + q.substr(10);
    }
    else if (qLower.find("remover de") == 0) {
        q = "DELETE FROM " + q.substr(11);
    }
    else if (qLower.find("selecione") == 0) {
        q = "SELECT " + q.substr(9);
    }
    else if (qLower.find("selecionar") == 0) {
        q = "SELECT " + q.substr(10);
    }
    else if (qLower.find("solte") == 0) {
        q = "DROP " + q.substr(5);
    }
    else if (qLower.find("soltar") == 0) {
        q = "DROP " + q.substr(6);
    }
    
    // Traduzir tipos de dados
    std::vector<std::pair<std::string, std::string>> tipos = {
        {"inteiro", "INTEGER"}, {"int", "INTEGER"}, {"numero", "REAL"}, 
        {"decimal", "REAL"}, {"real", "REAL"}, {"texto", "TEXT"}, 
        {"string", "TEXT"}, {"booleano", "BOOLEAN"}, {"bool", "BOOLEAN"},
        {"data", "DATE"}, {"hora", "TIME"}, {"data_hora", "DATETIME"},
        {"blob", "BLOB"}, {"nulo", "NULL"}
    };
    
    for (auto& [pt, sql] : tipos) {
        // Substituir apenas palavras completas (case insensitive)
        std::string lower = q;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        size_t pos = 0;
        while ((pos = lower.find(pt, pos)) != std::string::npos) {
            // Verificar se é palavra completa
            bool inicio_ok = (pos == 0) || !isalnum(lower[pos - 1]);
            bool fim_ok = (pos + pt.length() >= lower.length()) || !isalnum(lower[pos + pt.length()]);
            if (inicio_ok && fim_ok) {
                q.replace(pos, pt.length(), sql);
                lower = q;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                pos += sql.length();
            } else {
                pos += pt.length();
            }
        }
    }
    
    // Traduzir palavras-chave comuns
    std::vector<std::pair<std::string, std::string>> palavras = {
        {"onde", "WHERE"}, {"e", "AND"}, {"ou", "OR"}, {"nao", "NOT"},
        {"ordene por", "ORDER BY"}, {"ordem por", "ORDER BY"}, {"ordene", "ORDER BY"},
        {"agrupe por", "GROUP BY"}, {"agrupar por", "GROUP BY"},
        {"limite", "LIMIT"}, {"valores", "VALUES"}, {"em", "IN"},
        {"entre", "BETWEEN"}, {"como", "LIKE"}, {"de", "FROM"},
        {"defina", "SET"}, {"definir", "SET"}, {"valores", "VALUES"},
        {"conte", "COUNT"}, {"soma", "SUM"}, {"media", "AVG"},
        {"minimo", "MIN"}, {"maximo", "MAX"}, {"distinto", "DISTINCT"},
        {"juncao", "JOIN"}, {"juncao interna", "INNER JOIN"}, {"juncao esquerda", "LEFT JOIN"},
        {"juncao direita", "RIGHT JOIN"}, {"por", "BY"}, {"ascendente", "ASC"}, {"descendente", "DESC"}
    };
    
    for (auto& [pt, sql] : palavras) {
        std::string lower = q;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        size_t pos = 0;
        while ((pos = lower.find(pt, pos)) != std::string::npos) {
            bool inicio_ok = (pos == 0) || !isalnum(lower[pos - 1]);
            bool fim_ok = (pos + pt.length() >= lower.length()) || !isalnum(lower[pos + pt.length()]);
            if (inicio_ok && fim_ok) {
                q.replace(pos, pt.length(), sql);
                lower = q;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                pos += sql.length();
            } else {
                pos += pt.length();
            }
        }
    }
    
    return q;
}

// Executar query SQLite (implementação embutida)
static std::string executarSQLite(const std::string& query, const std::string& caminho_banco = ":memory:") {
    std::string sql = traduzirQuery(query);
    std::string sqlLower = paraMinusculas(sql);
    
    auto& banco = gerenciador.obterBanco(caminho_banco);
    
    // CREATE TABLE
    if (sqlLower.find("create table") == 0) {
        std::string resto = sql.substr(13);
        size_t paren = resto.find('(');
        if (paren == std::string::npos) return "[SQLite] Erro: CREATE TABLE sem parênteses";
        
        std::string nome_tabela = trim(resto.substr(0, paren));
        std::string definicao = resto.substr(paren + 1);
        if (!definicao.empty() && definicao.back() == ')') definicao.pop_back();
        
        TabelaBR tabela;
        tabela.nome = nome_tabela;
        
        for (auto& col_str : dividir(definicao, ',')) {
            auto partes = dividir(col_str, ' ');
            if (partes.empty()) continue;
            
            ColunaTabela col;
            col.nome = partes[0];
            if (partes.size() > 1) col.tipo = partes[1];
            
            std::string lower_col = paraMinusculas(col_str);
            if (lower_col.find("primary key") != std::string::npos) col.chave_primaria = true;
            if (lower_col.find("autoincrement") != std::string::npos || 
                lower_col.find("auto_increment") != std::string::npos) col.auto_incremento = true;
            
            tabela.colunas.push_back(col);
        }
        
        banco[nome_tabela] = tabela;
        gerenciador.salvar(caminho_banco);
        return "[SQLite] Tabela '" + nome_tabela + "' criada com sucesso";
    }
    
    // DROP TABLE
    if (sqlLower.find("drop table") == 0) {
        std::string nome_tabela = trim(sql.substr(11));
        auto it = banco.find(nome_tabela);
        if (it == banco.end()) return "[SQLite] Erro: Tabela '" + nome_tabela + "' não existe";
        banco.erase(it);
        gerenciador.salvar(caminho_banco);
        return "[SQLite] Tabela '" + nome_tabela + "' removida";
    }
    
    // INSERT INTO
    if (sqlLower.find("insert into") == 0) {
        std::string resto = sql.substr(12);
        size_t paren = resto.find('(');
        if (paren == std::string::npos) return "[SQLite] Erro: INSERT sem parênteses";
        
        std::string nome_tabela = trim(resto.substr(0, paren));
        std::string colunas_str = resto.substr(paren + 1);
        size_t values_pos = colunas_str.find("VALUES");
        if (values_pos == std::string::npos) values_pos = colunas_str.find("values");
        if (values_pos == std::string::npos) return "[SQLite] Erro: INSERT sem VALUES";
        
        std::string cols = colunas_str.substr(0, values_pos);
        if (!cols.empty() && cols.back() == ')') cols.pop_back();
        
        std::string valores_str = colunas_str.substr(values_pos + 6);
        if (!valores_str.empty() && valores_str.back() == ')') valores_str.pop_back();
        
        auto it = banco.find(nome_tabela);
        if (it == banco.end()) return "[SQLite] Erro: Tabela '" + nome_tabela + "' não existe";
        
        auto& tabela = it->second;
        std::vector<std::string> colunas_inserir;
        for (auto& c : dividir(cols, ',')) colunas_inserir.push_back(trim(c));
        
        std::vector<std::string> valores;
        for (auto& v : dividir(valores_str, ',')) valores.push_back(removerAspas(v));
        
        std::vector<std::string> nova_linha(tabela.colunas.size(), "NULL");
        
        for (size_t i = 0; i < tabela.colunas.size(); i++) {
            if (tabela.colunas[i].auto_incremento || tabela.colunas[i].chave_primaria) {
                nova_linha[i] = std::to_string(tabela.proximo_id);
            }
        }
        
        for (size_t i = 0; i < colunas_inserir.size() && i < valores.size(); i++) {
            int idx = tabela.indiceColuna(colunas_inserir[i]);
            if (idx >= 0) nova_linha[idx] = valores[i];
        }
        
        tabela.linhas.push_back(nova_linha);
        int64_t id = tabela.proximo_id;
        tabela.proximo_id++;
        
        gerenciador.salvar(caminho_banco);
        return "[SQLite] Registro inserido. ID: " + std::to_string(id);
    }
    
    // SELECT
    if (sqlLower.find("select") == 0) {
        std::string resto = sql.substr(6);
        size_t from_pos = resto.find(" FROM ");
        if (from_pos == std::string::npos) from_pos = resto.find(" from ");
        if (from_pos == std::string::npos) return "[SQLite] Erro: SELECT sem FROM";
        
        std::string colunas_sel = trim(resto.substr(0, from_pos));
        std::string resto2 = resto.substr(from_pos + 6);
        
        size_t where_pos = resto2.find(" WHERE ");
        if (where_pos == std::string::npos) where_pos = resto2.find(" where ");
        
        std::string nome_tabela;
        std::string condicao;
        
        if (where_pos != std::string::npos) {
            nome_tabela = trim(resto2.substr(0, where_pos));
            condicao = trim(resto2.substr(where_pos + 7));
        } else {
            nome_tabela = trim(resto2);
        }
        
        auto it = banco.find(nome_tabela);
        if (it == banco.end()) return "[SQLite] Erro: Tabela '" + nome_tabela + "' não existe";
        
        auto& tabela = it->second;
        ResultadoSQLite resultado;
        resultado.sucesso = true;
        
        std::vector<int> indices_sel;
        if (colunas_sel == "*") {
            for (size_t i = 0; i < tabela.colunas.size(); i++) {
                indices_sel.push_back((int)i);
                resultado.colunas.push_back(tabela.colunas[i].nome);
            }
        } else {
            for (auto& c : dividir(colunas_sel, ',')) {
                std::string nome_col = trim(c);
                int idx = tabela.indiceColuna(nome_col);
                if (idx >= 0) {
                    indices_sel.push_back(idx);
                    resultado.colunas.push_back(nome_col);
                }
            }
        }
        
        for (auto& linha : tabela.linhas) {
            bool incluir = true;
            if (!condicao.empty()) {
                std::stringstream css(condicao);
                std::string col, op, val;
                css >> col >> op;
                std::getline(css, val);
                val = removerAspas(val);
                
                int idx = tabela.indiceColuna(col);
                if (idx >= 0 && idx < (int)linha.size()) {
                    std::string valor_linha = linha[idx];
                    double v1 = 0, v2 = 0;
                    try { v1 = std::stod(valor_linha); } catch (...) {}
                    try { v2 = std::stod(val); } catch (...) {}
                    
                    if (op == "=" || op == "==") incluir = (valor_linha == val);
                    else if (op == "!=") incluir = (valor_linha != val);
                    else if (op == ">") incluir = (v1 > v2);
                    else if (op == "<") incluir = (v1 < v2);
                    else if (op == ">=") incluir = (v1 >= v2);
                    else if (op == "<=") incluir = (v1 <= v2);
                }
            }
            
            if (incluir) {
                std::vector<std::string> linha_sel;
                for (int idx : indices_sel) {
                    if (idx < (int)linha.size()) linha_sel.push_back(linha[idx]);
                }
                resultado.linhas.push_back(linha_sel);
            }
        }
        
        return resultado.paraTexto();
    }
    
    // UPDATE
    if (sqlLower.find("update") == 0) {
        std::string resto = sql.substr(6);
        size_t set_pos = resto.find(" SET ");
        if (set_pos == std::string::npos) set_pos = resto.find(" set ");
        if (set_pos == std::string::npos) return "[SQLite] Erro: UPDATE sem SET";
        
        std::string nome_tabela = trim(resto.substr(0, set_pos));
        std::string resto2 = resto.substr(set_pos + 5);
        
        size_t where_pos = resto2.find(" WHERE ");
        if (where_pos == std::string::npos) where_pos = resto2.find(" where ");
        
        std::string sets_str;
        std::string condicao;
        
        if (where_pos != std::string::npos) {
            sets_str = trim(resto2.substr(0, where_pos));
            condicao = trim(resto2.substr(where_pos + 7));
        } else {
            sets_str = trim(resto2);
        }
        
        auto it = banco.find(nome_tabela);
        if (it == banco.end()) return "[SQLite] Erro: Tabela '" + nome_tabela + "' não existe";
        
        auto& tabela = it->second;
        int linhas_afetadas = 0;
        
        std::vector<std::pair<std::string, std::string>> sets;
        for (auto& s : dividir(sets_str, ',')) {
            size_t eq = s.find('=');
            if (eq != std::string::npos) {
                std::string col = trim(s.substr(0, eq));
                std::string val = removerAspas(s.substr(eq + 1));
                sets.push_back({col, val});
            }
        }
        
        for (auto& linha : tabela.linhas) {
            bool incluir = true;
            if (!condicao.empty()) {
                std::stringstream css(condicao);
                std::string col, op, val;
                css >> col >> op;
                std::getline(css, val);
                val = removerAspas(val);
                
                int idx = tabela.indiceColuna(col);
                if (idx >= 0 && idx < (int)linha.size()) {
                    std::string valor_linha = linha[idx];
                    if (op == "=" || op == "==") incluir = (valor_linha == val);
                    else if (op == "!=") incluir = (valor_linha != val);
                    else {
                        double v1 = 0, v2 = 0;
                        try { v1 = std::stod(valor_linha); } catch (...) {}
                        try { v2 = std::stod(val); } catch (...) {}
                        if (op == ">") incluir = (v1 > v2);
                        else if (op == "<") incluir = (v1 < v2);
                        else if (op == ">=") incluir = (v1 >= v2);
                        else if (op == "<=") incluir = (v1 <= v2);
                    }
                }
            }
            
            if (incluir) {
                for (auto& [col, val] : sets) {
                    int idx = tabela.indiceColuna(col);
                    if (idx >= 0 && idx < (int)linha.size()) linha[idx] = val;
                }
                linhas_afetadas++;
            }
        }
        
        gerenciador.salvar(caminho_banco);
        return "[SQLite] Registros atualizados: " + std::to_string(linhas_afetadas);
    }
    
    // DELETE
    if (sqlLower.find("delete from") == 0) {
        std::string resto = sql.substr(12);
        size_t where_pos = resto.find(" WHERE ");
        if (where_pos == std::string::npos) where_pos = resto.find(" where ");
        
        std::string nome_tabela;
        std::string condicao;
        
        if (where_pos != std::string::npos) {
            nome_tabela = trim(resto.substr(0, where_pos));
            condicao = trim(resto.substr(where_pos + 7));
        } else {
            nome_tabela = trim(resto);
        }
        
        auto it = banco.find(nome_tabela);
        if (it == banco.end()) return "[SQLite] Erro: Tabela '" + nome_tabela + "' não existe";
        
        auto& tabela = it->second;
        int linhas_removidas = 0;
        
        auto novo_fim = std::remove_if(tabela.linhas.begin(), tabela.linhas.end(), [&](const std::vector<std::string>& linha) {
            if (condicao.empty()) {
                linhas_removidas++;
                return true;
            }
            
            std::stringstream css(condicao);
            std::string col, op, val;
            css >> col >> op;
            std::getline(css, val);
            val = removerAspas(val);
            
            int idx = tabela.indiceColuna(col);
            if (idx >= 0 && idx < (int)linha.size()) {
                std::string valor_linha = linha[idx];
                bool match = false;
                if (op == "=" || op == "==") match = (valor_linha == val);
                else if (op == "!=") match = (valor_linha != val);
                else {
                    double v1 = 0, v2 = 0;
                    try { v1 = std::stod(valor_linha); } catch (...) {}
                    try { v2 = std::stod(val); } catch (...) {}
                    if (op == ">") match = (v1 > v2);
                    else if (op == "<") match = (v1 < v2);
                    else if (op == ">=") match = (v1 >= v2);
                    else if (op == "<=") match = (v1 <= v2);
                }
                if (match) linhas_removidas++;
                return match;
            }
            return false;
        });
        
        tabela.linhas.erase(novo_fim, tabela.linhas.end());
        gerenciador.salvar(caminho_banco);
        return "[SQLite] Registros removidos: " + std::to_string(linhas_removidas);
    }
    
    // PRAGMA table_info
    if (sqlLower.find("pragma table_info") == 0) {
        std::string nome_tabela = trim(sql.substr(18));
        if (!nome_tabela.empty() && nome_tabela.back() == ')') nome_tabela.pop_back();
        
        auto it = banco.find(nome_tabela);
        if (it == banco.end()) return "[SQLite] Erro: Tabela '" + nome_tabela + "' não existe";
        
        ResultadoSQLite resultado;
        resultado.sucesso = true;
        resultado.colunas = {"cid", "name", "type", "notnull", "dflt_value", "pk"};
        
        for (size_t i = 0; i < it->second.colunas.size(); i++) {
            auto& col = it->second.colunas[i];
            std::vector<std::string> linha = {
                std::to_string(i), col.nome, col.tipo, "0", "NULL",
                col.chave_primaria ? "1" : "0"
            };
            resultado.linhas.push_back(linha);
        }
        
        return resultado.paraTexto();
    }
    
    // SELECT from sqlite_master
    if (sqlLower.find("select name from sqlite_master") == 0) {
        ResultadoSQLite resultado;
        resultado.sucesso = true;
        resultado.colunas = {"name"};
        
        for (auto& [nome, _] : banco) {
            resultado.linhas.push_back({nome});
        }
        
        return resultado.paraTexto();
    }
    
    return "[SQLite] Query não suportada: " + sql;
}

// Executar query e retornar JSON
static std::string executarSQLiteJSON(const std::string& query, const std::string& caminho_banco = ":memory:") {
    std::string resultado = executarSQLite(query, caminho_banco);
    std::stringstream ss;
    ss << "{\"resultado\": \"" << resultado << "\"}";
    return ss.str();
}

// Funções de alto nível
static void fecharBanco(const std::string& caminho_banco) {
    gerenciador.fechar(caminho_banco);
}

static void fecharTodosBancos() {
    gerenciador.fecharTodos();
}
