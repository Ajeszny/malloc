#include "heap.h"
#include "custom_unistd.h"
#include <stdio.h>
#include "tested_declarations.h"
#include "rdebug.h"
#include "tested_declarations.h"
#include "rdebug.h"

const int base_plotek = 0x11111111;

#define PAGE_SIZE       4096    // Długość strony w bajtach
#define PAGE_FENCE      1       // Liczba stron na jeden płotek
#define PAGES_AVAILABLE 16380   // Liczba stron dostępnych dla sterty
#define PAGES_TOTAL     (PAGES_AVAILABLE + 2 * PAGE_FENCE)
#define CHUNK_SIZE      sizeof(memory_unit)
#define PLOTEK_SIZE     sizeof(int)



typedef struct allocated_memory {
    struct allocated_memory* prev;
    struct allocated_memory* prev_check;
    struct allocated_memory* next;
    struct allocated_memory* next_check;
    char free;
    char free_check;
    size_t size;
    size_t size_check;
    size_t checksum;
}memory_unit;
void memunit_modify_next(memory_unit* this, memory_unit* next);
void memunit_modify_prev (memory_unit* this, memory_unit* prev);
void memunit_modify_free (memory_unit* this, char free);
void memunit_modify_size (memory_unit* this, size_t size);
void memunit_modify_checksum(memory_unit* this);
size_t memunit_get_checksum(memory_unit* this);
_Bool memunit_check(memory_unit* this);

void memunit_modify_next(memory_unit* this, memory_unit* next) {
    this->next = next;
    this->next_check = next;
    memunit_modify_checksum(this);
}

void memunit_modify_prev (memory_unit* this, memory_unit* prev) {
    this->prev = prev;
    this->prev_check = prev;
    memunit_modify_checksum(this);
}

void memunit_modify_free (memory_unit* this, char free) {
    this->free = free;
    this->free_check = free;
    memunit_modify_checksum(this);
}

void memunit_modify_size (memory_unit* this, size_t size) {
    this->size = size;
    this->size_check = size;
    *(int*)((uint8_t*)this + this->size + CHUNK_SIZE + PLOTEK_SIZE) = base_plotek;
    memunit_modify_checksum(this);
}

size_t memunit_get_checksum_d(memory_unit* this) {
    size_t sum = 0;
    if (heap_validate() == 1) {
        heap_validate();
    }
    uint8_t* ptr = (uint8_t*) this;
    if (heap_validate() == 1) {
        heap_validate();
    }
    for (size_t i = 0; i < CHUNK_SIZE - sizeof(size_t); ++i) {
        sum += ptr [i];
        if (heap_validate() == 1) {
            heap_validate();
        }
    }
    sum += 42;
    if (heap_validate() == 1) {
        heap_validate();
    }
    return sum;
}

void memunit_modify_checksum_d(memory_unit* this) {

    int* plotek1 = (int*)((uint8_t*)this + this->size + CHUNK_SIZE + PLOTEK_SIZE);

    if (heap_validate() == 1&&plotek1) {
        heap_validate();
    }
    size_t* kok = &this->checksum;
    *kok = 0;
    if (heap_validate() == 1) {
        heap_validate();
    }
    int* plotek = (int*)((uint8_t*)this + this->size + CHUNK_SIZE + PLOTEK_SIZE);
    if (!custom_sbrk(0)) {
        return;
    }
    size_t sum = memunit_get_checksum_d(this);
    *kok = sum;
    if (heap_validate() == 1&&plotek) {
        heap_validate();
    }
    if (heap_validate() == 1) {
        heap_validate();
    }
}

void memunit_modify_size_d (memory_unit* this, size_t size) {
    this->size = size;
    this->size_check = size;
    *(int*)((uint8_t*)this + this->size + CHUNK_SIZE + PLOTEK_SIZE) = base_plotek;
    if (heap_validate() == 1) {
        heap_validate();
    }
    memunit_modify_checksum_d(this);
    if (heap_validate() == 1) {
        heap_validate();
    }
}

void memunit_modify_checksum(memory_unit* this) {

    this->checksum = memunit_get_checksum(this);
}

size_t memunit_get_checksum(memory_unit* this) {
    size_t sum = 0;
    uint8_t* ptr = (uint8_t*) this;
    for (size_t i = 0; i < CHUNK_SIZE - sizeof(size_t); ++i) {
        sum += ptr [i];
    }
    sum += 42;
    return sum;
}

_Bool memunit_check(memory_unit* this) {
    size_t chck = memunit_get_checksum(this);
    return (this->free == this->free_check
    &&this->next == this->next_check
    && this->size == this->size_check
    && this->prev == this->prev_check
    &&this->checksum == chck);
}

struct heap_t {
    size_t avail;
    size_t used;
    void* memstart;
    memory_unit* first_unit;
    memory_unit* last_unit;
    size_t chunks;
    size_t malloc_invocations;
    size_t free_invocations;
    size_t realloc_invocations;
} heap;

memory_unit* find_free_blocks(size_t size) {
    memory_unit* cursor = heap.first_unit;
    while(cursor->next) {
        if (cursor->free&& cursor->size >= size) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return 0;
}

memory_unit* find_free_blocks_aligned(size_t size) {
    memory_unit* cursor = heap.first_unit;
    while(cursor->next) {
        uint8_t* ptr = (uint8_t*)cursor + CHUNK_SIZE + PLOTEK_SIZE;
        if (cursor->free&& cursor->size >= size&& ((intptr_t)ptr & (intptr_t)(PAGE_SIZE - 1)) == 0) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return 0;
}

int heap_setup(void) {
    void* memstart = custom_sbrk(0);
    if (memstart == (void*)-1) {
        return -1;//Zdechło
    }
    heap.memstart = memstart;
    heap.avail = PAGE_SIZE * PAGES_TOTAL;
    heap.used = 0;
    heap.first_unit = NULL;
    heap.chunks = 0;
    return 0;
}

void heap_clean(void) {
    heap.chunks = 0;
    heap.first_unit = 0;
    heap.last_unit = 0;
    heap.memstart = 0;
    heap.avail = 0;
    custom_sbrk(heap.used * -1);
    heap.used = 0;

}

void *heap_malloc(size_t size) {
    heap.malloc_invocations++;
    struct heap_t* heap_debug_ptr = &heap;
    if (!heap_debug_ptr) {
        return NULL;
    }
    if (size < 1||size + heap.used >= heap.avail) {
        return NULL;
    }
    if (!heap.first_unit) {
        heap.used = CHUNK_SIZE + size + PLOTEK_SIZE * 2;

        heap.memstart = custom_sbrk(size + PLOTEK_SIZE*2 + CHUNK_SIZE);
        if (heap.memstart == (void *) -1) {
            heap.used = 0;
            heap.memstart = NULL;
            return NULL;
        }
        memory_unit *chunk = (memory_unit *) heap.memstart;
        *(int*)((uint8_t *)chunk + CHUNK_SIZE) = base_plotek;
        memunit_modify_size(chunk, size);
        memunit_modify_next(chunk, NULL);
        memunit_modify_prev(chunk, heap.first_unit);
        memunit_modify_free(chunk, 0);
        *(int*)((uint8_t *)chunk + CHUNK_SIZE + size + PLOTEK_SIZE) = base_plotek;
        heap.first_unit = chunk;
        heap.last_unit = chunk;
        heap.chunks = 1;

        return ((uint8_t *)chunk) + PLOTEK_SIZE + CHUNK_SIZE;
    }
    memory_unit* free_block = find_free_blocks(size);
    if (free_block) {
        memunit_modify_size(free_block, size);
        memunit_modify_free(free_block, 0);
        *(int*)((uint8_t *)free_block + CHUNK_SIZE) = base_plotek;
        *(int*)((uint8_t *)free_block + CHUNK_SIZE + PLOTEK_SIZE + size) = base_plotek;
        return ((uint8_t*) free_block + PLOTEK_SIZE + CHUNK_SIZE);

    }
    memory_unit* cursor = heap.first_unit;
    while(cursor->next) {
        cursor = cursor->next;
    }

    uint8_t* memory = custom_sbrk(size + CHUNK_SIZE + PLOTEK_SIZE*2);
    memory_unit * new_unit = (memory_unit*) memory;
    memunit_modify_next(new_unit, NULL);
    memunit_modify_prev(new_unit, cursor);
    memunit_modify_size(new_unit, size);
    memunit_modify_free(new_unit, 0);
    uint8_t* plotek_pos = (memory + CHUNK_SIZE);

    memunit_modify_next(cursor, new_unit);
    *(int*)plotek_pos = base_plotek;
    plotek_pos += PLOTEK_SIZE + size;
    *(int*)plotek_pos = base_plotek;

    heap.last_unit = new_unit;
    heap.chunks++;
    heap.used += size + CHUNK_SIZE + PLOTEK_SIZE*2;
    return memory + CHUNK_SIZE + PLOTEK_SIZE;
}

void *heap_calloc(size_t number, size_t size) {
    if (number < 1||size < 1) {
        return 0;
    }
    void* ptr = heap_malloc(number*size);
    if (!ptr) {
        return NULL;
    }
    for(size_t i=0;i<size*number;i++){
        *((uint8_t*)ptr + i) = 0;
    }
    return ptr;
}

void *heap_realloc(void *memblock, size_t count) {
    heap.realloc_invocations++;
    if (get_pointer_type(memblock) != pointer_valid&& memblock != NULL) {
        return NULL;
    }
    if (!heap.memstart||heap_validate()) {
        return NULL;
    }
    if (count == 0) {
        heap_free(memblock);
        return NULL;
    }
    if (!memblock) {
        return heap_malloc(count);
    }
    memory_unit* cursor = (memory_unit*)((uint8_t*)memblock - PLOTEK_SIZE - CHUNK_SIZE);
    if (cursor->size == count) {
        return memblock;
    }
    if (cursor->next) {
        size_t bytes = (size_t)cursor->next - (size_t)cursor - CHUNK_SIZE - PLOTEK_SIZE - PLOTEK_SIZE;
        if (cursor->size < bytes) {
            memunit_modify_size(cursor, bytes);
            if (heap_validate() == 1) {
                heap_validate();
            }
        }
    }
    if (cursor->size >= count) {

        memunit_modify_size(cursor, count);
        if (heap_validate() == 1) {
            heap_validate();
        }
        *(int*)((uint8_t*)memblock + count) = base_plotek;
        return memblock;
    }
    if (!cursor->next) {
        if (custom_sbrk(count - cursor->size) == (void*)-1) {
            return NULL;
        }
        heap.used += (count - cursor->size);

        memunit_modify_size_d(cursor, count);

        if (heap_validate() == 1) {
            heap_validate();
        }
        *(int*)((uint8_t*)memblock + cursor->size) = base_plotek;
        return memblock;
    }

    memory_unit* cursor2 = heap.first_unit;

    memory_unit * tmp = (memory_unit*)cursor2;

    while(cursor2) {
        if(cursor2->free == 1) {
            tmp = cursor2;
        }

        cursor2 = cursor2->next;

        if(tmp->free && cursor2 && cursor2->free == 1) {
            memunit_modify_size(tmp, tmp->size + cursor2->size + CHUNK_SIZE + PLOTEK_SIZE*2);

            memunit_modify_next(tmp, cursor2->next);

            if(cursor2->next) {
                memunit_modify_prev(cursor2->next, tmp);
            }
        } else {
            tmp = tmp->next;
        }

        if(!cursor2->next) {
            break;
        }

        cursor2 = tmp;
    }

    if (cursor->next->free) {
        if ((cursor->size + cursor->next->size + CHUNK_SIZE + PLOTEK_SIZE*2) >= count) {
            memunit_modify_next(cursor, cursor->next->next);
            memunit_modify_size(cursor, count);

            if (cursor->next) {
                memunit_modify_prev(cursor->next, cursor);
            }
            *(int*)((uint8_t*)memblock + cursor->size) = base_plotek;
            return memblock;
        }
    }

    uint8_t* old = (uint8_t*)memblock;
    void* new_memblock = heap_malloc(count);
    if (!new_memblock) {
        return NULL;
    }
    uint8_t* new = (uint8_t*)new_memblock;
    memcpy(new, old, cursor->size);
    heap_free(memblock);


    return new_memblock;
}

void heap_free(void *memblock) {
    heap.free_invocations++;
    if (!memblock) {
        return;
    }
    if (get_pointer_type(memblock) != pointer_valid) {
        return;
    }

    memory_unit* cursor = (memory_unit*)((uint8_t*)memblock - PLOTEK_SIZE - CHUNK_SIZE);
    memunit_modify_free(cursor, 1);

        if (cursor->next) {
            size_t s = (uint8_t*)cursor->next - (uint8_t*)cursor - PLOTEK_SIZE * 2 - CHUNK_SIZE;
            if (s > cursor->size) {
                memunit_modify_size(cursor, s);
            }
        }//Checks if there is some wolna przestrzeń between the freed chunk and the next one


    int flag = 1;

    while(cursor != NULL) {
        if(!cursor->free) {
            flag = 0;
        }

        cursor = cursor->next;
    }

    if(flag) {
        heap.chunks = 0;
        heap.first_unit = 0;
        heap.last_unit = 0;
        custom_sbrk(heap.used * -1);
        heap.used = 0;
    }
    memory_unit* cursor2 = heap.first_unit;

    memory_unit * tmp = (memory_unit*)cursor2;

    while(cursor2) {
        if(cursor2->free == 1) {
            tmp = cursor2;
        }

        cursor2 = cursor2->next;

        if(tmp->free && cursor2 && cursor2->free == 1) {
            memunit_modify_size(tmp, tmp->size + cursor2->size + CHUNK_SIZE + PLOTEK_SIZE*2);

            memunit_modify_next(tmp, cursor2->next);

            if(cursor2->next) {
                memunit_modify_prev(cursor2->next, tmp);
            }
        } else {
            tmp = tmp->next;
        }

        if(!cursor2->next) {
            break;
        }

        cursor2 = tmp;
    }
    return;
}

size_t heap_get_largest_used_block_size(void) {
    if (!heap.first_unit||!heap.memstart||!heap.used||!heap.chunks) {
        return 0;
    }
    if (heap_validate()) {
        return 0;
    }
    memory_unit * cursor = heap.first_unit;
    size_t max_size = (cursor->free)?0:cursor->size;

    do {
        if (!cursor->free&&(cursor->size > max_size)) {max_size = cursor->size;}
        cursor = cursor->next;
    }while (cursor);
        return max_size;
}
enum pointer_type_t get_pointer_type(const void* const pointer) {
    if(pointer == NULL) {
        return pointer_null;
    }

    if(heap_validate() == 1) {
        return pointer_heap_corrupted;
    }

    memory_unit *cursor = heap.first_unit;

    while(cursor){
        if(!cursor->free) {
            size_t result = (size_t)cursor + CHUNK_SIZE;
            uint8_t *gorny_plotek = (uint8_t*)cursor + CHUNK_SIZE, *dolny_plotek = (uint8_t*)cursor + CHUNK_SIZE + PLOTEK_SIZE + cursor->size;
            uint8_t* memstart = (uint8_t*)cursor + CHUNK_SIZE + PLOTEK_SIZE;
            if((uint8_t*)cursor <= (uint8_t*)pointer && (uint8_t*)result > (uint8_t*)pointer) {
                return pointer_control_block;
            }

            if((uint8_t*)pointer >= gorny_plotek && (uint8_t*)pointer < memstart) {
                return pointer_inside_fences;
            }

            if((uint8_t*)pointer == memstart) {
                return pointer_valid;
            }

            if((uint8_t *)pointer > memstart && (uint8_t *)pointer < dolny_plotek) {
                return pointer_inside_data_block;
            }

            if((uint8_t*)pointer >= dolny_plotek && (uint8_t*)pointer < dolny_plotek + PLOTEK_SIZE) {
                return pointer_inside_fences;
            }
        }

        cursor = cursor->next;
    }

    return pointer_unallocated;
}


int heap_validate(void){
    if (!heap.memstart) {
        return 2;
    }

    memory_unit* cursor = heap.first_unit;
    if (!cursor) {
        return 0;
    }

    while (cursor) {
        uint8_t* ptr = (uint8_t*)cursor;
        if (!memunit_check(cursor)) {
            return 3;
        }
        int* plotek_1 = (int*) (ptr+CHUNK_SIZE);
        int* plotek_2 = (int*) (ptr+CHUNK_SIZE + cursor->size + PLOTEK_SIZE);

        if (*plotek_1 != base_plotek) {
            return 1;
        } if (*plotek_2  != base_plotek) {
            return 1;
        }
        cursor = cursor->next;
    }

    return 0;
}

void* get_next_chunk_pos() {
    uint8_t* current_cursor = (uint8_t*)custom_sbrk(0);
    short current_page_remained_size = PAGE_SIZE - (short)(heap.used % PAGE_SIZE);//Zawsze < 4096
    if (current_page_remained_size >= (short)(PLOTEK_SIZE + CHUNK_SIZE)) {
        return ((current_cursor + current_page_remained_size) - (PLOTEK_SIZE + CHUNK_SIZE));
    }
    uint8_t* end_pos = (current_cursor + current_page_remained_size) + //Round to next page
            PAGE_SIZE - (PLOTEK_SIZE + CHUNK_SIZE);
    return end_pos;
}

void* heap_malloc_aligned(size_t size) {
    heap.malloc_invocations++;
    struct heap_t* heap_debug_ptr = &heap;
    if (!heap_debug_ptr) {
        return NULL;
    }
    if (size < 1||size + heap.used >= heap.avail) {
        return NULL;
    }
    if (!heap.first_unit) {
        size_t to_allocate = (size_t)get_next_chunk_pos() - (size_t)custom_sbrk(0);
        heap.used += to_allocate;
        heap.memstart = custom_sbrk(to_allocate);

        heap.used += CHUNK_SIZE + size + PLOTEK_SIZE * 2;
        heap.memstart = custom_sbrk(size + PLOTEK_SIZE*2 + CHUNK_SIZE);

        if (heap.memstart == (void *) -1) {
            heap.memstart = NULL;
            return NULL;
        }
        memory_unit *chunk = (memory_unit *) heap.memstart;
        *(int*)((uint8_t *)chunk + CHUNK_SIZE) = base_plotek;
        memunit_modify_size(chunk, size);
        memunit_modify_next(chunk, NULL);
        memunit_modify_prev(chunk, heap.first_unit);
        memunit_modify_free(chunk, 0);
        *(int*)((uint8_t *)chunk + CHUNK_SIZE + size + PLOTEK_SIZE) = base_plotek;
        heap.first_unit = chunk;
        heap.last_unit = chunk;
        heap.chunks = 1;

        return ((uint8_t *)chunk) + PLOTEK_SIZE + CHUNK_SIZE;
    }
    memory_unit* free_block = find_free_blocks_aligned(size);
    if (free_block) {
        memunit_modify_size(free_block, size);
        memunit_modify_free(free_block, 0);
        *(int*)((uint8_t *)free_block + CHUNK_SIZE) = base_plotek;
        *(int*)((uint8_t *)free_block + CHUNK_SIZE + PLOTEK_SIZE + size) = base_plotek;
        return ((uint8_t*) free_block + PLOTEK_SIZE + CHUNK_SIZE);

    }
    memory_unit* cursor = heap.first_unit;
    while(cursor->next) {
        cursor = cursor->next;
    }

    size_t to_allocate = (size_t)get_next_chunk_pos() - (size_t)custom_sbrk(0);
    heap.used += to_allocate;
    heap.memstart = custom_sbrk(to_allocate);

    uint8_t* memory = custom_sbrk(size + CHUNK_SIZE + PLOTEK_SIZE*2);
    memory_unit * new_unit = (memory_unit*) memory;
    memunit_modify_next(new_unit, NULL);
    memunit_modify_prev(new_unit, cursor);
    memunit_modify_size(new_unit, size);
    memunit_modify_free(new_unit, 0);
    uint8_t* plotek_pos = (memory + CHUNK_SIZE);

    memunit_modify_next(cursor, new_unit);
    *(int*)plotek_pos = base_plotek;
    plotek_pos += PLOTEK_SIZE + size;
    *(int*)plotek_pos = base_plotek;

    heap.last_unit = new_unit;
    heap.chunks++;
    heap.used += size + CHUNK_SIZE + PLOTEK_SIZE*2;
    return memory + CHUNK_SIZE + PLOTEK_SIZE;
}
void* heap_calloc_aligned(size_t number, size_t size) {
    if (number < 1||size < 1) {
        return 0;
    }
    void* ptr = heap_malloc_aligned(number*size);
    if (!ptr) {
        return NULL;
    }
    for(size_t i=0;i<size*number;i++){
        *((uint8_t*)ptr + i) = 0;
    }
    return ptr;
}
void* heap_realloc_aligned(void* memblock, size_t count) {
    heap.realloc_invocations++;
    if (get_pointer_type(memblock) != pointer_valid&& memblock != NULL) {
        return NULL;
    }
    if (!heap.memstart||heap_validate()) {
        return NULL;
    }
    if (count == 0) {
        heap_free(memblock);
        return NULL;
    }
    if (!memblock) {
        return heap_malloc_aligned(count);
    }
    memory_unit* cursor = (memory_unit*)((uint8_t*)memblock - PLOTEK_SIZE - CHUNK_SIZE);
    if (cursor->size == count) {
        return memblock;
    }
    if (cursor->next) {
        size_t bytes = (size_t)cursor->next - (size_t)cursor - CHUNK_SIZE - PLOTEK_SIZE - PLOTEK_SIZE;
        if (cursor->size < bytes) {
            memunit_modify_size(cursor, bytes);
            if (heap_validate() == 1) {
                heap_validate();
            }
        }
    }
    if (cursor->size >= count) {

        memunit_modify_size(cursor, count);
        if (heap_validate() == 1) {
            heap_validate();
        }
        *(int*)((uint8_t*)memblock + count) = base_plotek;
        return memblock;
    }
    if (!cursor->next) {

        if (custom_sbrk(count - cursor->size) == (void*)-1) {
            return NULL;
        }
        heap.used += (count - cursor->size);

        memunit_modify_size_d(cursor, count);

        if (heap_validate() == 1) {
            heap_validate();
        }
        *(int*)((uint8_t*)memblock + cursor->size) = base_plotek;
        return memblock;
    }

    memory_unit* cursor2 = heap.first_unit;

    memory_unit * tmp = (memory_unit*)cursor2;

    while(cursor2) {
        if(cursor2->free == 1) {
            tmp = cursor2;
        }

        cursor2 = cursor2->next;

        if(tmp->free && cursor2 && cursor2->free == 1) {
            memunit_modify_size(tmp, tmp->size + cursor2->size + CHUNK_SIZE + PLOTEK_SIZE*2);

            memunit_modify_next(tmp, cursor2->next);

            if(cursor2->next) {
                memunit_modify_prev(cursor2->next, tmp);
            }
        } else {
            tmp = tmp->next;
        }

        if(!cursor2->next) {
            break;
        }

        cursor2 = tmp;
    }

    if (cursor->next->free) {
        if ((cursor->size + cursor->next->size + CHUNK_SIZE + PLOTEK_SIZE*2) >= count) {
            memunit_modify_next(cursor, cursor->next->next);
            memunit_modify_size(cursor, count);

            if (cursor->next) {
                memunit_modify_prev(cursor->next, cursor);
            }
            *(int*)((uint8_t*)memblock + cursor->size) = base_plotek;
            return memblock;
        }
    }

    uint8_t* old = (uint8_t*)memblock;
    void* new_memblock = heap_malloc_aligned(count);
    if (!new_memblock) {
        return NULL;
    }
    uint8_t* new = (uint8_t*)new_memblock;
    memcpy(new, old, cursor->size);
    heap_free(memblock);


    return new_memblock;
}


