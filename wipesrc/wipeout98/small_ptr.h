#ifndef SMALL_PTR_H
#define SMALL_PTR_H

#include <cstddef>

void small_ptr_set_heap_extents(void*, size_t);
uint32_t small_ptr_shrink(void*);
void* small_ptr_expand(uint32_t);

template<typename T>
class SmallPtr
{
public:
    SmallPtr() = default;

    explicit SmallPtr(T* ptr)
        : m_offset(small_ptr_shrink(ptr))
    {}

    operator T*() const {
        return get_ptr();
    }

    T* operator->() const {
        return get_ptr();
    }

    T& operator*() const {
        return *get_ptr();
    }

private:
    T* get_ptr() const {
        return reinterpret_cast<T*>(small_ptr_expand(m_offset));
    }

    uint32_t m_offset;
};

#endif
