// BR Package Marketplace - JavaScript
class BRMarketplace {
    constructor() {
        this.pacotes = [
            {
                id: 'br-web',
                nome: 'br-web',
                versao: '2.1.0',
                descricao: 'Framework web completo para BR com roteamento, templates e middleware.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'web',
                downloads: 12500,
                estrelas: 4.8,
                atualizado: '2026-01-15',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-sqlite',
                nome: 'br-sqlite',
                versao: '1.5.2',
                descricao: 'Wrapper nativo SQLite para BR com suporte a migrations e queries em português.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'dados',
                downloads: 8900,
                estrelas: 4.9,
                atualizado: '2026-01-10',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-gui',
                nome: 'br-gui',
                versao: '1.0.0',
                descricao: 'Biblioteca GUI nativa com suporte a Qt para aplicações desktop.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'gui',
                downloads: 6700,
                estrelas: 4.7,
                atualizado: '2026-01-08',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-ml',
                nome: 'br-ml',
                versao: '0.9.5',
                descricao: 'Biblioteca de Machine Learning com regressão, classificação e redes neurais.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'ia',
                downloads: 5400,
                estrelas: 4.6,
                atualizado: '2026-01-05',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-http',
                nome: 'br-http',
                versao: '1.3.0',
                descricao: 'Cliente HTTP completo com suporte a REST, WebSocket e streaming.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'rede',
                downloads: 11200,
                estrelas: 4.8,
                atualizado: '2026-01-12',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-test',
                nome: 'br-test',
                versao: '1.2.0',
                descricao: 'Framework de testes nativo com assertions, mocks e coverage.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'utilitarios',
                downloads: 7800,
                estrelas: 4.5,
                atualizado: '2026-01-03',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-orm',
                nome: 'br-orm',
                versao: '0.8.0',
                descricao: 'ORM para BR com suporte a múltiplos bancos de dados.',
                autor: 'Comunidade',
                avatar: 'CM',
                categoria: 'dados',
                downloads: 3200,
                estrelas: 4.3,
                atualizado: '2025-12-28',
                dependencias: ['br-sqlite'],
                licenca: 'Apache-2.0'
            },
            {
                id: 'br-auth',
                nome: 'br-auth',
                versao: '1.0.0',
                descricao: 'Sistema de autenticação com JWT, OAuth2 e sessões.',
                autor: 'Comunidade',
                avatar: 'CM',
                categoria: 'web',
                downloads: 4500,
                estrelas: 4.4,
                atualizado: '2026-01-01',
                dependencias: ['br-web'],
                licenca: 'MIT'
            },
            {
                id: 'br-async',
                nome: 'br-async',
                versao: '0.5.0',
                descricao: 'Runtime assíncrono com async/await e paralelismo.',
                autor: 'BR Team',
                avatar: 'BT',
                categoria: 'utilitarios',
                downloads: 6100,
                estrelas: 4.7,
                atualizado: '2026-01-07',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-csv',
                nome: 'br-csv',
                versao: '1.1.0',
                descricao: 'Parser e gerador de arquivos CSV para BR.',
                autor: 'Comunidade',
                avatar: 'CM',
                categoria: 'dados',
                downloads: 2800,
                estrelas: 4.2,
                atualizado: '2025-12-20',
                dependencias: [],
                licenca: 'MIT'
            },
            {
                id: 'br-telegram',
                nome: 'br-telegram',
                versao: '0.3.0',
                descricao: 'Bot API para Telegram com suporte a webhooks.',
                autor: 'Comunidade',
                avatar: 'CM',
                categoria: 'rede',
                downloads: 1900,
                estrelas: 4.1,
                atualizado: '2025-12-15',
                dependencias: ['br-http'],
                licenca: 'MIT'
            },
            {
                id: 'br-plot',
                nome: 'br-plot',
                versao: '0.2.0',
                descricao: 'Biblioteca de visualização de dados com gráficos.',
                autor: 'Comunidade',
                avatar: 'CM',
                categoria: 'dados',
                downloads: 2400,
                estrelas: 4.0,
                atualizado: '2025-12-10',
                dependencias: [],
                licenca: 'MIT'
            }
        ];
        
        this.filtroAtual = '';
        this.categoriaAtual = '';
        this.ordenacaoAtual = 'popular';
        
        this.inicializar();
    }
    
    inicializar() {
        this.renderizarPacotes();
        this.inicializarEventos();
    }
    
    inicializarEventos() {
        // Busca
        document.getElementById('search-input').addEventListener('input', (e) => {
            this.filtroAtual = e.target.value.toLowerCase();
            this.renderizarPacotes();
        });
        
        // Filtro de categoria
        document.getElementById('category-filter').addEventListener('change', (e) => {
            this.categoriaAtual = e.target.value;
            this.renderizarPacotes();
        });
        
        // Ordenação
        document.getElementById('sort-select').addEventListener('change', (e) => {
            this.ordenacaoAtual = e.target.value;
            this.renderizarPacotes();
        });
        
        // Links de categoria na sidebar
        document.querySelectorAll('.category-list a').forEach(link => {
            link.addEventListener('click', (e) => {
                e.preventDefault();
                const categoria = link.getAttribute('data-category');
                document.getElementById('category-filter').value = categoria;
                this.categoriaAtual = categoria;
                this.renderizarPacotes();
            });
        });
        
        // Fechar modal
        document.querySelector('.close').addEventListener('click', () => {
            document.getElementById('package-modal').style.display = 'none';
        });
        
        window.addEventListener('click', (e) => {
            if (e.target.classList.contains('modal')) {
                e.target.style.display = 'none';
            }
        });
    }
    
    filtrarPacotes() {
        return this.pacotes.filter(pacote => {
            const matchBusca = !this.filtroAtual || 
                pacote.nome.toLowerCase().includes(this.filtroAtual) ||
                pacote.descricao.toLowerCase().includes(this.filtroAtual);
            
            const matchCategoria = !this.categoriaAtual || 
                pacote.categoria === this.categoriaAtual;
            
            return matchBusca && matchCategoria;
        });
    }
    
    ordenarPacotes(pacotes) {
        switch (this.ordenacaoAtual) {
            case 'popular':
                return pacotes.sort((a, b) => b.estrelas - a.estrelas);
            case 'recentes':
                return pacotes.sort((a, b) => new Date(b.atualizado) - new Date(a.atualizado));
            case 'nome':
                return pacotes.sort((a, b) => a.nome.localeCompare(b.nome));
            case 'downloads':
                return pacotes.sort((a, b) => b.downloads - a.downloads);
            default:
                return pacotes;
        }
    }
    
    renderizarPacotes() {
        const grid = document.getElementById('packages-grid');
        let pacotesFiltrados = this.filtrarPacotes();
        pacotesFiltrados = this.ordenarPacotes(pacotesFiltrados);
        
        if (pacotesFiltrados.length === 0) {
            grid.innerHTML = `
                <div class="no-results">
                    <h3>Nenhum pacote encontrado</h3>
                    <p>Tente ajustar os filtros de busca.</p>
                </div>
            `;
            return;
        }
        
        grid.innerHTML = pacotesFiltrados.map(pacote => this.criarCardPacote(pacote)).join('');
        
        // Adicionar eventos de clique nos cards
        document.querySelectorAll('.package-card').forEach(card => {
            card.addEventListener('click', () => {
                const pacoteId = card.getAttribute('data-id');
                this.abrirDetalhesPacote(pacoteId);
            });
        });
    }
    
    criarCardPacote(pacote) {
        const icone = this.obterIconeCategoria(pacote.categoria);
        const downloadsFormatados = this.formatarNumero(pacote.downloads);
        
        return `
            <div class="package-card" data-id="${pacote.id}">
                <div class="package-header">
                    <div class="package-icon">${icone}</div>
                    <div>
                        <div class="package-name">${pacote.nome}</div>
                        <span class="package-version">v${pacote.versao}</span>
                    </div>
                </div>
                <div class="package-description">${pacote.descricao}</div>
                <div class="package-meta">
                    <div class="package-stats">
                        <span>⭐ ${pacote.estrelas}</span>
                        <span>📥 ${downloadsFormatados}</span>
                    </div>
                    <div class="package-author">
                        <div class="author-avatar">${pacote.avatar}</div>
                        <span>${pacote.autor}</span>
                    </div>
                </div>
            </div>
        `;
    }
    
    obterIconeCategoria(categoria) {
        const icones = {
            web: '🌐',
            dados: '📊',
            gui: '🖼️',
            ia: '🤖',
            rede: '🔗',
            utilitarios: '🔧'
        };
        return icones[categoria] || '📦';
    }
    
    formatarNumero(numero) {
        if (numero >= 1000) {
            return (numero / 1000).toFixed(1) + 'k';
        }
        return numero.toString();
    }
    
    abrirDetalhesPacote(pacoteId) {
        const pacote = this.pacotes.find(p => p.id === pacoteId);
        if (!pacote) return;
        
        const modal = document.getElementById('package-modal');
        const detalhes = document.getElementById('package-details');
        
        const icone = this.obterIconeCategoria(pacote.categoria);
        const downloadsFormatados = this.formatarNumero(pacote.downloads);
        
        detalhes.innerHTML = `
            <div class="package-detail-header">
                <div class="package-detail-icon">${icone}</div>
                <div class="package-detail-info">
                    <h2>${pacote.nome}</h2>
                    <span class="version">v${pacote.versao}</span>
                    <div class="author">Por ${pacote.autor}</div>
                </div>
            </div>
            
            <div class="package-detail-description">
                ${pacote.descricao}
            </div>
            
            <div class="package-detail-section">
                <h3>Instalação</h3>
                <div class="install-command">br instalar ${pacote.nome}</div>
            </div>
            
            <div class="package-detail-section">
                <h3>Estatísticas</h3>
                <ul>
                    <li>⭐ Avaliação: ${pacote.estrelas}/5.0</li>
                    <li>📥 Downloads: ${downloadsFormatados}</li>
                    <li>📅 Atualizado: ${new Date(pacote.atualizado).toLocaleDateString('pt-BR')}</li>
                    <li>📄 Licença: ${pacote.licenca}</li>
                </ul>
            </div>
            
            <div class="package-detail-section">
                <h3>Categoria</h3>
                <ul>
                    <li>${pacote.categoria.charAt(0).toUpperCase() + pacote.categoria.slice(1)}</li>
                </ul>
            </div>
            
            ${pacote.dependencias.length > 0 ? `
                <div class="package-detail-section">
                    <h3>Dependências</h3>
                    <ul>
                        ${pacote.dependencias.map(dep => `<li>${dep}</li>`).join('')}
                    </ul>
                </div>
            ` : ''}
            
            <button class="install-btn" onclick="window.location.href='br://instalar/${pacote.nome}'">
                📥 Instalar Pacote
            </button>
        `;
        
        modal.style.display = 'block';
    }
}

// Inicializar marketplace quando DOM estiver pronto
document.addEventListener('DOMContentLoaded', () => {
    window.marketplace = new BRMarketplace();
});
