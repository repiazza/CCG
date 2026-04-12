# 🏗️ ARCHITECTURE – CCG

## 🧠 Visão geral

O CCG é uma engine de card game em **C (C89/C99)** com arquitetura **multi-frontend**.
A direção técnica do projeto é desacoplar cada vez mais:

- **core de jogo** (regras, entidades, fluxo)
- **frontends** (input, render, loop de plataforma)

Com isso, o mesmo core pode evoluir sem dependência rígida de um único backend gráfico.

---

## 🧩 API de frontend

A base da arquitetura é a interface `STRUCT_FRONTEND_API`, usada para padronizar o ciclo de execução entre backends.

Ciclo principal da API:

- `init`
- `pollEvent`
- `beginFrame`
- `endFrame`
- `shouldQuit`
- `shutdown`

No estado atual, **SDL2**, **raylib** e **Terminal** implementam essa API em níveis diferentes de maturidade.

---

## 🧱 Estado atual dos backends

- **SDL2**
  - Backend mais completo no momento
  - Implementação de referência funcional (gameplay + render/UI mais madura)

- **raylib**
  - Backend em estágio MVP
  - Já usa o modelo novo de ciclo por eventos/frame
  - Ainda não possui paridade funcional completa com SDL2

- **Terminal**
  - Continua funcional
  - Útil para baseline/debug e validação rápida de fluxo

---

## ⚙️ Modelo de execução

Atualmente coexistem dois modelos.

### 1) Modelo legado (predominante)

Loops específicos por backend:

- `vSDL_MainLoop`
- `vCNSL_MainLoop`

Esse modelo ainda sustenta partes importantes da execução principal.

### 2) Modelo novo (em evolução)

Modelo baseado em `STRUCT_FRONTEND_API`, orientado a eventos/frame.

- Já utilizado no backend raylib
- Parcialmente adotado em SDL2/Terminal (inicialização e integração progressiva)

---

## 🎯 Direção arquitetural

A migração em andamento segue os pontos abaixo:

- convergência para um modelo unificado de frontend
- remoção gradual dos loops legados
- raylib como implementação alinhada ao modelo novo
- SDL2 como base funcional durante a transição

Importante: essa convergência ainda não está concluída.

---

## 🔧 Sistema de build

- **Makefile** é a base de build do projeto
- **scripts/** é a interface de uso recomendada para desenvolvimento diário
- Há padronização por plataforma e perfil (Linux/Win32/Apple, debug/fake, backend)

Exemplos de chaves de build no Makefile:

- `USE_SDL2=1`
- `USE_RAYLIB=1`
- `DEBUG=1`
- `FAKE=1`

---

## ⚙️ Configuração

A configuração de runtime é feita por XML em `conf/`.

Exemplos:

- `conf/ccg.xml`
- `conf/screen.xml`
- `conf/msg.xml`
- `conf/image.xml`

Princípios atuais:

- paths e parâmetros de execução configuráveis
- separação entre configuração externa e código-fonte

---

## 🔁 CI/CD e versionamento

O fluxo de versionamento segue **Conventional Commits**.

Impacto semântico esperado dos tipos:

- `feat` → incremento **minor**
- `fix` → incremento **patch**
- `BREAKING CHANGE` (ou `!`) → incremento **major**

Esse padrão viabiliza versionamento/release automatizados em pipelines de CI/CD.
