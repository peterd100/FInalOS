#include "thread_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "memory_manager.h"

pthread_mutex_t manager_mutex;

// Function to run all threads simulating different processes
void runThreads(const char *outputFile, int seed, MemoryManager *manager, char **thread_input_files, int num_processes) {
    pthread_t *threads = malloc(num_processes * sizeof(pthread_t));
    ThreadArg *args = malloc(num_processes * sizeof(ThreadArg));
    pthread_mutex_init(&manager_mutex, NULL);

    // Create threads for each process
    for (int i = 0; i < num_processes; i++) {
        args[i].process_id = i;
        args[i].manager = manager;
        args[i].outputFile = outputFile;
        args[i].thread_input_file = thread_input_files[i];
        if (pthread_create(&threads[i], NULL, processThread, (void *)&args[i])) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < num_processes; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    free(threads);
    free(args);
    pthread_mutex_destroy(&manager_mutex);
}

// Thread function to handle process simulation
void *processThread(void *arg) {
    ThreadArg *targ = (ThreadArg *)arg;
    FILE *file = fopen(targ->outputFile, "a");
    if (file == NULL) {
        fprintf(stderr, "Unable to open output file for process %d.\n", targ->process_id);
        return NULL;
    }

    fprintf(file, "Process %d started\n", targ->process_id);
    FILE *inputFile = fopen(targ->thread_input_file, "r");
    if (inputFile == NULL) {
        fprintf(stderr, "Unable to open thread input file for process %d.\n", targ->process_id);
        fclose(file);  // Ensure the output file is closed to avoid resource leak
        return NULL;
    }

    char operation;
    char register_name[3];
    uint32_t virtual_address;
    size_t register_index = 0;

    // Process each memory access instruction from the input file
    while (fscanf(inputFile, " %c %2s %x", &operation, register_name, &virtual_address) == 3) {
        fprintf(file, "P%d OPERATION: %c %s %#010x\n", targ->process_id, operation, register_name, virtual_address);
        pthread_mutex_lock(&manager_mutex);

        // Check if the page is resident in memory
        int resident = isPageResident(targ->manager, virtual_address);
        size_t frame;
        if (!resident) {
            // Allocate frame if page is not resident
            frame = allocateFrame(targ->manager);
            if (frame == SIZE_MAX) {
                fprintf(file, "P%d: no free frames available, unable to allocate memory\n", targ->process_id);
                pthread_mutex_unlock(&manager_mutex);
                continue;  // Continue processing other instructions
            }
            setPageFrameMapping(targ->manager, virtual_address, frame);
            fprintf(file, "P%d: page %u not resident in memory, using free frame %zu\n", targ->process_id, virtual_address / 4096, frame);
        } else {
            frame = getFrameForPage(targ->manager, virtual_address);
            fprintf(file, "P%d: valid translation from page %u to frame %zu\n", targ->process_id, virtual_address / 4096, frame);
        }

        uint32_t pa = translateVirtualToPhysical(targ->manager, virtual_address);
        pthread_mutex_unlock(&manager_mutex);
        if (pa != (uint32_t)-1) {
            fprintf(file, "P%d: translated VA %#010x to PA %#010x\n", targ->process_id, virtual_address, pa);
            uint32_t data = 0x67c66973 + (uint32_t)register_index;
            fprintf(file, "P%d: %s = %#010x (mem at virtual addr %#010x)\n", targ->process_id, register_name, data, virtual_address);
            register_index++;
        } else {
            fprintf(file, "P%d: invalid translation for VA %#010x\n", targ->process_id, virtual_address);
        }
    }

    // Close files and complete process simulation
    fclose(inputFile);
    fprintf(file, "Process %d complete\n", targ->process_id);
    fclose(file);
    return NULL;
}


