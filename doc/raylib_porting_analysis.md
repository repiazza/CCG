# Análise de portabilidade para Raylib (mantendo Terminal + SDL2)

## Objetivo

Definir um plano de migração para adicionar **Raylib** ao CCG sem regressão e sem duplicação estrutural, mantendo suporte oficial aos 3 modos:

1. **Terminal**
2. **SDL2**
3. **Raylib**

Princípios obrigatórios deste plano:

- evitar fork permanente de código entre SDL2 e Raylib;
- centralizar regras de jogo em módulos compartilhados;
- reduzir `#ifdef` espalhado pelo domínio de gameplay.

---

## Estado atual (resumo técnico)

- O build atual separa terminal e SDL2 via `USE_SDL2` no `Makefile`.
- O runtime já seleciona GUI com `--sdl` e mantém terminal como fallback.
- Há acoplamento forte a SDL em headers/módulos gráficos (`sdl_api`, `event`, `event_render`, `image`, `font`).
- O modo terminal já é um fluxo separado (`vCNSL_MainLoop`), o que ajuda na coexistência dos 3 modos.

Conclusão: a base é adequada para evolução, mas precisa de **API de frontend única** antes de incluir Raylib.

---

## Política de build e seleção de backend (compile-time vs runtime)

## Compile-time

Flags de compilação recomendadas:

- `USE_SDL2=1` habilita backend SDL2
- `USE_RAYLIB=1` habilita backend Raylib

Política recomendada:

- **Desenvolvimento**: permitir binário com SDL2+Raylib simultaneamente (quando ambiente suportar).
- **Release**: permitir também builds separados por backend, se desejado para simplificar distribuição.

## Runtime

Seleção em linha de comando:

- manter `--sdl` (compatibilidade)
- adicionar `--raylib`
- opcional: `--backend=terminal|sdl2|raylib` (forma canônica futura)

Regras:

- `--sdl` + `--raylib` no mesmo comando => **erro explícito de uso**.
- sem flags => **terminal permanece default**.
- backend solicitado mas não compilado => erro amigável informando flags disponíveis.

---

## Arquitetura alvo: separação em 3 eixos

A API de frontend deve separar explicitamente:

1. **Render** (desenho)
2. **Input** (teclado/mouse)
3. **Loop/Timing** (delta time, quit, pacing/fps)

Motivação técnica:

- SDL2 é orientado a fila de eventos (`SDL_PollEvent`).
- Raylib opera majoritariamente em polling de estado por frame (`IsKeyPressed`, `GetMousePosition`, etc.).
- Sem separar loop/timing, comportamento de input e responsividade divergem entre backends.

---

## Modelo de eventos lógicos (game events) — obrigatório

Módulos compartilhados do jogo **não podem consumir** `SDL_Event` nem tipos Raylib.

Deve existir um modelo lógico comum (exemplo mínimo):

- `CCG_EVT_NONE`
- `CCG_EVT_CONFIRM`
- `CCG_EVT_CANCEL`
- `CCG_EVT_UP`
- `CCG_EVT_DOWN`
- `CCG_EVT_LEFT`
- `CCG_EVT_RIGHT`
- `CCG_EVT_CLICK`
- `CCG_EVT_QUIT`

Sugestão de payload mínimo para clique:

- `x`, `y`, `button`

## Tabela de mapeamento (exemplo inicial)

| Evento lógico | SDL2 | Raylib |
|---|---|---|
| `CCG_EVT_CONFIRM` | `SDL_KEYDOWN` + `SDLK_RETURN` | `IsKeyPressed(KEY_ENTER)` |
| `CCG_EVT_CANCEL` | `SDL_KEYDOWN` + `SDLK_ESCAPE` | `IsKeyPressed(KEY_ESCAPE)` |
| `CCG_EVT_UP` | `SDL_KEYDOWN` + `SDLK_UP` | `IsKeyPressed(KEY_UP)` |
| `CCG_EVT_DOWN` | `SDL_KEYDOWN` + `SDLK_DOWN` | `IsKeyPressed(KEY_DOWN)` |
| `CCG_EVT_LEFT` | `SDL_KEYDOWN` + `SDLK_LEFT` | `IsKeyPressed(KEY_LEFT)` |
| `CCG_EVT_RIGHT` | `SDL_KEYDOWN` + `SDLK_RIGHT` | `IsKeyPressed(KEY_RIGHT)` |
| `CCG_EVT_CLICK` | `SDL_MOUSEBUTTONDOWN` (botão esquerdo) | `IsMouseButtonPressed(MOUSE_LEFT_BUTTON)` |
| `CCG_EVT_QUIT` | `SDL_QUIT` | `WindowShouldClose()` (checado a cada frame) |

Resultado esperado: gameplay processa apenas `CCG_Event`.

---

## Regra de isolamento de tipos em headers

Regra de projeto:

- **Nenhum header compartilhado** pode expor `SDL_*` ou `Raylib_*`.

Tipos comuns mínimos expostos:

- `CCG_Rect { int x, y, w, h; }`
- `CCG_Color { uint8_t r, g, b, a; }`
- handles opacos de recurso:
  - `typedef struct CCG_Tex* CCG_TexH;`
  - `typedef struct CCG_Font* CCG_FontH;`

SDL2 e Raylib devem ficar restritos aos `.c` dos backends.

---

## API genérica de frontend (render + input + loop)

Criar `frontend_api.h` com contrato único, por exemplo:

- ciclo de vida: `init`, `shutdown`
- frame: `begin_frame`, `draw_*`, `end_frame`
- input lógico (preferência de simplicidade): `poll_event(CCG_Event* ev)` retornando `0/1`
  - opcional para batching futuro: `poll_events(out, max)`
- loop/timing: `should_quit`, `get_delta_time`, `set_target_fps`
  - em Raylib, `should_quit()` deve refletir `WindowShouldClose()` verificado por frame
- recursos: `load_texture`, `unload_texture`, `load_font`, `unload_font`

Implementações:

- `frontend_sdl2.c`
- `frontend_raylib.c`

O modo terminal permanece fora da API gráfica, mas deve expor/produzir os mesmos eventos lógicos `CCG_EVT_*` para unificar o processamento de gameplay/input.

---

## Assets e ciclo de vida de recursos (obrigatório)

Definir propriedade e descarte por backend:

- cada backend é dono de seus handles (`CCG_TexH`, `CCG_FontH`);
- criação e destruição ocorrem **no backend que alocou**.

Pontos de descarte obrigatórios:

1. **shutdown do jogo** (sempre)
2. **troca de tela/layout** quando recurso não for mais necessário
3. **reload de configuração** (se aplicável)

Pontos de atenção:

- SDL2 e Raylib possuem APIs e contratos de recurso diferentes.
- não compartilhar ponteiro nativo entre backends.
- manter rastreamento explícito de ownership para evitar leak/double free.

---

## MVP definido: “Raylib backend mínimo”

Escopo mínimo para considerar backend Raylib funcional (sem paridade total ainda):

1. criar janela, loop básico e encerramento (`quit`)
2. mapear input lógico mínimo:
   - confirmar/cancelar
   - setas
   - clique esquerdo
3. render mínimo de fluxo jogável:
   - tela de boas-vindas
   - mesa principal com HUD básico
   - diálogo textual essencial
4. integração com gameplay existente **sem alterar regras**

Fora do MVP inicial:

- animações avançadas
- efeitos de popup/flash completos
- otimizações de batching e refinamentos visuais

---

## Estratégia recomendada (ordem de implementação)

1. **Backend selection no `main` + cmdline**
   - adicionar `--raylib`, manter `--sdl`, tratar conflito como erro.

2. **Criar `frontend_api.h` + tipos comuns**
   - separar render/input/loop-timing.

3. **Adaptar SDL2 para a interface genérica primeiro**
   - sem alterar gameplay.

4. **Implementar backend Raylib mínimo na mesma interface**
   - loop + input lógico + render mínimo.

5. **Migrar loaders de imagem/fonte para backend-aware**
   - com ownership e unload explícitos.

6. **Expandir recursos + scripts/matriz de teste**
   - animações, HUD completo, paridade visual e validação cruzada.

> Correção de estratégia aplicada: não adotar “espelhar SDL e depois unificar”. A unificação vem primeiro.

---

## Checklist de validação (aceite)

### Build/execução

- [ ] Build terminal sem `USE_SDL2` e sem `USE_RAYLIB` compila e executa.
- [ ] Build com `USE_SDL2=1` compila e `--sdl` inicia GUI SDL2.
- [ ] Build com `USE_RAYLIB=1` compila e `--raylib` inicia GUI Raylib.
- [ ] Build com `USE_SDL2=1 USE_RAYLIB=1` (dev) permite selecionar backend em runtime.

### Regras de seleção

- [ ] Sem flags de backend => terminal é default.
- [ ] `--sdl` + `--raylib` => erro explícito.
- [ ] backend não compilado => erro amigável.

### Compatibilidade funcional mínima

- [ ] Entrada de ações principais (confirm/cancel/setas/click) consistente entre SDL2 e Raylib.
- [ ] Loop/timing não altera regras de turno e resolução de combate.
- [ ] Recursos (textura/fonte) são descarregados sem leaks aparentes no encerramento.

---

## Riscos e mitigação

Riscos principais:

1. divergência de comportamento por diferenças de input model (event queue vs polling);
2. regressão no fluxo principal por mistura de código de backend com gameplay;
3. aumento de `#ifdef` fora da camada de backend.

Mitigações:

- API única de frontend com eventos lógicos;
- isolamento de tipos em headers compartilhados;
- validação incremental com matriz terminal/SDL2/Raylib desde o início.

---

## Conclusão

A migração sustentável para 3 modos depende de uma decisão arquitetural: **API genérica primeiro, Raylib depois**. Isso reduz duplicação, evita fork permanente SDL-vs-Raylib e preserva manutenção de longo prazo mantendo Terminal como fallback padrão.
