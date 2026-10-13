// Implementação do Serviço GUI Nativo para BR
#define UNICODE
#define _UNICODE
#include "servico_gui.h"
#include <iostream>
#include <functional>
#include <vector>
#include <string>

#ifdef _WIN32
ServicoGUI* ServicoGUI::instancia = nullptr;

LRESULT CALLBACK ServicoGUI::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    
    if (uMsg == WM_COMMAND) {
        if (instancia) {
            // Processar cliques em botões
            for (auto& comp : instancia->componentes) {
                if (comp->tipo == "botao" && (LOWORD(wParam) == 1)) {
                    if (comp->aoClicar) {
                        comp->aoClicar();
                    }
                }
            }
        }
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif

ServicoGUI::ServicoGUI() 
    : larguraJanela(400), alturaJanela(300), janelaVisivel(false) {
#ifdef _WIN32
    hwndPrincipal = nullptr;
    hInstance = GetModuleHandle(nullptr);
    instancia = this;
#endif
}

ServicoGUI::~ServicoGUI() {
    fechar();
}

void ServicoGUI::criarJanela(const std::string& titulo, int largura, int altura) {
    tituloJanela = titulo;
    larguraJanela = largura;
    alturaJanela = altura;
    
#ifdef _WIN32
    // Registrar classe da janela
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"BRGUIWindow";
    
    std::wstring wtitulo(titulo.begin(), titulo.end());
    
    if (!RegisterClass(&wc)) {
        std::cerr << "[GUI] Erro ao registrar classe da janela" << std::endl;
        return;
    }
    
    // Criar janela
    hwndPrincipal = CreateWindowEx(
        0,
        L"BRGUIWindow",
        wtitulo.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        largura, altura,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    
    if (!hwndPrincipal) {
        std::cerr << "[GUI] Erro ao criar janela" << std::endl;
        return;
    }
    
    janelaVisivel = true;
#else
    std::cout << "[GUI] Janela criada: " << titulo << " (" << largura << "x" << altura << ")" << std::endl;
    std::cout << "[GUI] Nota: GUI nativa completa apenas disponível no Windows. Linux/macOS em desenvolvimento." << std::endl;
    janelaVisivel = true;
#endif
}

void ServicoGUI::adicionarBotao(const std::string& id, const std::string& texto, 
                                  int x, int y, std::function<void()> aoClicar) {
    auto botao = std::make_shared<ComponenteGUI>();
    botao->tipo = "botao";
    botao->id = id;
    botao->texto = texto;
    botao->x = x;
    botao->y = y;
    botao->largura = 100;
    botao->altura = 30;
    botao->aoClicar = aoClicar;
    componentes.push_back(botao);
    
#ifdef _WIN32
    if (hwndPrincipal) {
        std::wstring wtexto(texto.begin(), texto.end());
        CreateWindow(
            L"BUTTON",
            wtexto.c_str(),
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            x, y, 100, 30,
            hwndPrincipal,
            (HMENU)1,
            hInstance,
            nullptr
        );
    }
#else
    std::cout << "[GUI] Botão adicionado: " << id << " = \"" << texto << "\" em (" << x << "," << y << ")" << std::endl;
#endif
}

void ServicoGUI::adicionarCampo(const std::string& id, const std::string& texto, 
                                 int x, int y, int largura) {
    auto campo = std::make_shared<ComponenteGUI>();
    campo->tipo = "campo";
    campo->id = id;
    campo->texto = texto;
    campo->x = x;
    campo->y = y;
    campo->largura = largura;
    campo->altura = 25;
    componentes.push_back(campo);
    
#ifdef _WIN32
    if (hwndPrincipal) {
        std::wstring wtexto(texto.begin(), texto.end());
        HWND hEdit = CreateWindow(
            L"EDIT",
            wtexto.c_str(),
            WS_BORDER | WS_CHILD | WS_VISIBLE,
            x, y, largura, 25,
            hwndPrincipal,
            nullptr,
            hInstance,
            nullptr
        );
    }
#else
    std::cout << "[GUI] Campo adicionado: " << id << " = \"" << texto << "\" em (" << x << "," << y << ")" << std::endl;
#endif
}

void ServicoGUI::adicionarRotulo(const std::string& id, const std::string& texto, 
                                  int x, int y) {
    auto rotulo = std::make_shared<ComponenteGUI>();
    rotulo->tipo = "rotulo";
    rotulo->id = id;
    rotulo->texto = texto;
    rotulo->x = x;
    rotulo->y = y;
    rotulo->largura = 200;
    rotulo->altura = 20;
    componentes.push_back(rotulo);
    
#ifdef _WIN32
    if (hwndPrincipal) {
        std::wstring wtexto(texto.begin(), texto.end());
        CreateWindow(
            L"STATIC",
            wtexto.c_str(),
            WS_CHILD | WS_VISIBLE,
            x, y, 200, 20,
            hwndPrincipal,
            nullptr,
            hInstance,
            nullptr
        );
    }
#else
    std::cout << "[GUI] Rótulo adicionado: " << id << " = \"" << texto << "\" em (" << x << "," << y << ")" << std::endl;
#endif
}

std::string ServicoGUI::obterValorCampo(const std::string& id) {
    // Em implementação real, buscaria valor do componente
    for (auto& comp : componentes) {
        if (comp->id == id && comp->tipo == "campo") {
            return comp->texto;
        }
    }
    return "";
}

void ServicoGUI::definirValorCampo(const std::string& id, const std::string& valor) {
    for (auto& comp : componentes) {
        if (comp->id == id && comp->tipo == "campo") {
            comp->texto = valor;
#ifdef _WIN32
            // Atualizar valor no controle Windows
            if (hwndPrincipal) {
                // Buscar HWND do controle e atualizar
            }
#endif
            return;
        }
    }
}

void ServicoGUI::mostrar() {
#ifdef _WIN32
    if (hwndPrincipal) {
        ShowWindow(hwndPrincipal, SW_SHOW);
        UpdateWindow(hwndPrincipal);
        
        // Loop de mensagens
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#else
    std::cout << "[GUI] Mostrando janela: " << tituloJanela << std::endl;
    std::cout << "[GUI] Componentes: " << componentes.size() << std::endl;
    janelaVisivel = true;
#endif
}

void ServicoGUI::fechar() {
#ifdef _WIN32
    if (hwndPrincipal) {
        DestroyWindow(hwndPrincipal);
        hwndPrincipal = nullptr;
    }
#endif
    janelaVisivel = false;
}
