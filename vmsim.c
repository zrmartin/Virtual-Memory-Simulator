#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <inttypes.h>
#include <math.h>
#include "vmsim.h"

int requests = 0, faults = 0, writes = 0;

void endProgram() {
	printf("\n");
	printf("Requests: %d\n", requests);
	printf("Page Faults: %d\n", faults);
	printf("Page Faults W/ Write: %d", writes);
	exit(EXIT_SUCCESS);
}

void getArgs(int *bus, int *vm, int *phys, int *rows, int *max_entries, int *mask, char *argv[], int argc) {
	if (argc < 4) {
		printf("Format: ./vsim [bus] [vm] [phys]\n");
		exit(EXIT_FAILURE);
	}
	*bus = atoi(argv[1]);
	*vm = atoi(argv[2]);
	*phys = atoi(argv[3]);
	if (*bus > 32) {
		printf("Bus size must be <= 32\n");
		exit(EXIT_FAILURE);
	}
	if (*vm > *bus) {
		printf("Bits used for vm must be less than bus size\n");
		exit(EXIT_FAILURE);
	}
	if (*phys > *vm) {
		printf("Bits used for phys pages must be <= bits used for virtual page\n");
		exit(EXIT_FAILURE);
	}
	// Can't get pow() to work, so I manually make pow() function for a few necessary variables
	for (int i = 0; i < *vm; i++) {
		*rows *= 2;
	}
	for (int i = 0; i < *phys; i++) {
		*max_entries *= 2;
	}
	for (int i = 0; i < *bus - *vm; i++) {
		*mask *= 2;
	}
	*mask -= 1;
}

uint32_t findMax(int bus) {
	// Max request int based on bus size
	uint32_t max = 1;
	for (int i = 0; i < bus; i++) {
		max *= 2;
	}
	max -= 1;
	return max;
}

// Make sure request isn't larger than 2^bus or 2^32
// Also increment 'requests' global variable
uint32_t checkRequest(uint64_t test, uint32_t max, int bus) {
	
	if (test > INT_MAX) {
		printf("Request is larger than max value of a 32 bit unsigned int\n");
		exit(EXIT_FAILURE);
	}
	if (test > max) {
		printf("Request is larger than max value of a %d bit unsigned int\n", bus);
		exit(EXIT_FAILURE);
	}
	requests++;
	return (uint32_t) test;
}

/* Takes a request and checks to see if vm entry is present or not.
   If it is present, return the physical mem mapping.
   If it is not present, add a new entry if entry < max_entries and increase page fault global.
   If entry == max_entries must find an entry to evict.
   First entry with a 0 in read/write column is evicted.
   If all entries are a 1 in read/write column, first written entry is evicted 
   and page fualt W/ write is recorded
*/
int checkTable(int table[][3], uint32_t *request, int option, int *entry, int max_entries, int rows) {
	// Needed to get the physical mapping of an entry that is going to be evicted.
	int temp_entry;
	// Page is not present in table, page fault occured, must add to table
	if (table[*request][1] == 0) {
		faults++;
		// Have not hit max entries determined by 'phys' arg, add a new page with current value of entry.
		if (*entry < max_entries) {
			table[*request][0] = *entry;
			table[*request][1] = 1;
			table[*request][2] = option;
			return (*entry)++;
		}
		// Need to evict a page first
		else {
			// Getting first present page with no dirty bit
			for (int i = 0; i < rows; i++) {
				if (table[i][1] == 1 && table[i][2] == 0) {
					// Clearing table entry for eviction
					table[i][1] = 0;
					// Copying phys page number for new entry
					temp_entry = table[i][0];
					// Clearing phys page number
					table[i][0] = -1;
					// Setting new table entry
					table[*request][0] = temp_entry;
					table[*request][1] = 1;
					table[*request][2] = option;
					return temp_entry;
				}
			}
			// Getting first present page with dirty bit
			for (int i = 0; i < rows; i++) {
				if (table[i][1] == 1 && table[i][2] == 1) {
					// Clearing table entry for eviction
					table[i][1] = 0;
					// Copying phys page number for new entry
					temp_entry = table[i][0];
					// Clearing phys page number
					table[i][0] = -1;
					table[i][2] = 0;
					// Setting new table entry
					table[*request][0] = temp_entry;
					table[*request][1] = 1;
					table[*request][2] = option;
					writes++;
					return temp_entry;
				}
			}
		}
	}
	// Page is aleady present in table
	else {
		// Even if page is present, must set the option
		table[*request][2] = option;
		return (table[*request][0]);
	}
}

int main(int argc, char *argv[]) {
	int bus, vm, phys, option, rows = 1, entry = 0, max_entries = 1, phys_page, mask = 1;
	uint32_t max, request, offset, result;
	uint64_t test;

	getArgs(&bus, &vm, &phys, &rows, &max_entries, &mask, argv, argc);
	int table[rows][3];
	// Initializing table
	for (int i = 0; i < rows; i++) {
		table[i][0] = -1;
		table[i][1] = 0;
		table[i][2] = 0;
	}
	max = findMax(bus);
	
	while (TRUE) {
		signal(SIGINT, endProgram);
		// If conditional used for when input is given by a file for test scripts.
		if (scanf("%d", &option) < 0) {
			endProgram();
		}
		scanf("%lu", &test);
		if (option > 1 || option < 0) {
			printf("option must be 0 or 1\n");
			continue;
		}
		request = offset = checkRequest(test, max, bus);
		request = request >> (bus - vm);
		phys_page = checkTable(table, &request, option, &entry, max_entries, rows);
		phys_page = phys_page << (bus - vm);
		offset = offset & mask;
		result = phys_page | offset;
		printf("%lu\n", (unsigned long) result);

	}

}