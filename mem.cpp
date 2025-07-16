// Memory management for the kernel

#include <stddef.h>

#include "include/error.h"
#include "include/kernel.h"
#include "include/mem.h"
#include "include/multiboot.h"

// PMM bitmap
static uint32_t* pmm_bm = 0;
// Total frames
static uint32_t pmm_tf = 0;
// Bitmap size
static uint32_t pmm_bm_s = 0;

// Maximum number of caches
#define MAX_CACHES 10
// Minimum power for cache size
#define MIN_CACHE_POWER 4


// This structure defines a single slab of memory
typedef struct slab_s
{
    // Points to the next slab
    struct slab_s* next;
    void* free_list_head;
    uint16_t obj_in_use;
    uint16_t tot_obj;
} slab_t;


// Cache of slabs
typedef struct {
    slab_t* part_slabs;
    slab_t* full_slabs;
    uint32_t obj_size;
} kmem_cache_t;

static kmem_cache_t caches[MAX_CACHES] = {0};


// Handy-dandy forward declarino!
static void kmem_cache_expand(kmem_cache_t* cache);


// Initialize the kernel memory manager
void kmem_init()
{
    for (int i = 0; i < MAX_CACHES; ++i)
    {
        caches[i].part_slabs = NULL;
        caches[i].full_slabs = NULL;
        caches[i].obj_size = 1 << (i + MIN_CACHE_POWER);
    }

    log(LOG_INFO, "The kernel heap has been initialized.\n");
}


// Locate a cache, given an allocation size
static kmem_cache_t* find_for_size(uint32_t size)
{
    for (int i = 0; i < MAX_CACHES; ++i)
    {
        if (caches[i].obj_size >= size)
        {
            return &caches[i];
        }
    }

    // Too BEEEG
    return NULL;
}


void* kmalloc(uint32_t size)
{
    if (size == 0)
    {
        return NULL;
    }

    uint32_t req_size = size + sizeof(uint8_t);
    kmem_cache_t* cache = find_for_size(req_size);

    if (!cache)
    {
        log(LOG_ERROR, "The requested size is too large.\n");
        // TODO: a decent explanation, perhaps
        return NULL;
    }

    if (!cache -> part_slabs)
    {
        kmem_cache_expand(cache);

        if (!cache -> part_slabs)
        {
            punchout("Unable to expand the cache.\n");
            return NULL;
        }
    }

    slab_t* slab = cache -> part_slabs;
    void* ptr = slab -> free_list_head;

    // Update free list
    slab -> free_list_head = *((void**)ptr);
    slab -> obj_in_use++;

    // If slab is full, move to the full slab list
    if (slab -> obj_in_use == slab -> tot_obj)
    {
        cache -> part_slabs = slab -> next;
        slab -> next = cache -> full_slabs;
        cache -> full_slabs = slab;
    }

    uint8_t cidx = cache - caches;
    uint8_t* head_ptr = (uint8_t*)ptr;
    *head_ptr = cidx;

    return (void*)(head_ptr + sizeof(uint8_t));
}


void kfree(void* ptr)
{
    if (!ptr)
    {
        return;
    }

    uint8_t* head_ptr = (uint8_t*)ptr - sizeof(uint8_t);
    uint8_t cidx = *head_ptr;
    void* true_ptr = (void*)head_ptr;

    if (cidx >= MAX_CACHES)
    {
        log(LOG_ERROR, "The cache index is invalid: ");
        print_dec(cidx);
        print("\n");
        return;
    }

    kmem_cache_t* cache = &caches[cidx];

    // TODO use actual pointer artihmetic
    slab_t* slab = NULL;
    slab_t* prev_slab = NULL;


    for (slab_t* s = cache -> part_slabs; s; s = s -> next)
    {
        if (true_ptr >= (void*)s && true_ptr < (void*)s + PMM_F_SIZE)
        {
            slab = s;
            break;
        }
    }

    if (!slab)
    {
        for (slab_t* s = cache -> full_slabs; s; prev_slab = s, s = s -> next)
        {
            if (true_ptr >= (void*)s && true_ptr < (void*)s + PMM_F_SIZE)
            {
                slab = s;

                if (prev_slab)
                {
                    prev_slab -> next = s -> next;
                }

                else
                {
                    cache -> full_slabs = s -> next;
                }

                s -> next = cache -> part_slabs;
                cache -> part_slabs = s;
                break;
            }
        }
    }

    if (!slab)
    {
        print_red("\nFailed to locate slab for pointer: ");
        print_hex((uint32_t)true_ptr);
        print("\n");
        return;
    }

    *((void**)true_ptr) = slab -> free_list_head;
    slab -> free_list_head = true_ptr;
    slab ->  obj_in_use--;
}


static void kmem_cache_expand(kmem_cache_t* cache)
{
    uint32_t slab_page = pmm_alloc_frame();

    if (!slab_page)
    {
        log(LOG_ERROR, "Could not allocate a slab page.\n");
        return;
    }

    slab_t* new_slab = (slab_t*)slab_page;
    new_slab -> obj_in_use = 0;
    new_slab -> tot_obj = (PMM_F_SIZE - sizeof(slab_t)) / cache -> obj_size;

    new_slab -> free_list_head = (void*)((uint32_t)new_slab + sizeof(slab_t));
    char* p = (char*)new_slab -> free_list_head;

    for (uint16_t i = 0; i < new_slab -> tot_obj - 1; ++i)
    {
        *((void**)p) = (void*)(p + cache -> obj_size);
        p += cache -> obj_size;
    }

    *((void**)p) = NULL;

    new_slab -> next = cache -> part_slabs;
    cache -> part_slabs = new_slab;
}


// Function that sets a bit within the bitmap
static void pmm_bitset(uint32_t fidx)
{
    uint32_t dwidx = fidx / 32;
    uint32_t bidx = fidx % 32;
    pmm_bm[dwidx] |= (1 << bidx);
}


// Clear a bit from within the bitmap
static void pmm_bitclear(uint32_t fidx)
{
    uint32_t dwidx = fidx / 32;
    uint32_t bidx = fidx % 32;
    pmm_bm[dwidx] &= ~(1 << bidx);
}

// Verify a bit has been set
static bool pmm_bitcheck(uint32_t fidx)
{
    uint32_t dwidx = fidx / 32;
    uint32_t bidx = fidx % 32;
    return (pmm_bm[dwidx] & (1 << bidx)) != 0;
}

// Locate the first free frame
static uint32_t pmm_first_free()
{
    for (uint32_t i = 0; i < pmm_bm_s / 4; ++i)
    {
        if (pmm_bm[i] != 0xFFFFFFFF)
        {
            for (uint32_t j = 0; j < 32; ++j)
            {
                uint32_t bit = 1 << j;

                if (!(pmm_bm[i] & bit))
                {
                    uint32_t fidx = i * 32 + j;

                    if (fidx < pmm_tf)
                    {
                        return fidx;
                    }
                }
            }
        }
    }

    return -1;
}


// Initialze PMM
void pmm_init(mb_info_t* mbt)
{
    if (!(mbt -> flags & (1 << 6)))
    {
        punchout("No memory map was provided.\n");
        return;
    }


    // Determine amount of memory available
    uint32_t tot_mem = 0;
    mb_memmap_entry_t* mmap = (mb_memmap_entry_t*)mbt -> mmap_addr;

    while((uint32_t)mmap < mbt -> mmap_addr + mbt -> mmap_length)
    {
        // If the memory is useable
        if (mmap -> type == 1)
        {
            uint64_t r_end = mmap -> addr + mmap -> len;

            if (r_end > tot_mem)
            {
                tot_mem = r_end;
            }
        }

        mmap = (mb_memmap_entry_t*)((uint32_t)mmap + mmap -> size + sizeof(mmap -> size));
    }

    pmm_tf = tot_mem / PMM_F_SIZE;
    pmm_bm_s = pmm_tf / PMM_FPB;

    if (pmm_tf % PMM_FPB)
    {
        pmm_bm_s++;
    }


    // Find a place for the bitmap
    // Bitmap will go after the kernel's end symbol
    extern uint32_t end;
    pmm_bm = (uint32_t*)&end;


    // Initialize bitmap, mark all memory as used
    for (uint32_t i = 0; i < pmm_bm_s / 4; ++i)
    {
        pmm_bm[i] = 0xFFFFFFFF;
    }

    
    // Mark any useable region as being free
    mmap = (mb_memmap_entry_t*)mbt -> mmap_addr;

    while ((uint32_t)mmap < mbt -> mmap_addr + mbt -> mmap_length)
    {
        if (mmap -> type == 1)
        {
            uint32_t start_frame = mmap -> addr / PMM_F_SIZE;
            uint32_t num_frames = mmap -> len / PMM_F_SIZE;

            for (uint32_t i = 0; i < num_frames; ++i)
            {
                pmm_bitclear(start_frame + i);
            }
        }

        mmap = (mb_memmap_entry_t*)((uint32_t)mmap + mmap -> size + sizeof(mmap -> size));
    }

    // Mark kernel, bitmap regions as being used
    uint32_t k_start_frame = 0x100000 / PMM_F_SIZE;
    uint32_t k_end_addr = (uint32_t)&end;
    uint32_t bm_end_addr = k_end_addr + pmm_bm_s;
    uint32_t used_frames = (bm_end_addr - 0x100000) / PMM_F_SIZE + 1;

    for (uint32_t i = 0; i < used_frames; ++i)
    {
        pmm_bitset(k_start_frame + i);
    }

    log(LOG_INFO, "Physical memory manager initialized.\n");
    print_white("Total memory available: ");
    print_dec(tot_mem / 1024);
    print_white(" KB\n");
    print_white("Total frames available: ");
    print_dec(pmm_tf);
    print_white(" frames\n");
    print_white("Bitmap size: ");
    print_dec(pmm_bm_s);
    print_white(" bytes\n");
}


uint32_t pmm_alloc_frame()
{
    int32_t fidx = pmm_first_free();

    if (fidx == -1)
    {
        punchout("No frames are available.\n");
        return 0;
    }

    pmm_bitset(fidx);
    return fidx * PMM_F_SIZE;
}


void pmm_free(uint32_t f_addr)
{
    if (f_addr % PMM_F_SIZE != 0)
    {
        log(LOG_ERROR, "Invalid frame address: 0x");
        print_hex(f_addr);
        return;
    }

    uint32_t fidx = f_addr / PMM_F_SIZE;

    if (fidx >= pmm_tf)
    {
        log(LOG_ERROR, "Frame's address is out of bounds: 0x");
        print_hex(f_addr);
        return;
    }

    pmm_bitclear(fidx);
}