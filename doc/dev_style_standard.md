# Padrão de Código CCG (C ANSI/C99)

Este projeto segue regras obrigatórias de estilo e compatibilidade para manter portabilidade (Linux/macOS/Win32) e consistência do código.

## Versão de C
- Alvo: ANSI C / C99.
- Evitar extensões de compilador sem `#ifdef` e fallback.

## Indentação e blocos
- Indentação: **2 espaços**.
- Chaves no mesmo estilo do código existente.
- Linhas e blocos devem permanecer legíveis; evitar “one-liners” complexos.

## Declarações (REGRA CRÍTICA)
- **NADA de declarar fora do escopo de declarações.**
- Declarar variáveis **no começo do escopo** (início de função e início de blocos relevantes).
- **EXPRESSAMENTE PROIBIDO**: `for (int i = 0; ...)`
  - Correto:
    - `int ii;`
    - `for (ii = 0; ii < n; ii++) { ... }`

## Notação Húngara (obrigatória)
- CamelCase com prefixos compostos por tipo + qualificador.
- Prefixos normativos:
  - `i` int
  - `c` char
  - `l` long
  - `f` float
  - `d` double
  - `p` ponteiro
  - `st` struct
  - `sz` string terminada em zero
  - `a` array (aglutinações estáticas)
  - `g` global
  - `u` unsigned
  - `k` const

## Composições comuns
- `psz` ponteiro para string: `char *pszBuffer;`
- `kpsz` ponteiro const para string: `const char *kpszName;`
- `gi` inteiro global: `int giFlag;`
- `ul` unsigned long: `unsigned long uiValue;`
- `ast` array estático de structs: `STRUCT_PLAYER astPlayer[MAX_PLAYER];`
- Ponteiros para struct tipada:
  - `PSTRUCT_PLAYER pstPlayer;` (ponteiro de STRUCT_PLAYER)

## Tipos, typedefs e defines
- `typedef` e nomes de `struct` **sempre em MAIÚSCULO** para o tipo:
  - `typedef struct STRUCT_PLAYER { ... } STRUCT_PLAYER, *PSTRUCT_PLAYER;`
- `#define` sempre MAIÚSCULO:
  - `#define MAX_PLAYER 10`

## Headers: isolamento de backend (importante para SDL2/Raylib)
- Headers compartilhados **não podem** expor tipos `SDL_*` nem `Raylib_*`.
- Tipos/handles gráficos devem ser opacos ou wrappers do projeto.
- Tipos nativos SDL/Raylib ficam **somente** nos `.c` do backend.

## Mudanças e commits
- Evitar “reformat” do código antigo.
- Código novo deve seguir este padrão.
- Commits pequenos, com escopo claro (sem misturar refactor + feature).
