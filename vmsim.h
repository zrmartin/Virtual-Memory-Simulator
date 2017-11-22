#ifndef VMSIM_H
#define VMSIM_H

#define TRUE 1
#define INT_MAX 4294967295

void getArgs(int *bus, int *vm, int *phys, int *rows, int *max_entries, int *mask, char *argv[], int argc);
uint32_t findMax(int bus);
uint32_t checkRequest(uint64_t test, uint32_t max, int bus);
int checkTable(int table[][3], uint32_t *request, int option, int *entry, int max_entries, int rows);
#endif
