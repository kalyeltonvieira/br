// String Pool Básico (Interning) - Implementação
#include "string_pool.h"
#include <iostream>
#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <cctype>

// ===================== STRING POOL IMPLEMENTATION =====================

StringPool::StringPool() 
    : strings_internadas(0), hits_cache(0), misses_cache(0), 
      memoria_economizada(0), capacidade_maxima(100000) {
}

StringPool::~StringPool() {
    limpar();
}

std::string_view StringPool::internar(const std::string& s) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Verifica se já está internada
    auto it = interned_strings.find(s);
    if (it != interned_strings.end()) {
        hits_cache++;
        memoria_economizada += s.size();
        return it->second;
    }
    
    // Verifica capacidade máxima
    if (interned_strings.size() >= capacidade_maxima) {
        limpar_nao_usadas();
    }
    
    // Adiciona nova string
    adicionar_storage(s);
    std::string_view sv = criar_string_view(storage.back());
    
    interned_strings[s] = sv;
    interned_set.insert(sv);
    strings_internadas++;
    misses_cache++;
    
    return sv;
}

std::string_view StringPool::internar(std::string&& s) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Verifica se já está internada
    auto it = interned_strings.find(s);
    if (it != interned_strings.end()) {
        hits_cache++;
        memoria_economizada += s.size();
        return it->second;
    }
    
    // Verifica capacidade máxima
    if (interned_strings.size() >= capacidade_maxima) {
        limpar_nao_usadas();
    }
    
    // Adiciona nova string (move)
    storage.push_back(std::move(s));
    std::string_view sv = criar_string_view(storage.back());
    
    interned_strings[storage.back()] = sv;
    interned_set.insert(sv);
    strings_internadas++;
    misses_cache++;
    
    return sv;
}

std::string_view StringPool::internar(const char* s, size_t tamanho) {
    return internar(std::string(s, tamanho));
}

bool StringPool::esta_internada(const std::string& s) const {
    std::lock_guard<std::mutex> lock(mutex);
    return interned_strings.find(s) != interned_strings.end();
}

bool StringPool::esta_internada(std::string_view sv) const {
    std::lock_guard<std::mutex> lock(mutex);
    return interned_set.find(sv) != interned_set.end();
}

std::string_view StringPool::obter_internada(const std::string& s) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = interned_strings.find(s);
    if (it != interned_strings.end()) {
        return it->second;
    }
    
    return std::string_view();
}

void StringPool::limpar() {
    std::lock_guard<std::mutex> lock(mutex);
    
    interned_strings.clear();
    interned_set.clear();
    storage.clear();
    strings_internadas = 0;
}

size_t StringPool::limpar_nao_usadas() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Em uma implementação real, rastreariamos uso de cada string
    // Aqui é um placeholder - limpa 10% das strings mais antigas
    size_t para_remover = interned_strings.size() / 10;
    size_t removidas = 0;
    
    auto it = interned_strings.begin();
    while (it != interned_strings.end() && removidas < para_remover) {
        interned_set.erase(it->second);
        it = interned_strings.erase(it);
        removidas++;
    }
    
    strings_internadas -= removidas;
    return removidas;
}

size_t StringPool::obter_contagem() const {
    std::lock_guard<std::mutex> lock(mutex);
    return strings_internadas;
}

size_t StringPool::obter_hits() const {
    std::lock_guard<std::mutex> lock(mutex);
    return hits_cache;
}

size_t StringPool::obter_misses() const {
    std::lock_guard<std::mutex> lock(mutex);
    return misses_cache;
}

double StringPool::obter_taxa_hit() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    size_t total = hits_cache + misses_cache;
    if (total == 0) return 0.0;
    
    return (double)hits_cache / total * 100.0;
}

size_t StringPool::obter_memoria_economizada() const {
    std::lock_guard<std::mutex> lock(mutex);
    return memoria_economizada;
}

void StringPool::definir_capacidade_maxima(size_t max) {
    std::lock_guard<std::mutex> lock(mutex);
    capacidade_maxima = max;
}

size_t StringPool::obter_capacidade_maxima() const {
    return capacidade_maxima;
}

void StringPool::adicionar_storage(const std::string& s) {
    storage.push_back(s);
}

std::string_view StringPool::criar_string_view(const std::string& s) {
    return std::string_view(s);
}

// ===================== STRING BUILDER IMPLEMENTATION =====================

StringBuilder::StringBuilder(size_t capacidade_inicial) 
    : tamanho_usado(0) {
    buffer.reserve(capacidade_inicial);
}

StringBuilder::~StringBuilder() {
    buffer.clear();
}

void StringBuilder::append(const std::string& s) {
    buffer.insert(buffer.end(), s.begin(), s.end());
    tamanho_usado += s.size();
}

void StringBuilder::append(const char* s, size_t tamanho) {
    buffer.insert(buffer.end(), s, s + tamanho);
    tamanho_usado += tamanho;
}

void StringBuilder::append(char c) {
    buffer.push_back(c);
    tamanho_usado++;
}

void StringBuilder::append(std::string_view sv) {
    buffer.insert(buffer.end(), sv.begin(), sv.end());
    tamanho_usado += sv.size();
}

void StringBuilder::append_format(const char* formato, ...) {
    char temp_buffer[1024];
    va_list args;
    va_start(args, formato);
    vsnprintf(temp_buffer, sizeof(temp_buffer), formato, args);
    va_end(args);
    
    append(temp_buffer);
}

std::string StringBuilder::build() {
    return std::string(buffer.begin(), buffer.end());
}

std::string_view StringBuilder::build_view(StringPool* pool) {
    if (pool) {
        return pool->internar(build());
    }
    return std::string_view(buffer.data(), buffer.size());
}

void StringBuilder::clear() {
    buffer.clear();
    tamanho_usado = 0;
}

void StringBuilder::reserve(size_t capacidade) {
    buffer.reserve(capacidade);
}

size_t StringBuilder::tamanho() const {
    return tamanho_usado;
}

size_t StringBuilder::capacidade() const {
    return buffer.capacity();
}

bool StringBuilder::vazio() const {
    return tamanho_usado == 0;
}

// ===================== STRING UTILS IMPLEMENTATION =====================

std::string StringUtils::concatenar_view(std::string_view a, std::string_view b) {
    std::string resultado;
    resultado.reserve(a.size() + b.size());
    resultado.append(a);
    resultado.append(b);
    return resultado;
}

std::string StringUtils::concatenar_view(std::vector<std::string_view> partes) {
    size_t tamanho_total = 0;
    for (auto& parte : partes) {
        tamanho_total += parte.size();
    }
    
    std::string resultado;
    resultado.reserve(tamanho_total);
    
    for (auto& parte : partes) {
        resultado.append(parte);
    }
    
    return resultado;
}

std::string_view StringUtils::substring_view(std::string_view s, size_t start, size_t end) {
    if (start >= s.size()) return std::string_view();
    if (end > s.size()) end = s.size();
    if (start >= end) return std::string_view();
    
    return s.substr(start, end - start);
}

bool StringUtils::igual_ignorando_caso(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    
    for (size_t i = 0; i < a.size(); i++) {
        if (tolower(a[i]) != tolower(b[i])) {
            return false;
        }
    }
    
    return true;
}

bool StringUtils::comeca_com(std::string_view str, std::string_view prefixo) {
    if (prefixo.size() > str.size()) return false;
    
    return str.substr(0, prefixo.size()) == prefixo;
}

bool StringUtils::termina_com(std::string_view str, std::string_view sufixo) {
    if (sufixo.size() > str.size()) return false;
    
    return str.substr(str.size() - sufixo.size()) == sufixo;
}

size_t StringUtils::hash_string(std::string_view s) {
    return std::hash<std::string_view>{}(s);
}

size_t StringUtils::hash_string_case_insensitive(std::string_view s) {
    size_t hash = 5381;
    
    for (char c : s) {
        hash = ((hash << 5) + hash) + tolower(c);
    }
    
    return hash;
}

std::vector<std::string_view> StringUtils::split_view(std::string_view s, char delimitador) {
    std::vector<std::string_view> resultado;
    size_t inicio = 0;
    
    for (size_t i = 0; i <= s.size(); i++) {
        if (i == s.size() || s[i] == delimitador) {
            resultado.push_back(s.substr(inicio, i - inicio));
            inicio = i + 1;
        }
    }
    
    return resultado;
}

std::vector<std::string_view> StringUtils::split_view(std::string_view s, std::string_view delimitador) {
    std::vector<std::string_view> resultado;
    size_t inicio = 0;
    size_t pos = 0;
    
    while ((pos = s.find(delimitador, inicio)) != std::string_view::npos) {
        resultado.push_back(s.substr(inicio, pos - inicio));
        inicio = pos + delimitador.size();
    }
    
    resultado.push_back(s.substr(inicio));
    return resultado;
}
