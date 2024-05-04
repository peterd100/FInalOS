#include "memory_manager.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define PAGE_SIZE 4096 

// Initialize the memory manager with frames and page table entries
void initializeMemoryManager(MemoryManager *manager, size_t memory_size, size_t page_size, size_t num_processes) {
    size_t num_frames = memory_size / page_size;
    manager->frames = malloc(num_frames * sizeof(Frame));
    manager->page_table = malloc(num_processes * num_frames * sizeof(PageTableEntry));
    manager->num_frames = num_frames;
    manager->num_entries = num_processes * num_frames;

    // Initialize frames and page table entries
    for (size_t i = 0; i < num_frames; i++) {
        manager->frames[i].frame_number = i;
        manager->frames[i].is_occupied = 0;
    }
    for (size_t i = 0; i < manager->num_entries; i++) {
        manager->page_table[i].virtual_address = 0;
        manager->page_table[i].frame_index = 0;
        manager->page_table[i].valid = 0;
    }
}

// Finalize the memory manager by freeing allocated resources
void finalizeMemoryManager(MemoryManager *manager) {
    free(manager->frames);
    free(manager->page_table);
}

// Translate a virtual address to a physical address
uint32_t translateVirtualToPhysical(const MemoryManager *manager, uint32_t virtual_address) {
    size_t page_number = virtual_address / PAGE_SIZE;
    size_t offset = virtual_address % PAGE_SIZE;
    for (size_t i = 0; i < manager->num_entries; i++) {
        if (manager->page_table[i].valid && manager->page_table[i].virtual_address / PAGE_SIZE == page_number) {
            size_t frame_number = manager->frames[manager->page_table[i].frame_index].frame_number;
            return (frame_number * PAGE_SIZE) + offset;
        }
    }
    return (uint32_t)-1;  // Return an invalid physical address if not found
}

// Check if a page is resident in memory
int isPageResident(const MemoryManager *manager, uint32_t virtual_address) {
    size_t page_number = virtual_address / PAGE_SIZE;
    for (size_t i = 0; i < manager->num_entries; i++) {
        if (manager->page_table[i].valid && manager->page_table[i].virtual_address == (page_number * PAGE_SIZE)) {
            return 1;  // Page is resident
        }
    }
    return 0;  // Page is not resident
}

// Allocate a free frame from the memory
size_t allocateFrame(MemoryManager *manager) {
    for (size_t i = 0; i < manager->num_frames; i++) {
        if (!manager->frames[i].is_occupied) {
            manager->frames[i].is_occupied = 1;
            return i;
        }
    }
    return evictPage(manager);  // Evict a page if no free frame is available
}

// Evict a page using a simple clock or other replacement algorithm
size_t evictPage(MemoryManager *manager) {
    // Simple FIFO eviction for demonstration
    for (size_t i = 0; i < manager->num_entries; i++) {
        if (manager->page_table[i].valid) {
            manager->frames[manager->page_table[i].frame_index].is_occupied = 0;
            manager->page_table[i].valid = 0;
            return manager->page_table[i].frame_index;
        }
    }
    return SIZE_MAX;  // No frame could be evicted
}

// Set a new mapping from a virtual address to a frame index
void setPageFrameMapping(MemoryManager *manager, uint32_t virtual_address, size_t frame_index) {
    size_t page_number = virtual_address / PAGE_SIZE;
    size_t index = page_number % manager->num_entries;  // Simplified hash mapping for demonstration
    manager->page_table[index].virtual_address = virtual_address;
    manager->page_table[index].frame_index = frame_index;
    manager->page_table[index].valid = 1;
}

// Get the frame index for a given page
size_t getFrameForPage(const MemoryManager *manager, uint32_t virtual_address) {
    size_t page_number = virtual_address / PAGE_SIZE;
    for (size_t i = 0; i < manager->num_entries; i++) {
        if (manager->page_table[i].valid && manager->page_table[i].virtual_address == (page_number * PAGE_SIZE)) {
            return manager->page_table[i].frame_index;
        }
    }
    return SIZE_MAX;  // Return an invalid index if not found
}
