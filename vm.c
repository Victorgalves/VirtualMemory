#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 32
#define PAGE_SIZE 256
#define FRAME_SIZE 256
#define PAGE_TABLE_SIZE 128
#define TLB_SIZE 16

typedef struct {
    int instrucao[8];
    int num_pagina[MAX - 8];
    int physicalAddress;
} InstrucaoPagina;

typedef struct {
    int num_pagina[MAX - 8];
    int index;
} TLBEntry;

typedef struct {
    int num_pagina[MAX - 8];
    int index;
    int ultimoAcesso;
} LRUEntry;

int vet_bin[MAX];
InstrucaoPagina pageTable[PAGE_TABLE_SIZE];
TLBEntry tlb[TLB_SIZE];
LRUEntry lru[PAGE_TABLE_SIZE];

int front = 0;
int rear = -1;
int itemCount = 0;
int time = 0;

int tlb_front = 0;
int tlb_rear = -1;
int tlb_itemCount = 0;

int lru_front = 0;
int lru_rear = -1;
int lru_itemCount = 0;

int countTlbHit=0;
int countLruHit=0;
int countEnd=0;
int pageFaults = 0; 

void inicializarInstrucaoPagina(InstrucaoPagina *ip) {
    for (int i = 0; i < 8; i++) {
        ip->instrucao[i] = 0;
    }
    for (int i = 0; i < MAX - 8; i++) {
        ip->num_pagina[i] = 0;
    }
    ip->physicalAddress = 0;
}

void inicializarTLBEntry(TLBEntry *entry) {
    for (int i = 0; i < MAX - 8; i++) {
        entry->num_pagina[i] = 0;
    }
    entry->index = -1;
}

void inicializarLRUEntry(LRUEntry *entry) {
    for (int i = 0; i < MAX - 8; i++) {
        entry->num_pagina[i] = 0;
    }
    entry->index = -1;
    entry->ultimoAcesso = 0;
}

void conversaoESeparacao(int n, InstrucaoPagina *ip) {
    int i = 0;

    inicializarInstrucaoPagina(ip);

    while (n > 0) {
        vet_bin[i] = n % 2;
        i++;
        n = n / 2;
    }

    for (int j = 0; j < i; j++) {
        if (j < 8) {
            ip->instrucao[j] = vet_bin[j];
        } else {
            ip->num_pagina[j - 8] = vet_bin[j];
        }
    }
}

int converterDecimal(int *binario, int tamanho) {
    int decimal = 0;
    for (int i = 0; i < tamanho; i++) {
        decimal += binario[i] * (1 << i);
    }
    return decimal;
}

int isFull() {
    return itemCount == PAGE_TABLE_SIZE;
}

int isEmpty() {
    return itemCount == 0;
}

int isTLBFull() {
    return tlb_itemCount == TLB_SIZE;
}

int isLRUFull() {
    return lru_itemCount == PAGE_TABLE_SIZE;
}

InstrucaoPagina deleteFifo() {
    InstrucaoPagina data = pageTable[front++];

    if(front == PAGE_TABLE_SIZE) {
        front = 0;
    }

    itemCount--;
    return data;  
}

TLBEntry deleteTLBFifo() {
    TLBEntry data = tlb[tlb_front++];

    if(tlb_front == TLB_SIZE) {
        tlb_front = 0;
    }

    tlb_itemCount--;
    return data;  
}

LRUEntry deleteLRU() {
    int oldestIndex = 0;
    for (int i = 1; i < lru_itemCount; i++) {
        if (lru[i].ultimoAcesso < lru[oldestIndex].ultimoAcesso) {
            oldestIndex = i;
        }
    }
    LRUEntry data = lru[oldestIndex];
    for (int i = oldestIndex; i < lru_itemCount - 1; i++) {
        lru[i] = lru[i + 1];
    }
    lru_itemCount--;
    return data;  
}

int inserir(InstrucaoPagina data) {
    if(!isFull()) {
        if(rear == PAGE_TABLE_SIZE-1) {
            rear = -1;            
        }       

        pageTable[++rear] = data;
        itemCount++;
        return 1;
    } else {
        return 0;
    }
}

int inserirTLB(TLBEntry data) {
    if(!isTLBFull()) {
        if(tlb_rear == TLB_SIZE-1) {
            tlb_rear = -1;            
        }       

        tlb[++tlb_rear] = data;
        tlb_itemCount++;
        return 1;
    } else {
        return 0;
    }
}

int inserirLRU(LRUEntry data) {
    if(!isLRUFull()) {
        lru[lru_itemCount++] = data;
        return 1;
    } else {
        return 0;
    }
}

int physicalAddress(int index, int instruction) {
    return (index * 256 )+ instruction;
}

int getTLBIndex(int *num_pagina) {
    for (int i = 0; i < tlb_itemCount; i++) {
        int isMatch = 1;
        for (int j = 0; j < MAX - 8; j++) {
            if (tlb[i].num_pagina[j] != num_pagina[j]) {
                isMatch = 0;
                break;
            }
        }
        if (isMatch) {
            return i; 
        }
    }
    return -1; 
}

void updateTLB(int *num_pagina, int index) {
    int tlbIndex = getTLBIndex(num_pagina);
    if (tlbIndex != -1) {
        tlb[tlbIndex].index = index;
    } else {
        TLBEntry entry;
        inicializarTLBEntry(&entry);
        for (int j = 0; j < MAX - 8; j++) {
            entry.num_pagina[j] = num_pagina[j];
        }
        entry.index = index;
        while (!inserirTLB(entry)) {
            deleteTLBFifo();
        }
    }
}


int getLRUIndex(int *num_pagina) {
    for (int i = 0; i < lru_itemCount; i++) {
        if (converterDecimal(lru[i].num_pagina, MAX - 8) == converterDecimal(num_pagina, MAX - 8)) {
            return i; 
        }
    }
    return -1;
}

void updateLRUEntry(int index) {
    lru[index].ultimoAcesso = time++;
}

int getOrInsertPageFifo(InstrucaoPagina data) {
    int tlbIndex = getTLBIndex(data.num_pagina);
    if (tlbIndex != -1) {
        countTlbHit++;
        return tlb[tlbIndex].index; 
    }

    for (int i = 0; i < itemCount; i++) {
        if (converterDecimal(pageTable[i].num_pagina, MAX - 8) == converterDecimal(data.num_pagina, MAX - 8)) {
            TLBEntry entry;
            inicializarTLBEntry(&entry);
            for (int j = 0; j < MAX - 8; j++) {
                entry.num_pagina[j] = data.num_pagina[j];
            }
            entry.index = i;
            while (!inserirTLB(entry)) {
                deleteTLBFifo();
            }
            return i;
        }
    }

    pageFaults++;

    while (!inserir(data)) {
        deleteFifo();
    }
    TLBEntry entry;
    inicializarTLBEntry(&entry);
    for (int j = 0; j < MAX - 8; j++) {
        entry.num_pagina[j] = data.num_pagina[j];
    }
    entry.index = rear;
    while (!inserirTLB(entry)) {
        deleteTLBFifo();
    }
    return rear;
}

int getOrInsertPageLRU(InstrucaoPagina data) {
    int lruIndex = getLRUIndex(data.num_pagina);
    if (lruIndex != -1) {
        int tlbIndex = getTLBIndex(data.num_pagina);
        if (tlbIndex != -1) {
            countTlbHit++; 
        }

        lru[lruIndex].ultimoAcesso = time++;
        updateTLB(data.num_pagina, lru[lruIndex].index); 
        return lru[lruIndex].index;
    }

    for (int i = 0; i < itemCount; i++) {
        if (converterDecimal(pageTable[i].num_pagina, MAX - 8) == converterDecimal(data.num_pagina, MAX - 8)) {
            updateLRUEntry(i);
            updateTLB(data.num_pagina, i);
            return i;
        }
    }

    pageFaults++;

    if (isLRUFull()) {
        LRUEntry removedEntry = deleteLRU();
        int removedIndex = removedEntry.index;
        pageTable[removedIndex] = data;

        LRUEntry entry;
        inicializarLRUEntry(&entry);
        for (int j = 0; j < MAX - 8; j++) {
            entry.num_pagina[j] = data.num_pagina[j];
        }
        entry.index = removedIndex;
        entry.ultimoAcesso = time++;
        lru[lru_itemCount++] = entry;
        updateTLB(data.num_pagina, removedIndex);
        return removedIndex;
    }

    pageTable[itemCount] = data;

    LRUEntry newEntry;
    inicializarLRUEntry(&newEntry);
    for (int j = 0; j < MAX - 8; j++) {
        newEntry.num_pagina[j] = data.num_pagina[j];
    }
    newEntry.index = itemCount;
    newEntry.ultimoAcesso = time++;
    lru[lru_itemCount++] = newEntry;

    updateTLB(data.num_pagina, itemCount);

    return itemCount++;
}

int readFromBackingStore(int address) {
    FILE *file = fopen("BACKING_STORE.bin", "rb");

    fseek(file, address, SEEK_SET); 
    signed char value;
    fread(&value, sizeof(signed char), 1, file); 
    fclose(file);
    return value;
}

void readFromFileFifo(char *filename) {
    FILE *file = fopen(filename, "r");
    FILE *output = fopen("correct.txt", "w");
    int num;
    while (fscanf(file, "%d", &num) != EOF) {
        InstrucaoPagina ip;
        conversaoESeparacao(num, &ip);
        int index = getOrInsertPageFifo(ip);
        ip.physicalAddress = physicalAddress(index, converterDecimal(ip.instrucao, 8));
        int value = readFromBackingStore(num);

        int tlbIndex = getTLBIndex(ip.num_pagina);
        fprintf(output,"Virtual address: %d TLB: %d Physical address: %d Value: %d\n", num, tlbIndex,ip.physicalAddress, value);
        countEnd++;
    }

    fprintf(output,"Number of Translated Addresses = %d\n", countEnd);
    fprintf(output,"Page Faults = %d\n", pageFaults);
    float pageFaultRate = (float)pageFaults / (float)countEnd;
    fprintf(output,"Page Fault Rate = %.3f\n", pageFaultRate);
    fprintf(output,"TLB Hits = %d\n", countTlbHit);
    float tlbHitRate = (float)countTlbHit / (float)countEnd;
    fprintf(output,"TLB Hit Rate = %.3f", tlbHitRate);
    fprintf(output,"\n");
    fclose(file);
    fclose(output);
}

void readFromFileLRU(char *filename) {
    FILE *file = fopen(filename, "r");
    FILE *output = fopen("correct.txt", "w");

    int num;
    while (fscanf(file, "%d", &num) != EOF) {
    
        InstrucaoPagina ip;
        conversaoESeparacao(num, &ip);
        int index = getOrInsertPageLRU(ip);
        ip.physicalAddress = physicalAddress(index, converterDecimal(ip.instrucao, 8));
        int value = readFromBackingStore(num);
        int tlbIndex = getTLBIndex(ip.num_pagina); 
        fprintf(output, "Virtual address: %d TLB: %d Physical address: %d Value: %d\n", num, tlbIndex,ip.physicalAddress, value); 
        countEnd++;
    }
     
    fprintf(output, "Number of Translated Addresses = %d\n", countEnd);
    fprintf(output, "Page Faults = %d\n", pageFaults);
    float pageFaultRate = (float)pageFaults / (float)countEnd;
    fprintf(output, "Page Fault Rate = %.3f\n", pageFaultRate);
    fprintf(output, "TLB Hits = %d\n", countTlbHit);
    float tlbHitRate = (float)countTlbHit / (float)countEnd;
    fprintf(output, "TLB Hit Rate = %.3f", tlbHitRate);
    fprintf(output,"\n");   
    fclose(file);
    fclose(output);
}

int main(int argc, char *argv[]) {

    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        inicializarInstrucaoPagina(&pageTable[i]);
        inicializarLRUEntry(&lru[i]);
    }
    for (int i = 0; i < TLB_SIZE; i++) {
        inicializarTLBEntry(&tlb[i]);
    }
    
   

    char *address_file = argv[1];
    char *replacement_algorithm = argv[2];

    if (strcmp(replacement_algorithm, "fifo") == 0) {
        readFromFileFifo(address_file);
    }  
    if (strcmp(replacement_algorithm, "lru") == 0) {
        readFromFileLRU(address_file);
    } 

    return 0;
}
