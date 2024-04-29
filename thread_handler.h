#ifndef THREAD_HANDLER_H
#define THREAD_HANDLER_H

#include <pthread.h>
#include "memory_manager.h"

typedef struct ThreadArg {
    int process_id;
    const char *outputFile;
    const char *thread_input_file;
    MemoryManager *manager;
} ThreadArg;

void runThreads(const char *outputFile, int seed, MemoryManager *manager, char **thread_input_files, int num_processes);
void *processThread(void *arg);

#endif

