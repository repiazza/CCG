# 🃏 CCG – Card Game Engine em C

CCG é um jogo/engine de cartas em **C (C89/C99)** com suporte a múltiplos frontends.
O projeto busca ser **leve, didático e open source**, mantendo gameplay estável enquanto a camada gráfica evolui.

---

## ✨ Features principais

- Sistema de jogadores e monstros
- Decks configuráveis
- Loja de cartas
- Módulo de batalha
- Modo terminal funcional
- Backend gráfico em evolução (SDL2/raylib)
- Configuração externa via XML (`conf/`)

---

## ⚡ Quick start

Fluxo recomendado (Linux):

```bash
./scripts/mkall_linux.sh
./bin/card_game
```

Para outros cenários de build, consulte `ENVIRONMENT.md`.

---

## 🖥️ Backends (resumo)

- **Terminal** → baseline/debug funcional
- **SDL2** → backend mais completo atualmente
- **raylib** → backend MVP em evolução

Seleção em runtime (quando compilado com o backend correspondente):

```bash
./bin/card_game --sdl
./bin/card_game --raylib
```

---

## 📚 Documentação

- [ENVIRONMENT.md](ENVIRONMENT.md) – setup, dependências e build
- [CONTRIBUTING.md](CONTRIBUTING.md) – fluxo de colaboração
- [ARCHITECTURE.md](ARCHITECTURE.md) – arquitetura técnica detalhada

---

## 📦 CI/CD

Versionamento automatizado baseado em **Conventional Commits**.
