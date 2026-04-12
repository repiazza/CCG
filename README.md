# 🃏 CCG – Card Game Engine (C) com Multi-Frontend

Projeto de jogo de cartas colecionáveis em **C (C89/C99)** em transição para uma arquitetura
**multi-frontend**. O objetivo atual é manter o gameplay estável enquanto a camada de frontend
é unificada entre **Terminal**, **SDL2** e **raylib**.

---

## 📌 Estado atual (source of truth)

- Seleção de backend em runtime via CLI:
  - `--sdl`
  - `--raylib`
- Validação de conflito entre `--sdl` e `--raylib`
- Três fluxos de execução existentes:
  - **Terminal** (funcional, baseline/debug)
  - **SDL2** (implementação mais completa no momento)
  - **raylib** (MVP em evolução)
- Build com Makefile para Linux / Win32 / Apple
- Flags de build por backend:
  - `USE_SDL2=1`
  - `USE_RAYLIB=1`
- Modos de build adicionais:
  - `DEBUG=1`
  - `FAKE=1`
- Configuração em XML em `conf/` (ex.: `ccg.xml`, `screen.xml`, `msg.xml`, `image.xml`)
- Scripts de compilação/portabilidade em `scripts/`

---

## 🧱 Arquitetura (legado + nova API)

Hoje coexistem dois modelos:

### 1) Modelo legado (predominante)

Loops específicos por backend, ainda responsáveis por boa parte da execução principal:
- `vSDL_MainLoop`
- `vCNSL_MainLoop`

### 2) Modelo novo (em evolução)

API de frontend orientada a ciclo de frame/eventos, com funções no formato:
- `init`
- `pollEvent`
- `beginFrame`
- `endFrame`
- `shouldQuit`
- `shutdown`

Situação atual:
- **raylib** já opera nesse modelo novo (MVP)
- **SDL2** e **Terminal** já inicializam via API, mas ainda preservam loops legados em partes do fluxo

---

## 🎯 Direção arquitetural

- Convergir os frontends para um modelo unificado de eventos/frame
- Preservar o gameplay/core isolado da camada de render/input
- Manter **SDL2** como base funcional de referência durante a migração
- Evoluir **raylib** como direção arquitetural de frontend
- Manter **Terminal** como modo funcional e útil para debug/baseline

> Importante: a migração para o modelo totalmente unificado **ainda não está concluída**.

---

## 🖥️ Backends e papéis atuais

### Terminal
- Continua funcional
- Usado como baseline/debug
- Mantém importância para testes rápidos e validação de fluxo

### SDL2
- Implementação mais completa atualmente
- Contém fluxo de gameplay/render/UI mais maduro
- Referência prática para comportamento do jogo durante a transição

### raylib
- Backend em estágio MVP
- Já possui janela + loop + integração básica de eventos
- Ainda não cobre todo o escopo funcional da versão SDL2

---

## 📂 Estrutura do projeto

```text
├── assets/           # imagens (.png)
├── conf/             # XMLs de configuração
├── include/          # cabeçalhos (.h)
├── src/              # implementações (.c)
├── fonts/            # fontes (.ttf)
├── scripts/          # scripts de build/portabilidade
├── doc/              # documentação
├── Makefile          # build principal
├── .gitignore
└── README.md
```

Diretórios temporários de build/execução:

```text
├── build/            # inclusão dinâmica de makefiles auxiliares
├── obj/              # objetos (.o)
├── log/              # logs
└── bin/              # executáveis
```

---

## 🛠️ Como compilar

Requisitos: GCC ou Clang + Make.

### Linux

```bash
# Build padrão (terminal)
make LINUX=1 all

# Build SDL2
make LINUX=1 USE_SDL2=1 all

# Build raylib
make LINUX=1 USE_RAYLIB=1 all

# Debug
make LINUX=1 DEBUG=1 all

# Fake
make LINUX=1 FAKE=1 all
```

### Windows (MinGW/MSYS2)

```bash
# Build padrão (terminal)
make _WIN32=1 all

# Build SDL2
make _WIN32=1 USE_SDL2=1 all

# Build raylib
make _WIN32=1 USE_RAYLIB=1 all
```

### Apple

```bash
# Build padrão (terminal)
make APPLE=1 all

# Build SDL2
make APPLE=1 USE_SDL2=1 all

# Build raylib
make APPLE=1 USE_RAYLIB=1 all
```

Também é possível usar os atalhos de `scripts/` (ex.: `mkall_linux.sh`, `mkallS_linux.sh`, etc.).

---

## ▶️ Execução

```bash
# Padrão (terminal)
./bin/card_game

# Solicitar backend SDL2
./bin/card_game --sdl

# Solicitar backend raylib
./bin/card_game --raylib
```

Notas:
- `--sdl` e `--raylib` são mutuamente exclusivos.
- Se um backend for solicitado sem ter sido compilado no binário atual,
  o programa informa erro e encerra.

---

## 🧪 Testes e cobertura

```bash
make test
make coverage
make coverage-html
```

---

## 🤝 Contribuição

Consulte [CONTRIBUTING.md](CONTRIBUTING.md).

Resumo rápido:
- Use Conventional Commits
- Evite acoplar core de jogo a backend específico
- Ao alterar frontend, informe claramente o backend impactado
- Não quebre fluxos de outros backends
