// lru.c
// Projeto Final - Estrutura de Dados - 2026-1
// Simulador de Cache LRU (arquivo unico)
//
// Compile:
//   gcc -std=c11 -Wall -Wextra -O2 lru.c -o lru
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
// Invariante:
// head.next aponta para o MRU (ou tail se vazia)
// tail.prev aponta para o LRU (ou head se vazia)

static void list_init(Cache* c) {
    c->head.next = &c->tail; // Liga head à tail e tail à head
    c->head.prev = NULL;
    c->tail.next = NULL;
    c->tail.prev = &c->head;
}

static void list_remove(Node* n) {
    n->prev->next = n->next; // Reconecta a lista sem n
    n->next->prev = n->prev;

    n->next = NULL; // Separa n da lista
    n->prev = NULL;
}

static void list_push_front(Cache* c, Node* n) {
    n->next = c->head.next; // Liga o nó novo ao MRU anterior
    n->prev = &c->head;     // Liga o nó novo ao head
    c->head.next->prev = n; // Liga o MRU anterior ao nó novo
    c->head.next = n;       // Liga o head ao nó novo
}

static Node* list_pop_back(Cache* c) {
    if (c->head.next == &c->tail) // Protege head de ser removido
        return NULL;

    Node* removed = c->tail.prev; // Salva o nó removido

    c->tail.prev = c->tail.prev->prev; // Liga tail ao nó antes do nó removido
    c->tail.prev->next = &c->tail;     // Liga o nó antes do nó removido ao tail

    removed->next = NULL; // Separa o nó removido da lista
    removed->prev = NULL;

    return removed;
}

// ====== HASH TABLE (SEPARATE CHAINING) ======

// Função hash simples para garantir mapeamento correto no array de baldes
static int hash_func(int key, int nbuckets) {
    int h = key % nbuckets;
    if (h < 0) h += nbuckets;
    return h;
}

static Hash* hash_create(int nbuckets) {
    Hash* h = (Hash*)malloc(sizeof(Hash));
    if (!h) return NULL;
    h->nbuckets = nbuckets;
    h->buckets = (HashEntry**)calloc(nbuckets, sizeof(HashEntry*));
    if (!h->buckets) {
        free(h);
        return NULL;
    }
    return h;
}

static void hash_destroy(Hash* h) {
    if (!h) return;
    for (int i = 0; i < h->nbuckets; i++) {
        HashEntry* curr = h->buckets[i];
        while (curr) {
            HashEntry* tmp = curr->next;
            free(curr);
            curr = tmp;
        }
    }
    free(h->buckets);
    free(h);
}

static Node* hash_get(Hash* h, int key) {
    if (!h) return NULL;
    int idx = hash_func(key, h->nbuckets);
    HashEntry* curr = h->buckets[idx];
    while (curr) {
        if (curr->key == key) {
            return curr->node;
        }
        curr = curr->next;
    }
    return NULL;
}

static int hash_put(Hash* h, int key, Node* node) {
    int idx = hash_func(key, h->nbuckets);
    HashEntry* curr = h->buckets[idx];
    
    // Se a chave já existir na hash, apenas atualiza a referência do nó
    while (curr) {
        if (curr->key == key) {
            curr->node = node;
            return 0; // Chave atualizada
        }
        curr = curr->next;
    }
    
    // Se for uma chave nova, cria uma entrada e insere no início do balde
    HashEntry* entry = (HashEntry*)malloc(sizeof(HashEntry));
    entry->key = key;
    entry->node = node;
    entry->next = h->buckets[idx];
    h->buckets[idx] = entry;
    return 1; // Nova chave inserida
}

static int hash_remove(Hash* h, int key) {
    int idx = hash_func(key, h->nbuckets);
    HashEntry* curr = h->buckets[idx];
    HashEntry* prev = NULL;
    
    while (curr) {
        if (curr->key == key) {
            if (prev) {
                prev->next = curr->next;
            } else {
                h->buckets[idx] = curr->next;
            }
            free(curr);
            return 1; // Removido com sucesso
        }
        prev = curr;
        curr = curr->next;
    }
    return 0; // Não existia
}

// ====== LRU CACHE API ======

static Cache* cache_create(int capacity, int nbuckets) {
    Cache* c = (Cache*)malloc(sizeof(Cache));
    if (!c) return NULL;
    
    c->capacity = capacity;
    c->size = 0;
    c->hits = 0;
    c->misses = 0;
    c->evictions = 0;
    
    c->h = hash_create(nbuckets);
    list_init(c);
    
    return c;
}

static void cache_destroy(Cache* c) {
    if (!c) return;
    
    // Libera os nós dinâmicos que estão dentro da lista
    Node* curr = c->head.next;
    while (curr != &c->tail) {
        Node* next = curr->next;
        free(curr);
        curr = next;
    }
    
    hash_destroy(c->h);
    free(c);
}

static int cache_get(Cache* c, int key, int* out_value) {
    Node* n = hash_get(c->h, key);
    if (n) {
        *out_value = n->value;
        // HIT: Retira de onde está e joga para o início (MRU)
        list_remove(n);
        list_push_front(c, n);
        c->hits++;
        return 1; // HIT
    }
    c->misses++;
    return 0; // MISS
}

static int cache_put(Cache* c, int key, int value, int* evicted_key) {
    Node* n = hash_get(c->h, key);
    
    if (n) {
        // UPDATE: Chave existe. Atualiza valor e vira MRU
        n->value = value;
        list_remove(n);
        list_push_front(c, n);
        return 0;
    }
    
    // Inserção de nova chave
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->key = key;
    new_node->value = value;
    
    int status = 2; // Padrão para INSERTED sem despejo
    
    if (c->size >= c->capacity) {
        // Cache cheio: remove o menos recentemente usado (LRU) da lista e da hash
        Node* lru = list_pop_back(c);
        if (lru) {
            *evicted_key = lru->key;
            hash_remove(c->h, lru->key);
            free(lru);
            c->evictions++;
            c->size--;
            status = 1; // Flag para EVICTED INSERTED
        }
    }
    
    list_push_front(c, new_node);
    hash_put(c->h, key, new_node);
    c->size++;
    
    return status;
}

static void cache_dump(Cache* c) {
    printf("CACHE: [MRU]");
    Node* curr = c->head.next;
    while (curr != &c->tail) {
        printf(" %d:%d", curr->key, curr->value);
        curr = curr->next;
    }
    printf(" [LRU]\n");
}

static void cache_stats(Cache* c) {
    printf("STATS: hits=%d misses=%d evictions=%d size=%d capacity=%d\n",
           c->hits, c->misses, c->evictions, c->size, c->capacity);
}

// ====== COMMAND PARSER / MAIN ======

static char* ltrim(char* s) {
    while (*s && isspace((unsigned char)*s)) s++;
    return s;
}

int main(void) {
    Cache* cache = NULL;
    int cache_created_once = 0; // Flag essencial para rastrear tentativas duplicadas de criação

    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        char* s = ltrim(line);
        if (*s == '\0' || *s == '\n' || *s == '#') continue;

        char cmd = s[0];

        if (cmd == 'X') {
            break;
        }

        if (cmd == 'C') {
            int capacity;
            if (sscanf(s + 1, "%d", &capacity) == 1) {
                if (capacity <= 0) {
                    printf("ERROR INVALID CAPACITY\n");
                    if (cache) cache_destroy(cache);
                    return 0; // Encerra imediatamente conforme especificação de erro crítico
                }
                
                if (cache_created_once) {
                    printf("ERROR CACHE ALREADY CREATED\n");
                } else {
                    // Configuração de buckets recomendada: (capacity * 2 + 1), mínimo 17
                    int nbuckets = capacity * 2 + 1;
                    if (nbuckets < 17) nbuckets = 17;
                    
                    cache = cache_create(capacity, nbuckets);
                    cache_created_once = 1;
                }
            }
            continue;
        }

        // Se qualquer comando de manipulação aparecer antes da criação estrutural
        if (!cache_created_once) {
            printf("ERROR CACHE NOT CREATED\n");
            continue;
        }

        if (cmd == 'G') {
            int key, out_val;
            if (sscanf(s + 1, "%d", &key) == 1) {
                if (cache_get(cache, key, &out_val)) {
                    printf("HIT %d\n", out_val);
                } else {
                    printf("MISS\n");
                }
            }
            continue;
        }

        if (cmd == 'P') {
            int key, value, evicted_key;
            if (sscanf(s + 1, "%d %d", &key, &value) == 2) {
                int res = cache_put(cache, key, value, &evicted_key);
                if (res == 0) {
                    printf("UPDATED\n");
                } else if (res == 1) {
                    printf("EVICTED %d INSERTED\n", evicted_key);
                } else {
                    printf("INSERTED\n");
                }
            }
            continue;
        }

        if (cmd == 'D') {
            cache_dump(cache);
            continue;
        }

        if (cmd == 'S') {
            cache_stats(cache);
            continue;
        }
    }

    if (cache) {
        cache_destroy(cache);
    }
    return 0;
}