# ⚙️ Environment Setup – CCG (Card Game Engine Multi-Frontend)

Este guia mostra como preparar o ambiente para compilar e rodar o **CCG**
em **Linux** e **Windows (MinGW/Cygwin/MSYS2)** usando `make`.

Atualmente, o projeto possui builds com foco em três modos:
- Terminal (padrão)
- SDL2 (`USE_SDL2=1`)
- raylib (`USE_RAYLIB=1`)

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

### 2. Dependências opcionais por backend

```bash
# SDL2 (Debian/Ubuntu)
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev

# raylib (Debian/Ubuntu; pacote pode variar por distro)
sudo apt install libraylib-dev
```

> Observação: nomes de pacotes podem variar por distribuição.

### 3. Clonar e compilar

```bash
git clone https://github.com/repiazza/CCG.git
cd CCG

# Terminal (padrão)
make LINUX=1 all
./bin/card_game
```

### 4. Builds por backend

```bash
# SDL2
make LINUX=1 USE_SDL2=1 all
./bin/card_game --sdl

# raylib
make LINUX=1 USE_RAYLIB=1 all
./bin/card_game --raylib
```

### 5. Opções extras

```bash
# Debug
make LINUX=1 DEBUG=1 all

# Fake
make LINUX=1 FAKE=1 all

# Limpeza
make clean
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

### 2. Instalar toolchain e libs

No terminal **MSYS2 MinGW64**:

```bash
pacman -Syu
pacman -S --needed base-devel mingw-w64-x86_64-toolchain
pacman -S --needed mingw-w64-x86_64-libxml2
```

Dependências opcionais por backend:

```bash
# SDL2
pacman -S --needed mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_image

# raylib
pacman -S --needed mingw-w64-x86_64-raylib
```

### 2) Build e execução (scripts)

```bash
git clone https://github.com/repiazza/CCG.git
cd CCG

# Terminal (padrão)
make _WIN32=1 all
./bin/card_game.exe

# SDL2
./scripts/mkallS_win32.sh
./bin/card_game.exe --sdl

# raylib
./scripts/mkallR_win32.sh
./bin/card_game.exe --raylib
```

### 4. Builds por backend

```bash
# SDL2
make _WIN32=1 USE_SDL2=1 all
./bin/card_game.exe --sdl

# raylib
make _WIN32=1 USE_RAYLIB=1 all
./bin/card_game.exe --raylib
```

### 5. Observações

- O Makefile detecta `mingw32/cygwin` via `gcc -dumpmachine`.
- Use **MinGW64** para build/execução, não o shell MSYS puro.
- `--sdl` e `--raylib` são mutuamente exclusivos em runtime.
- Se um backend não foi compilado no binário, a seleção via CLI falha com mensagem de erro.

---

## 🍎 Apple (macOS)

Exemplo base com Homebrew:

```bash
brew install gcc make libxml2 sdl2 sdl2_ttf sdl2_image raylib

# Terminal
make APPLE=1 all

# SDL2
make APPLE=1 USE_SDL2=1 all

# raylib
make APPLE=1 USE_RAYLIB=1 all
```

> No estado atual, flags/bibliotecas de raylib no macOS/Win32 podem exigir ajustes adicionais no ambiente.

---

## 📜 Scripts de build

A pasta `scripts/` contém atalhos para build por plataforma e perfil:

- `mkall_linux.sh`, `mkall_win32.sh`, `mkall_apple.sh`
- variantes SDL (`mkallS_*`), raylib (`mkallR_*` em Win32), debug (`mkd*`), fake (`mkf*`), debug+fake (`mkfd*`)

Exemplos:

```bash
./scripts/mkall_linux.sh
./scripts/mkallS_linux.sh
./scripts/mkallR_win32.sh
```

---

## ✅ Resumo rápido

- **Terminal:** build padrão (sem `USE_*`)
- **SDL2:** build com `USE_SDL2=1`
- **raylib:** build com `USE_RAYLIB=1`
- Configurações de runtime em XML dentro de `conf/`
- Portabilidade operacional via Makefile + scripts de `scripts/`
