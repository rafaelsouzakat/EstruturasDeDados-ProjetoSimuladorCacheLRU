# Simulador de Cache LRU

Projeto Final de Estruturas de Dados em C que implementa um simulador de cache com política LRU (Least Recently Used).

Estrutura principal:
- `codigo_final/` : implementação final (`lru.c`).
- `esqueleto/` : código esqueleto e arquivos de teste em `esqueleto/saidas_esperadas/`.

Como compilar (exemplo):

```bash
gcc -Wall -Wextra -o lru_exec codigo_final/lru.c
```

Como testar (exemplo usando os arquivos em `esqueleto/saidas_esperadas/`):

```bash
./lru_exec < esqueleto/saidas_esperadas/teste_base.txt > saida.txt
diff -u esqueleto/saidas_esperadas/teste_base_out.txt saida.txt
```

Observações:
- Ajuste os comandos de compilação/execução conforme a implementação do `main` no código.

Grupo:
