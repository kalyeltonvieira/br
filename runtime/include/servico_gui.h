// Serviço GUI Nativo para BR
#ifndef SERVICO_GUI_H
#define SERVICO_GUI_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

#ifdef _WIN32
    #include <windows.h>
#endif

// Estrutura para componentes GUI
struct ComponenteGUI {
    std::string tipo;  // "janela", "botao", "campo", "rotulo"
    std::string id;
    std::string texto;
    int x, y, largura, altura;
    std::function<void()> aoClicar;
    
    ComponenteGUI() : x(0), y(0), largura(100), altura(30) {}
};

// Serviço de GUI nativo
class ServicoGUI {
private:
    std::vector<std::shared_ptr<ComponenteGUI>> componentes;
    std::string tituloJanela;
    int larguraJanela, alturaJanela;
    bool janelaVisivel;
    
#ifdef _WIN32
    HWND hwndPrincipal;
    HINSTANCE hInstance;
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static ServicoGUI* instancia;
#endif
    
public:
    ServicoGUI();
    ~ServicoGUI();
    
    // Criar janela principal
    void criarJanela(const std::string& titulo, int largura, int altura);
    
    // Adicionar componentes
    void adicionarBotao(const std::string& id, const std::string& texto, int x, int y, 
                        std::function<void()> aoClicar);
    void adicionarCampo(const std::string& id, const std::string& texto, int x, int y, int largura);
    void adicionarRotulo(const std::string& id, const std::string& texto, int x, int y);
    
    // Obter valor de campo
    std::string obterValorCampo(const std::string& id);
    
    // Definir valor de campo
    void definirValorCampo(const std::string& id, const std::string& valor);
    
    // Mostrar janela (bloqueia até fechar)
    void mostrar();
    
    // Fechar janela
    void fechar();
    
    // Verificar se janela está visível
    bool estaVisivel() const { return janelaVisivel; }
};

#endif // SERVICO_GUI_H
