# ⚙️ Environment Setup – CCG

Este guia mostra como preparar ambiente, compilar e executar o CCG em Linux, Windows (MSYS2/MinGW) e macOS.

---

## 📜 Fluxo recomendado

No dia a dia, use **scripts em `scripts/`** como interface principal de build.

- `scripts/` = fluxo recomendado para dev
- `Makefile` = base interna de compilação

Padrão de nomes dos scripts:

- `mk` → build incremental (sem `clean`)
- `mkall` → build completo (`clean` + diretórios + build)
- `S` → build com SDL2
- `R` → build com raylib
- `d` → debug
- `f` → fake

Exemplos reais:

```bash
./scripts/mkallS_linux.sh   # all + SDL2
./scripts/mkdS_linux.sh      # incremental + debug + SDL2
./scripts/mkall_linux.sh     # all terminal
./scripts/mkallR_win32.sh    # all raylib (win32)
```

---

## 🐧 Linux

### 1) Dependências base

```bash
# Debian/Ubuntu
sudo apt update
sudo apt install build-essential gcc make libxml2-dev

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install libxml2-devel

# Arch
sudo pacman -S base-devel libxml2
```

### 2) Dependências opcionais por backend

```bash
# SDL2 (Debian/Ubuntu)
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

# raylib (Debian/Ubuntu; pacote pode variar por distro)
sudo apt install libraylib-dev
```

### 3) Build e execução (scripts)

```bash
# terminal
./scripts/mkall_linux.sh
./bin/card_game

# SDL2
./scripts/mkallS_linux.sh
./bin/card_game --sdl

# debug + SDL2
./scripts/mkdallS_linux.sh

# fake + terminal
./scripts/mkfall_linux.sh
```

### 4) Build direto com make (base interna)

```bash
make LINUX=1 all
make LINUX=1 USE_SDL2=1 all
make LINUX=1 USE_RAYLIB=1 all
make LINUX=1 DEBUG=1 all
make LINUX=1 FAKE=1 all
```

---

## 🪟 Windows (MSYS2/MinGW)

### 1) Instalar toolchain e libs

No terminal **MSYS2 MinGW64**:

```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S --needed mingw-w64-x86_64-libxml2

# SDL2
pacman -S --needed mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_image

# raylib
pacman -S --needed mingw-w64-x86_64-raylib
```

### 2) Build e execução (scripts)

```bash
# terminal
./scripts/mkall_win32.sh
./bin/card_game.exe

# SDL2
./scripts/mkallS_win32.sh
./bin/card_game.exe --sdl

# raylib
./scripts/mkallR_win32.sh
./bin/card_game.exe --raylib
```

### 3) Build direto com make

```bash
make _WIN32=1 all
make _WIN32=1 USE_SDL2=1 all
make _WIN32=1 USE_RAYLIB=1 all
```

Observações:
- Use shell **MinGW64** para compilar/rodar.
- `--sdl` e `--raylib` são mutuamente exclusivos.

---

## 🍎 macOS

Exemplo com Homebrew:

```bash
brew install gcc make libxml2 sdl2 sdl2_ttf sdl2_image raylib

# terminal
./scripts/mkall_apple.sh

# SDL2
./scripts/mkallS_apple.sh

# build via make
make APPLE=1 all
make APPLE=1 USE_SDL2=1 all
make APPLE=1 USE_RAYLIB=1 all
```

> No estado atual, raylib em macOS/Win32 pode exigir ajustes adicionais de ambiente.

---

## ✅ Resumo rápido

- Prefira `scripts/` para build diário
- `Makefile` é a base comum entre plataformas
- Backends de build:
  - terminal (padrão)
  - SDL2 (`S` / `USE_SDL2=1`)
  - raylib (`R` / `USE_RAYLIB=1`)
- Configuração runtime em XML dentro de `conf/`
