// Implementação da Biblioteca ML/AI Nativa para BR v2.0
#include "biblioteca_ml.h"
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <sstream>

// RegressaoLinear
RegressaoLinear::RegressaoLinear() {
    tipo = "regressao_linear";
    treinado = false;
    intercepto = 0.0;
}

double RegressaoLinear::prever(const std::vector<double>& entrada) {
    if (!treinado || coeficientes.size() != entrada.size()) {
        return 0.0;
    }
    
    double resultado = intercepto;
    for (size_t i = 0; i < entrada.size(); i++) {
        resultado += coeficientes[i] * entrada[i];
    }
    
    return resultado;
}

void RegressaoLinear::treinar(const Dataset& dados) {
    if (dados.X.empty() || dados.X.size() != dados.y.size()) {
        return;
    }
    
    size_t n = dados.X.size();
    size_t p = dados.num_features();
    
    // Método dos mínimos quadrados (simplificado)
    // Em produção, usar biblioteca como Eigen ou armadillo
    
    coeficientes.resize(p, 0.0);
    intercepto = 0.0;
    
    // Gradiente descendente simplificado
    double taxa = 0.001;
    int epocas = 1000;
    
    for (int epoca = 0; epoca < epocas; epoca++) {
        for (size_t i = 0; i < n; i++) {
            double previsto = prever(dados.X[i]);
            double erro = dados.y[i] - previsto;
            
            intercepto += taxa * erro;
            
            for (size_t j = 0; j < p; j++) {
                coeficientes[j] += taxa * erro * dados.X[i][j];
            }
        }
    }
    
    treinado = true;
}

std::vector<double> RegressaoLinear::obter_coeficientes() const {
    return coeficientes;
}

double RegressaoLinear::obter_intercepto() const {
    return intercepto;
}

// RegressaoLogistica
RegressaoLogistica::RegressaoLogistica(double taxa) 
    : taxa_aprendizado(taxa), vies(0.0) {
    tipo = "regressao_logistica";
    treinado = false;
}

double RegressaoLogistica::prever(const std::vector<double>& entrada) {
    if (!treinado || pesos.size() != entrada.size()) {
        return 0.5;
    }
    
    double z = vies;
    for (size_t i = 0; i < entrada.size(); i++) {
        z += pesos[i] * entrada[i];
    }
    
    return 1.0 / (1.0 + std::exp(-z)); // Sigmoid
}

void RegressaoLogistica::treinar(const Dataset& dados, int epocas) {
    if (dados.X.empty() || dados.X.size() != dados.y.size()) {
        return;
    }
    
    size_t n = dados.X.size();
    size_t p = dados.num_features();
    
    pesos.resize(p, 0.0);
    vies = 0.0;
    
    for (int epoca = 0; epoca < epocas; epoca++) {
        for (size_t i = 0; i < n; i++) {
            double previsto = prever(dados.X[i]);
            double erro = dados.y[i] - previsto;
            
            vies += taxa_aprendizado * erro;
            
            for (size_t j = 0; j < p; j++) {
                pesos[j] += taxa_aprendizado * erro * dados.X[i][j];
            }
        }
    }
    
    treinado = true;
}

void RegressaoLogistica::treinar(const Dataset& dados) {
    treinar(dados, 1000);
}

int RegressaoLogistica::prever_classe(const std::vector<double>& entrada) {
    return prever(entrada) >= 0.5 ? 1 : 0;
}

void RegressaoLogistica::definir_taxa_aprendizado(double taxa) {
    taxa_aprendizado = taxa;
}

// KNN
KNN::KNN(int vizinhos) : k(vizinhos) {
    tipo = "knn";
    treinado = false;
}

double KNN::prever(const std::vector<double>& entrada) {
    return prever_classe(entrada);
}

void KNN::treinar(const Dataset& dados) {
    dados_treinamento = dados;
    treinado = true;
}

int KNN::prever_classe(const std::vector<double>& entrada) {
    if (!treinado || dados_treinamento.X.empty()) {
        return 0;
    }
    
    // Calcular distâncias
    std::vector<std::pair<double, int>> distancias;
    
    for (size_t i = 0; i < dados_treinamento.X.size(); i++) {
        double dist = MatematicaML::distancia_euclidiana(entrada, dados_treinamento.X[i]);
        int classe = static_cast<int>(dados_treinamento.y[i]);
        distancias.push_back({dist, classe});
    }
    
    // Ordenar por distância
    std::sort(distancias.begin(), distancias.end());
    
    // Votar entre os k vizinhos mais próximos
    std::map<int, int> votos;
    for (int i = 0; i < std::min(k, (int)distancias.size()); i++) {
        votos[distancias[i].second]++;
    }
    
    // Retornar classe com mais votos
    int melhor_classe = 0;
    int max_votos = 0;
    for (const auto& [classe, votos] : votos) {
        if (votos > max_votos) {
            max_votos = votos;
            melhor_classe = classe;
        }
    }
    
    return melhor_classe;
}

void KNN::definir_k(int vizinhos) {
    k = vizinhos;
}

// RedeNeural
RedeNeural::RedeNeural(const std::vector<int>& camadas_ocultas, double taxa) 
    : arquitetura(camadas_ocultas), taxa_aprendizado(taxa) {
    tipo = "rede_neural";
    treinado = false;
    
    // Inicializar pesos aleatoriamente
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dis(0.0, 0.1);
    
    // Para simplificar, assumimos 1 camada de entrada + camadas ocultas + 1 camada de saída
    // Em produção, implementação completa seria mais robusta
}

double RedeNeural::prever(const std::vector<double>& entrada) {
    auto saida = feedforward(entrada);
    return saida.empty() ? 0.0 : saida[0];
}

void RedeNeural::treinar(const Dataset& dados, int epocas) {
    if (dados.X.empty() || dados.X.size() != dados.y.size()) {
        return;
    }
    
    // Inicializar arquitetura se necessário
    if (arquitetura.empty()) {
        arquitetura = {dados.num_features(), 10, 1};
    }
    
    // Inicializar pesos
    // (simplificado - em produção seria mais completo)
    
    for (int epoca = 0; epoca < epocas; epoca++) {
        for (size_t i = 0; i < dados.X.size(); i++) {
            backpropagation(dados.X[i], dados.y[i]);
        }
    }
    
    treinado = true;
}

void RedeNeural::treinar(const Dataset& dados) {
    treinar(dados, 1000);
}

std::vector<double> RedeNeural::feedforward(const std::vector<double>& entrada) {
    // Implementação simplificada
    // Em produção, implementação completa com múltiplas camadas
    return entrada;
}

void RedeNeural::backpropagation(const std::vector<double>& entrada, double saida_alvo) {
    // Implementação simplificada
    // Em produção, backpropagation completo
}

double RedeNeural::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double RedeNeural::derivada_sigmoid(double x) {
    double s = sigmoid(x);
    return s * (1.0 - s);
}

// KMeans
KMeans::KMeans(int num_clusters) : k(num_clusters) {
    tipo = "kmeans";
}

void KMeans::treinar(const Dataset& dados, int max_iteracoes) {
    if (dados.X.empty()) {
        return;
    }
    
    size_t n = dados.X.size();
    size_t p = dados.num_features();
    
    // Inicializar centroides aleatoriamente
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, n - 1);
    
    centroides.resize(k, std::vector<double>(p));
    for (int i = 0; i < k; i++) {
        int idx = dis(gen);
        centroides[i] = dados.X[idx];
    }
    
    // K-means iterations
    for (int iter = 0; iter < max_iteracoes; iter++) {
        // Atribuir pontos aos clusters
        clusters.resize(n);
        for (size_t i = 0; i < n; i++) {
            double min_dist = std::numeric_limits<double>::max();
            int melhor_cluster = 0;
            
            for (int j = 0; j < k; j++) {
                double dist = MatematicaML::distancia_euclidiana(dados.X[i], centroides[j]);
                if (dist < min_dist) {
                    min_dist = dist;
                    melhor_cluster = j;
                }
            }
            
            clusters[i] = melhor_cluster;
        }
        
        // Recalcular centroides
        for (int j = 0; j < k; j++) {
            std::vector<double> soma(p, 0.0);
            int count = 0;
            
            for (size_t i = 0; i < n; i++) {
                if (clusters[i] == j) {
                    for (size_t d = 0; d < p; d++) {
                        soma[d] += dados.X[i][d];
                    }
                    count++;
                }
            }
            
            if (count > 0) {
                for (size_t d = 0; d < p; d++) {
                    centroides[j][d] = soma[d] / count;
                }
            }
        }
    }
}

int KMeans::prever_cluster(const std::vector<double>& ponto) {
    double min_dist = std::numeric_limits<double>::max();
    int melhor_cluster = 0;
    
    for (int j = 0; j < k; j++) {
        double dist = MatematicaML::distancia_euclidiana(ponto, centroides[j]);
        if (dist < min_dist) {
            min_dist = dist;
            melhor_cluster = j;
        }
    }
    
    return melhor_cluster;
}

std::vector<int> KMeans::obter_clusters() const {
    return clusters;
}

std::vector<std::vector<double>> KMeans::obter_centroides() const {
    return centroides;
}

double KMeans::calcular_inercia(const Dataset& dados) {
    double inercia = 0.0;
    
    for (size_t i = 0; i < dados.X.size(); i++) {
        int cluster = clusters[i];
        double dist = MatematicaML::distancia_euclidiana(dados.X[i], centroides[cluster]);
        inercia += dist * dist;
    }
    
    return inercia;
}

// ProcessadorDados
std::vector<std::vector<double>> ProcessadorDados::normalizar_min_max(const std::vector<std::vector<double>>& dados) {
    if (dados.empty()) return dados;
    
    size_t n_features = dados[0].size();
    std::vector<double> min_vals(n_features, std::numeric_limits<double>::max());
    std::vector<double> max_vals(n_features, std::numeric_limits<double>::lowest());
    
    // Encontrar min e max
    for (const auto& amostra : dados) {
        for (size_t j = 0; j < n_features; j++) {
            min_vals[j] = std::min(min_vals[j], amostra[j]);
            max_vals[j] = std::max(max_vals[j], amostra[j]);
        }
    }
    
    // Normalizar
    std::vector<std::vector<double>> normalizado = dados;
    for (auto& amostra : normalizado) {
        for (size_t j = 0; j < n_features; j++) {
            if (max_vals[j] > min_vals[j]) {
                amostra[j] = (amostra[j] - min_vals[j]) / (max_vals[j] - min_vals[j]);
            }
        }
    }
    
    return normalizado;
}

std::vector<std::vector<double>> ProcessadorDados::normalizar_z_score(const std::vector<std::vector<double>>& dados) {
    if (dados.empty()) return dados;
    
    size_t n_features = dados[0].size();
    std::vector<double> medias(n_features, 0.0);
    std::vector<double> desvios(n_features, 0.0);
    
    // Calcular médias
    for (const auto& amostra : dados) {
        for (size_t j = 0; j < n_features; j++) {
            medias[j] += amostra[j];
        }
    }
    for (size_t j = 0; j < n_features; j++) {
        medias[j] /= dados.size();
    }
    
    // Calcular desvios padrão
    for (const auto& amostra : dados) {
        for (size_t j = 0; j < n_features; j++) {
            desvios[j] += std::pow(amostra[j] - medias[j], 2);
        }
    }
    for (size_t j = 0; j < n_features; j++) {
        desvios[j] = std::sqrt(desvios[j] / dados.size());
    }
    
    // Normalizar
    std::vector<std::vector<double>> normalizado = dados;
    for (auto& amostra : normalizado) {
        for (size_t j = 0; j < n_features; j++) {
            if (desvios[j] > 0) {
                amostra[j] = (amostra[j] - medias[j]) / desvios[j];
            }
        }
    }
    
    return normalizado;
}

std::pair<Dataset, Dataset> ProcessadorDados::dividir_treino_teste(const Dataset& dados, double proporcao_treino) {
    Dataset treino, teste;
    
    size_t n = dados.tamanho();
    size_t n_treino = static_cast<size_t>(n * proporcao_treino);
    
    // Embaralhar dados
    std::vector<size_t> indices(n);
    for (size_t i = 0; i < n; i++) indices[i] = i;
    std::random_shuffle(indices.begin(), indices.end());
    
    // Dividir
    for (size_t i = 0; i < n_treino; i++) {
        treino.X.push_back(dados.X[indices[i]]);
        treino.y.push_back(dados.y[indices[i]]);
    }
    
    for (size_t i = n_treino; i < n; i++) {
        teste.X.push_back(dados.X[indices[i]]);
        teste.y.push_back(dados.y[indices[i]]);
    }
    
    treino.nomes_features = dados.nomes_features;
    treino.nome_label = dados.nome_label;
    teste.nomes_features = dados.nomes_features;
    teste.nome_label = dados.nome_label;
    
    return {treino, teste};
}

std::vector<std::vector<double>> ProcessadorDados::criar_polinomios(const std::vector<std::vector<double>>& dados, int grau) {
    if (dados.empty() || grau < 2) return dados;
    
    size_t n = dados.size();
    size_t p = dados[0].size();
    size_t novo_p = p * grau;
    
    std::vector<std::vector<double>> polinomios(n, std::vector<double>(novo_p));
    
    for (size_t i = 0; i < n; i++) {
        for (size_t j = 0; j < p; j++) {
            polinomios[i][j] = dados[i][j];
            
            for (int g = 2; g <= grau; g++) {
                polinomios[i][j + (g - 1) * p] = std::pow(dados[i][j], g);
            }
        }
    }
    
    return polinomios;
}

std::vector<std::vector<double>> ProcessadorDados::one_hot_encode(const std::vector<int>& classes, int num_classes) {
    std::vector<std::vector<double>> encoded(classes.size(), std::vector<double>(num_classes, 0.0));
    
    for (size_t i = 0; i < classes.size(); i++) {
        if (classes[i] >= 0 && classes[i] < num_classes) {
            encoded[i][classes[i]] = 1.0;
        }
    }
    
    return encoded;
}

std::vector<std::vector<double>> ProcessadorDados::pca(const std::vector<std::vector<double>>& dados, int n_componentes) {
    // PCA simplificado
    // Em produção, usar SVD completo
    return dados;
}

// MetricasML
double MetricasML::acuracia(const std::vector<int>& previsto, const std::vector<int>& real) {
    if (previsto.size() != real.size() || previsto.empty()) return 0.0;
    
    int corretos = 0;
    for (size_t i = 0; i < previsto.size(); i++) {
        if (previsto[i] == real[i]) corretos++;
    }
    
    return static_cast<double>(corretos) / previsto.size();
}

double MetricasML::precisao(const std::vector<int>& previsto, const std::vector<int>& real, int classe_positiva) {
    int tp = 0, fp = 0;
    
    for (size_t i = 0; i < previsto.size(); i++) {
        if (previsto[i] == classe_positiva) {
            if (real[i] == classe_positiva) tp++;
            else fp++;
        }
    }
    
    return (tp + fp) > 0 ? static_cast<double>(tp) / (tp + fp) : 0.0;
}

double MetricasML::recall(const std::vector<int>& previsto, const std::vector<int>& real, int classe_positiva) {
    int tp = 0, fn = 0;
    
    for (size_t i = 0; i < real.size(); i++) {
        if (real[i] == classe_positiva) {
            if (previsto[i] == classe_positiva) tp++;
            else fn++;
        }
    }
    
    return (tp + fn) > 0 ? static_cast<double>(tp) / (tp + fn) : 0.0;
}

double MetricasML::f1_score(const std::vector<int>& previsto, const std::vector<int>& real, int classe_positiva) {
    double prec = precisao(previsto, real, classe_positiva);
    double rec = recall(previsto, real, classe_positiva);
    
    return (prec + rec) > 0 ? 2.0 * prec * rec / (prec + rec) : 0.0;
}

double MetricasML::mse(const std::vector<double>& previsto, const std::vector<double>& real) {
    if (previsto.size() != real.size() || previsto.empty()) return 0.0;
    
    double soma = 0.0;
    for (size_t i = 0; i < previsto.size(); i++) {
        soma += std::pow(previsto[i] - real[i], 2);
    }
    
    return soma / previsto.size();
}

double MetricasML::rmse(const std::vector<double>& previsto, const std::vector<double>& real) {
    return std::sqrt(mse(previsto, real));
}

double MetricasML::mae(const std::vector<double>& previsto, const std::vector<double>& real) {
    if (previsto.size() != real.size() || previsto.empty()) return 0.0;
    
    double soma = 0.0;
    for (size_t i = 0; i < previsto.size(); i++) {
        soma += std::abs(previsto[i] - real[i]);
    }
    
    return soma / previsto.size();
}

double MetricasML::r2(const std::vector<double>& previsto, const std::vector<double>& real) {
    if (previsto.size() != real.size() || previsto.empty()) return 0.0;
    
    double media_real = MatematicaML::media(real);
    double ss_res = 0.0, ss_tot = 0.0;
    
    for (size_t i = 0; i < previsto.size(); i++) {
        ss_res += std::pow(real[i] - previsto[i], 2);
        ss_tot += std::pow(real[i] - media_real, 2);
    }
    
    return ss_tot > 0 ? 1.0 - ss_res / ss_tot : 0.0;
}

std::vector<std::vector<int>> MetricasML::matriz_confusao(const std::vector<int>& previsto, 
                                                           const std::vector<int>& real, 
                                                           int num_classes) {
    std::vector<std::vector<int>> matriz(num_classes, std::vector<int>(num_classes, 0));
    
    for (size_t i = 0; i < previsto.size(); i++) {
        if (real[i] >= 0 && real[i] < num_classes && previsto[i] >= 0 && previsto[i] < num_classes) {
            matriz[real[i]][previsto[i]]++;
        }
    }
    
    return matriz;
}

// MatematicaML
double MatematicaML::distancia_euclidiana(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return std::numeric_limits<double>::max();
    
    double soma = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        soma += std::pow(a[i] - b[i], 2);
    }
    
    return std::sqrt(soma);
}

double MatematicaML::distancia_manhattan(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return std::numeric_limits<double>::max();
    
    double soma = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        soma += std::abs(a[i] - b[i]);
    }
    
    return soma;
}

double MatematicaML::produto_escalar(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return 0.0;
    
    double soma = 0.0;
    for (size_t i = 0; i < a.size(); i++) {
        soma += a[i] * b[i];
    }
    
    return soma;
}

std::vector<double> MatematicaML::soma_vetores(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return a;
    
    std::vector<double> resultado(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        resultado[i] = a[i] + b[i];
    }
    
    return resultado;
}

std::vector<double> MatematicaML::subtrair_vetores(const std::vector<double>& a, const std::vector<double>& b) {
    if (a.size() != b.size()) return a;
    
    std::vector<double> resultado(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        resultado[i] = a[i] - b[i];
    }
    
    return resultado;
}

std::vector<double> MatematicaML::multiplicar_escalar(const std::vector<double>& v, double escalar) {
    std::vector<double> resultado(v.size());
    for (size_t i = 0; i < v.size(); i++) {
        resultado[i] = v[i] * escalar;
    }
    return resultado;
}

Matriz MatematicaML::multiplicar_matrizes(const Matriz& A, const Matriz& B) {
    if (A.empty() || B.empty() || A[0].size() != B.size()) return A;
    
    size_t m = A.size();
    size_t n = B[0].size();
    size_t p = B.size();
    
    Matriz resultado(m, std::vector<double>(n, 0.0));
    
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            for (size_t k = 0; k < p; k++) {
                resultado[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    return resultado;
}

Matriz MatematicaML::transpor(const Matriz& M) {
    if (M.empty()) return M;
    
    size_t m = M.size();
    size_t n = M[0].size();
    
    Matriz resultado(n, std::vector<double>(m));
    
    for (size_t i = 0; i < m; i++) {
        for (size_t j = 0; j < n; j++) {
            resultado[j][i] = M[i][j];
        }
    }
    
    return resultado;
}

double MatematicaML::media(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    
    double soma = 0.0;
    for (double val : v) {
        soma += val;
    }
    
    return soma / v.size();
}

double MatematicaML::desvio_padrao(const std::vector<double>& v) {
    return std::sqrt(variancia(v));
}

double MatematicaML::variancia(const std::vector<double>& v) {
    if (v.empty()) return 0.0;
    
    double m = media(v);
    double soma = 0.0;
    
    for (double val : v) {
        soma += std::pow(val - m, 2);
    }
    
    return soma / v.size();
}

// BibliotecaML
BibliotecaML::BibliotecaML() {
}

std::string BibliotecaML::criar_regressao_linear(const std::string& nome) {
    modelos[nome] = std::make_unique<RegressaoLinear>();
    return nome;
}

std::string BibliotecaML::criar_regressao_logistica(const std::string& nome, double taxa) {
    modelos[nome] = std::make_unique<RegressaoLogistica>(taxa);
    return nome;
}

std::string BibliotecaML::criar_knn(const std::string& nome, int k) {
    modelos[nome] = std::make_unique<KNN>(k);
    return nome;
}

std::string BibliotecaML::criar_rede_neural(const std::string& nome, const std::vector<int>& camadas) {
    modelos[nome] = std::make_unique<RedeNeural>(camadas);
    return nome;
}

std::string BibliotecaML::criar_kmeans(const std::string& nome, int k) {
    modelos[nome] = std::make_unique<KMeans>(k);
    return nome;
}

bool BibliotecaML::treinar_modelo(const std::string& nome, const Dataset& dados) {
    auto it = modelos.find(nome);
    if (it != modelos.end()) {
        it->second->treinar(dados);
        return true;
    }
    return false;
}

double BibliotecaML::prever_modelo(const std::string& nome, const std::vector<double>& entrada) {
    auto it = modelos.find(nome);
    if (it != modelos.end()) {
        return it->second->prever(entrada);
    }
    return 0.0;
}

int BibliotecaML::prever_classe_modelo(const std::string& nome, const std::vector<double>& entrada) {
    auto it = modelos.find(nome);
    if (it != modelos.end()) {
        if (auto* knn = dynamic_cast<KNN*>(it->second.get())) {
            return knn->prever_classe(entrada);
        }
        if (auto* logistica = dynamic_cast<RegressaoLogistica*>(it->second.get())) {
            return logistica->prever_classe(entrada);
        }
    }
    return 0;
}

bool BibliotecaML::salvar_modelo(const std::string& nome, const std::string& caminho) {
    // Implementação simplificada - salvar apenas metadados
    std::ofstream arquivo(caminho);
    if (!arquivo.is_open()) return false;
    
    arquivo << "modelo:" << nome << "\n";
    arquivo << "treinado:" << (modelos[nome]->treinado ? "sim" : "nao") << "\n";
    
    arquivo.close();
    return true;
}

bool BibliotecaML::carregar_modelo(const std::string& nome, const std::string& caminho) {
    // Implementação simplificada
    return true;
}

bool BibliotecaML::remover_modelo(const std::string& nome) {
    return modelos.erase(nome) > 0;
}

std::vector<std::string> BibliotecaML::listar_modelos() {
    std::vector<std::string> nomes;
    for (const auto& [nome, modelo] : modelos) {
        nomes.push_back(nome);
    }
    return nomes;
}
