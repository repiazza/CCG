# 🃏 CCG – Card Game em C (Multi-Frontend)

Um jogo de cartas colecionáveis escrito em **C (C89/C99)**, com foco em código **leve, didático e open source**.

O projeto roda em arquitetura **multi-frontend**, permitindo executar o mesmo gameplay em:
- Terminal
- SDL2
- raylib

> A proposta é simples: manter o **core do jogo isolado** e permitir evolução da interface sem reescrever o gameplay.

---

# ✨ Features principais

- Sistema de jogadores e monstros  
- Decks configuráveis  
- Loja / mercado de cartas  
- Sistema de batalha  
- Interface via terminal (rápida e estável para testes)  
- Backend gráfico em evolução (**SDL2 / raylib**)  
- Estrutura modular em C (`src/` + `include/`)  
- Sistema de logs e trace (`log/card_game.log`)  

---

# ⚡ Quick start

## Build (recomendado via scripts)

```bash
./scripts/mkall_linux.sh
```

## Executar

```bash
./bin/card_game
```

## Testar frontends gráficos

```bash
./bin/card_game --sdl
# ou
./bin/card_game --raylib
```

---

# 🖥️ Backends

- **Terminal**  
  Funcional, usado como baseline e debug rápido  

- **SDL2**  
  Implementação mais completa atualmente (referência de comportamento)  

- **raylib**  
  Backend em evolução (MVP), seguindo a nova arquitetura  

---

# 🧩 Diferenciais técnicos

- Código em **C89/C99 puro**, com baixo acoplamento  
- Arquitetura preparada para múltiplos frontends  
- Build isonômico entre Linux, Windows (MinGW/MSYS2) e macOS  
- Scripts que simplificam o fluxo de compilação  
- Configuração dinâmica via XML (`conf/`)  
- Versionamento automatizado baseado em **Conventional Commits**  

---

# 📂 Estrutura do projeto

```
├── assets/           # imagens (.png)
├── conf/             # configurações XML
├── include/          # headers (.h)
├── src/              # implementações (.c)
├── fonts/            # fontes (.ttf)
├── scripts/          # scripts de build
├── doc/              # documentação
├── Makefile
└── README.md
```

Diretórios gerados em build:

```
├── build/            # includes auxiliares de make
├── obj/              # objetos (.o)
├── log/              # logs de execução
└── bin/              # executáveis
```

---

# 📚 Documentação

Para mais detalhes:

- ENVIRONMENT.md → setup e build  
- CONTRIBUTING.md → como contribuir  
- ARCHITECTURE.md → arquitetura do sistema  

---

# 🤝 Contribuindo

Contribuições são bem-vindas — desde correções simples até evolução de gameplay e frontends.

- Siga o padrão **Conventional Commits**
- Evite acoplar lógica de jogo a backend específico
- Teste impactos entre frontends

Veja mais em: CONTRIBUTING.md

---

# 🧠 Nota

Este README é o ponto de entrada do projeto.  
Para detalhes técnicos mais profundos (API de frontend, modelo de execução, etc.), consulte o `ARCHITECTURE.md`.

---

# 👴

> Projeto em evolução contínua — priorizando clareza, portabilidade e controle de baixo nível.
