#include "small_ptr.h"
#include <cassert>

static void* g_begin = nullptr;
static void* g_end = nullptr;

void small_ptr_set_heap_extents(void* base, size_t size)
{
    assert(size <= UINT32_MAX);
    g_begin = base;
    g_end = reinterpret_cast<char*>(base) + size;
}

uint32_t small_ptr_shrink(void* ptr)
{
    assert(ptr >= g_begin && ptr < g_end);
    return reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(g_begin);
}

void* small_ptr_expand(uint32_t offset)
{
    return reinterpret_cast<char*>(g_begin) + offset;
}
