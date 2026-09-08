#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// Dynamic Array
typedef struct { int *data; int size; int capacity; } DynamicArray;

void init_array(DynamicArray *arr, int cap) {
    if (cap <= 0) cap = 2;
    arr->data = (int *)malloc(cap * sizeof(int));
    arr->size = 0; arr->capacity = cap;
}
void resize_if_needed(DynamicArray *arr) {
    if (arr->size >= arr->capacity) {
        arr->capacity *= 2;
        arr->data = (int *)realloc(arr->data, arr->capacity * sizeof(int));
    }
}
void array_insert_back(DynamicArray *arr, int value) {
    resize_if_needed(arr);
    arr->data[arr->size++] = value;
}
void array_insert_front(DynamicArray *arr, int value) {
    resize_if_needed(arr);
    for (int i = arr->size; i > 0; i--) arr->data[i] = arr->data[i - 1];
    arr->data[0] = value;
    arr->size++;
}
int array_get(DynamicArray *arr, int index) {
    return (index >= 0 && index < arr->size) ? arr->data[index] : -1;
}
int array_find(DynamicArray *arr, int value) {
    for (int i = 0; i < arr->size; i++) if (arr->data[i] == value) return i;
    return -1;
}
void array_free(DynamicArray *arr) { free(arr->data); arr->size = arr->capacity = 0; }

// Linked List
typedef struct Node { int value; struct Node *next; } Node;
typedef struct { Node *head; int size; } LinkedList;

void init_list(LinkedList *list) { list->head = NULL; list->size = 0; }
void list_insert_front(LinkedList *list, int value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->value = value; new_node->next = list->head;
    list->head = new_node; list->size++;
}
void list_insert_back(LinkedList *list, int value) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->value = value; new_node->next = NULL;
    if (list->head == NULL) list->head = new_node;
    else {
        Node *current = list->head;
        while (current->next != NULL) current = current->next;
        current->next = new_node;
    }
    list->size++;
}
int list_get(LinkedList *list, int index) {
    Node *current = list->head;
    for (int i = 0; i < index && current != NULL; i++) current = current->next;
    return current ? current->value : -1;
}
int list_find(LinkedList *list, int value) {
    Node *current = list->head; int index = 0;
    while (current != NULL) {
        if (current->value == value) return index;
        current = current->next; index++;
    }
    return -1;
}
void list_free(LinkedList *list) {
    Node *current = list->head;
    while (current != NULL) { Node *next = current->next; free(current); current = next; }
    list->head = NULL; list->size = 0;
}

// --- TABLE DE HACHAGE ---
#define TABLE_SIZE 10007
typedef struct Entry { int value; struct Entry *next; } Entry;
typedef struct { Entry *buckets[TABLE_SIZE]; } HashTable;

void init_hash(HashTable *ht) { for (int i = 0; i < TABLE_SIZE; i++) ht->buckets[i] = NULL; }
int hash_good(int value) { int h = value % TABLE_SIZE; return h < 0 ? h + TABLE_SIZE : h; }
int hash_bad(int value) { return 0; }

void hash_insert(HashTable *ht, int value, int use_bad) {
    int index = use_bad ? hash_bad(value) : hash_good(value);
    Entry *new_entry = (Entry *)malloc(sizeof(Entry));
    new_entry->value = value; new_entry->next = ht->buckets[index];
    ht->buckets[index] = new_entry;
}
int hash_contains(HashTable *ht, int value, int use_bad) {
    int index = use_bad ? hash_bad(value) : hash_good(value);
    Entry *current = ht->buckets[index];
    while (current != NULL) {
        if (current->value == value) return 1;
        current = current->next;
    }
    return 0;
}
void hash_free(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        Entry *current = ht->buckets[i];
        while (current != NULL) { Entry *next = current->next; free(current); current = next; }
        ht->buckets[i] = NULL;
    }
}

// Benchmark

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
}

void benchmark_array(int N, int reps) {
    DynamicArray arr; init_array(&arr, 2);
    struct timespec start, end;
    
    for (int i = 0; i < N; i++) array_insert_back(&arr, i);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) array_get(&arr, N/2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [Array] Accès N/2        : %.6f s\n", get_time_diff(start, end) / reps);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) array_find(&arr, -1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [Array] Rech. absente    : %.6f s\n", get_time_diff(start, end) / reps);

    int safe_reps = reps > 10 ? 10 : reps; 
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<safe_reps; r++) array_insert_front(&arr, 999);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [Array] Insert début     : %.6f s\n", get_time_diff(start, end) / safe_reps);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) array_insert_back(&arr, 999);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [Array] Insert fin       : %.6f s\n", get_time_diff(start, end) / reps);

    long long sum_array = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) {
        sum_array = 0;
        for(int i = 0; i < arr.size; i++) sum_array += arr.data[i];
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [Array] Parcours somme   : %.6f s\n", get_time_diff(start, end) / reps);

    array_free(&arr);
}

void benchmark_list(int N, int reps) {
    LinkedList list; init_list(&list);
    struct timespec start, end;
    
    for (int i = 0; i < N; i++) list_insert_front(&list, i);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) list_get(&list, N/2);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [List]  Accès N/2        : %.6f s\n", get_time_diff(start, end) / reps);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) list_find(&list, -1);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [List]  Rech. absente    : %.6f s\n", get_time_diff(start, end) / reps);

    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) list_insert_front(&list, 999);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [List]  Insert début     : %.6f s\n", get_time_diff(start, end) / reps);

    int safe_reps = reps > 10 ? 10 : reps;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<safe_reps; r++) list_insert_back(&list, 999);
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [List]  Insert fin       : %.6f s\n", get_time_diff(start, end) / safe_reps);

    long long sum_list = 0;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for(int r=0; r<reps; r++) {
        sum_list = 0;
        Node *current = list.head;
        while(current != NULL) {
            sum_list += current->value;
            current = current->next;
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("  [List]  Parcours somme   : %.6f s\n", get_time_diff(start, end) / reps);

    list_free(&list);
}

void benchmark_hash(int N, int reps, int use_bad) {
    HashTable ht; init_hash(&ht);
    struct timespec start, end;
    
    for (int i = 0; i < N; i++) hash_insert(&ht, i, use_bad);

    clock_gettime(CLOCK_MONOTONIC, &start);
    int safe_reps = (use_bad && N > 10000) ? 10 : reps;
    for(int r=0; r<safe_reps; r++) hash_contains(&ht, -1, use_bad);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    printf("  [Hash %s] Rech. absente : %.6f s\n", use_bad ? "BAD " : "GOOD", get_time_diff(start, end) / safe_reps);

    hash_free(&ht);
}

// script

int main() {
    int tailles[] = {1000, 10000, 100000, 1000000};
    int nb_tailles = 4;
    int repetitions = 100;

    for (int i = 0; i < nb_tailles; i++) {
        int N = tailles[i];
        printf("\n==========================================\n");
        printf(" TESTS POUR N = %d\n", N);
        printf("==========================================\n");
        
        benchmark_array(N, repetitions);
        printf("------------------------------------------\n");
        benchmark_list(N, repetitions);
        printf("------------------------------------------\n");
        benchmark_hash(N, repetitions, 0);
        benchmark_hash(N, repetitions, 1);
    }

    printf("\n=== BENCHMARK TERMINÉ ===\n");
    return 0;
}