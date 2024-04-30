#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>

typedef struct Frame {
    size_t frame_number;
    int is_occupied;
} Frame;

typedef struct PageTableEntry {
    size_t virtual_address;
    size_t frame_index;
    int valid;
} PageTableEntry;

typedef struct MemoryManager {
    Frame *frames;
    PageTableEntry *page_table;
    size_t num_frames;
    size_t num_entries;  // Total number of entries in the page table
} MemoryManager;

void initializeMemoryManager(MemoryManager *manager, size_t memory_size, size_t page_size, size_t num_processes);
void finalizeMemoryManager(MemoryManager *manager);
uint32_t translateVirtualToPhysical(const MemoryManager *manager, uint32_t virtual_address);
int isPageResident(const MemoryManager *manager, uint32_t virtual_address);
size_t allocateFrame(MemoryManager *manager);
size_t evictPage(MemoryManager *manager);
void setPageFrameMapping(MemoryManager *manager, uint32_t virtual_address, size_t frame_index);
size_t getFrameForPage(const MemoryManager *manager, uint32_t virtual_address);

#endif

