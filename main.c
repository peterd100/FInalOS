#include "memory_manager.h"
#include "thread_handler.h"
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <input file> <output file> <RNG seed>\n", argv[0]);
        return 1;
    }
    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Error: Unable to open input file.\n");
        return 1;
    }
    size_t memory_size, page_size;
    int num_processes;
    fscanf(inputFile, "%zu %zu %d", &memory_size, &page_size, &num_processes);
    
    char **thread_input_files = malloc(num_processes * sizeof(char *));
    for (int i = 0; i < num_processes; i++) {
        thread_input_files[i] = malloc(256 * sizeof(char));
        fscanf(inputFile, "%s", thread_input_files[i]);
    }
    
    fclose(inputFile);
    MemoryManager manager;
    initializeMemoryManager(&manager, memory_size, page_size, num_processes);
    runThreads(argv[2], atoi(argv[3]), &manager, thread_input_files, num_processes);
    finalizeMemoryManager(&manager);
    for (int i = 0; i < num_processes; i++) {
        free(thread_input_files[i]);
    }
    free(thread_input_files);
    printf("Simulation complete. Output written to %s\n", argv[2]);
    printf("Main: program completed\n");
    return 0;
}
