// lru_esqueleto.c
// Projeto Final - Estrutura de Dados
// Simulador de Cache LRU (arquivo unico)
//
// Regras do esqueleto:
// - Voce deve manter as assinaturas e os nomes das funcoes.
// - Voce deve usar lista duplamente ligada com sentinelas (head e tail).
// - Voce deve usar hash com encadeamento (separate chaining).
// - Saidas devem respeitar o formato do enunciado.
//
// Compile:
//   gcc -std=c11 -Wall -Wextra -O2 lru_esqueleto.c -o lru
//
// Execute:
//   ./lru < teste_base.txt

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ====== CONSTANTS / TYPES ======

typedef struct Node {
    int key;
    int value;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct HashEntry {
    int key;
    Node* node;               // aponta para o Node real na lista LRU
    struct HashEntry* next;   // encadeamento do balde
} HashEntry;

typedef struct Hash {
    int nbuckets;
    HashEntry** buckets;
} Hash;

typedef struct Cache {
    int capacity;
    int size;

    int hits;
    int misses;
    int evictions;

    Hash* h;

    // Sentinelas obrigatorias: head <-> ... <-> tail
    Node head;
    Node tail;
} Cache;

// ====== DOUBLY LINKED LIST (LRU) ======
// TODO: implementar lista com sentinelas.
//
// Invariante:
// head.next aponta para o MRU (ou tail se vazia)
// tail.prev aponta para o LRU (ou head se vazia)

static void list_init(Cache* c) {
    // TODO
    (void)c;
}

static void list_remove(Node* n) {
    // TODO
    (void)n;
}

static void list_push_front(Cache* c, Node* n) {
    // TODO
    (void)c; (void)n;
}

static Node* list_pop_back(Cache* c) {
    // TODO
    (void)c;
    return NULL;
}

// ====== HASH TABLE (SEPARATE CHAINING) ======
// TODO: implementar hash com encadeamento.
// Dica: use uma funcao hash simples para int.
// Dica: nbuckets pode ser (capacity * 2 + 1), com minimo 17.

static Hash* hash_create(int nbuckets) {
    (void)nbuckets;
    return NULL; // TODO
}

static void hash_destroy(Hash* h) {
    // TODO
    (void)h;
}

static Node* hash_get(Hash* h, int key) {
    // TODO
    (void)h; (void)key;
    return NULL;
}

static int hash_put(Hash* h, int key, Node* node) {
    // TODO: retorna 1 se inseriu nova chave, 0 se atualizou
    (void)h; (void)key; (void)node;
    return 0;
}

static int hash_remove(Hash* h, int key) {
    // TODO: retorna 1 se removeu, 0 se nao existia
    (void)h; (void)key;
    return 0;
}

// ====== LRU CACHE API ======
// TODO: implementar as operacoes do cache.
// Dica: GET e PUT devem ser O(1) amortizado.
// Dica: em HIT/UPDATE/INSERT, o item vira MRU (vai para a frente).

static Cache* cache_create(int capacity, int nbuckets) {
    (void)capacity; (void)nbuckets;
    return NULL; // TODO
}

static void cache_destroy(Cache* c) {
    // TODO: liberar nos, entradas da hash, buckets, e o proprio Cache
    (void)c;
}

static int cache_get(Cache* c, int key, int* out_value) {
    // TODO: retorna 1 hit, 0 miss
    (void)c; (void)key; (void)out_value;
    return 0;
}

static int cache_put(Cache* c, int key, int value, int* evicted_key) {
    // TODO: retorna 1 se evictou, 0 se nao
    (void)c; (void)key; (void)value; (void)evicted_key;
    return 0;
}

static void cache_dump(Cache* c) {
    // TODO
    (void)c;
}

static void cache_stats(Cache* c) {
    // TODO
    (void)c;
}

// ====== COMMAND PARSER / MAIN ======

static char* ltrim(char* s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

int main(void) {
    Cache* cache = NULL;

    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        char* s = ltrim(line);
        if (*s == '\0' || *s == '\n' || *s == '#') continue;

        char cmd = s[0];

        if (cmd == 'X') {
            break;
        }

        if (cmd == 'C') {
            // TODO: parsear capacidade, criar cache, tratar erro de segunda criacao
            // Saidas esperadas:
            // - ERROR CACHE ALREADY CREATED
            // - ERROR INVALID CAPACITY
            continue;
        }

        if (!cache) {
            printf("ERROR CACHE NOT CREATED\n");
            continue;
        }

        if (cmd == 'G') {
            // TODO: parsear chave, chamar cache_get, imprimir MISS ou HIT <valor>
            continue;
        }

        if (cmd == 'P') {
            // TODO: parsear chave e valor, chamar cache_put,
            // imprimir INSERTED / UPDATED / EVICTED <k> INSERTED
            continue;
        }

        if (cmd == 'D') {
            // TODO
            continue;
        }

        if (cmd == 'S') {
            // TODO
            continue;
        }

        // comando desconhecido: ignorar
    }

    if (cache) {
        cache_destroy(cache);
    }
    return 0;
}
