# 🤝 Guia de Contribuição

Obrigado por contribuir com o **CCG**.

Este repositório está em transição para arquitetura **multi-frontend**. Hoje coexistem:
- lógica de jogo/core
- frontends Terminal, SDL2 e raylib
- partes legadas e partes já migradas para API de frontend unificada

O objetivo das contribuições é evoluir o projeto sem regressões entre backends.

---

## 🚀 Fluxo de contribuição

1. Faça um fork do repositório
2. Crie um branch descritivo:
   ```bash
   git checkout -b feature/minha-feature
   ```
3. Implemente sua alteração
4. Compile e valide o(s) backend(s) impactado(s)
5. Abra um Pull Request com contexto técnico claro

---

## 🧾 Padrão de commits (Conventional Commits)

Utilizamos [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/):

```text
<tipo>[escopo opcional]: <descrição>

[corpo opcional]

[rodapé(s) opcional(is)]
```

### Tipos
- `feat`: nova funcionalidade
- `fix`: correção de bug
- `docs`: documentação
- `style`: formatação (sem mudança de comportamento)
- `refactor`: refatoração sem nova feature/fix
- `perf`: melhoria de performance
- `test`: testes
- `build`: build/dependências
- `ci`: integração contínua
- `chore`: manutenção geral

### Exemplos
```bash
feat(frontend-raylib): adicionar mapeamento de evento de teclado
fix(core): corrigir cálculo de dano crítico
docs(readme): atualizar seção de backends e migração
feat!: alterar contrato da API de frontend
```

Use `!` ou `BREAKING CHANGE:` quando houver quebra de compatibilidade.

---

## 🧱 Diretrizes de arquitetura para PRs

### 1) Separe core de frontend

- Evite acoplar regra de negócio ao backend gráfico.
- Código de gameplay deve permanecer no core sempre que possível.
- Rendering/input/backend-specific deve ficar isolado em módulos de frontend.

### 2) Não quebre outros backends

Ao alterar qualquer parte de execução, valide impactos em:
- Terminal
- SDL2
- raylib

Se a mudança for específica de backend, deixe explícito no PR e no commit scope.

### 3) Respeite o estado de maturidade dos backends

- **SDL2**: implementação funcional mais completa hoje.
- **raylib**: backend em evolução (MVP), ainda em convergência.
- **Terminal**: permanece funcional e relevante para baseline/debug.

### 4) Migração para API unificada

A migração para modelo de frontend unificado está em andamento.
Contribuições são bem-vindas para reduzir dependência de loops legados,
mas sem quebrar o comportamento atual.

---

## 🧪 Validação mínima antes do PR

Use scripts sempre que possível:

```bash
./scripts/mkall_linux.sh
./scripts/mkallS_linux.sh
```

E execute ao menos os builds relacionados ao que foi alterado.

### Linux
```bash
# terminal
make LINUX=1 all

# SDL2
make LINUX=1 USE_SDL2=1 all

# raylib
make LINUX=1 USE_RAYLIB=1 all
```

### Win32 (MinGW)
```bash
# terminal
make _WIN32=1 all

# SDL2
make _WIN32=1 USE_SDL2=1 all

# raylib
make _WIN32=1 USE_RAYLIB=1 all
```

### Perfis adicionais
```bash
make LINUX=1 DEBUG=1 all
make LINUX=1 FAKE=1 all
```

Se não conseguir validar algum backend por limitação de ambiente, documente isso no PR.

---

## 🧹 Padrões de código

- Linguagem: **C99**
- Indentação: **2 espaços**
- Variáveis declaradas no início do escopo
- Convenções de nomenclatura históricas do projeto devem ser preservadas
- Headers em `include/`, implementação em `src/`
- Configuração runtime em `conf/*.xml` (não `config/`)

---

## ✅ Checklist para Pull Request

- [ ] Código compila no(s) backend(s) impactado(s)
- [ ] Não houve regressão clara em backend não alterado
- [ ] Escopo da mudança está claro (core, SDL2, raylib, terminal)
- [ ] Commit(s) seguem Conventional Commits
- [ ] Documentação atualizada quando necessário

---

## 📬 Dúvidas

Abra uma issue ou discussion descrevendo:
- cenário
- backend/plataforma
- passos para reproduzir
