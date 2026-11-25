#include "core/varray.h"
#include "core/vmemory.h"
#include "core/vtypes.h"

#define V_ARRAY_ADDRESS(array, index) ((array)->data + (index) * (array)->element_size)

struct vArray {
    vbytes data;
    u32 size;
    u32 capacity;
    u32 element_size;
};

vArray vArrayCreate(u32 element_size, u32 initial_capacity) {
    vArray array = vMalloc(sizeof(struct vArray));
    // allocate linear array of bytes for each element
    array->data = vMalloc(element_size * initial_capacity);
    array->size = 0;
    array->capacity = initial_capacity;
    array->element_size = element_size;
    return array;
}

void vArrayDestroy(vArray array) {
    vFree(array->data);
    vFree(array);
}

inline u32 vArrayGetSize(vArray array) {
    return array->size;
}

inline u32 vArrayGetCapacity(vArray array) {
    return array->capacity;
}

inline u32 vArrayGetElementSize(vArray array) {
    return array->element_size;
}

inline bool vArrayIsEmpty(vArray array) {
    return array->size == 0;
}

inline void vArrayClear(vArray array) {
    array->size = 0;
}

bool vArrayPush(vArray array, const void* element) {
    if(array->size >= array->capacity) {
        u32 new_capacity = array->capacity * 2;
        void* new_data = vRealloc(array->data, new_capacity * array->element_size);
        array->data = new_data;
        array->capacity = new_capacity;
    }
    vMemoryCopy(V_ARRAY_ADDRESS(array, array->size), element, array->element_size);
    array->size++;
    return true;
}

bool vArrayPop(vArray array, void* out_element) {
    if (array->size == 0 || !out_element) {
        return false;
    }
    array->size--;
    vMemoryCopy(out_element, V_ARRAY_ADDRESS(array, array->size), array->element_size);
    return true;
}

bool vArraySet(vArray array, u32 index, const void* element) {
    if(index > array->size) {
        return false;
    }
    vMemoryCopy(V_ARRAY_ADDRESS(array, index), element, array->element_size);
    return true;
}
bool vArrayRemove(vArray array, u32 index) {
    if(index >= array->size) {
        return false;
    }
    for(u32 i = index; i < array->size - 1; ++i) {
        vMemoryCopy(V_ARRAY_ADDRESS(array, i), V_ARRAY_ADDRESS(array, i + 1), array->element_size);
    }
    array->size--;
    return true;
}

void vArrayResize(vArray array, u32 new_size) {
    if(new_size > array->capacity) {
        vArrayReserve(array, new_size);
    }
    array->size = new_size;
}
void vArrayReserve(vArray array, u32 new_capacity) {
    if(new_capacity <= array->capacity) {
        return;
    }
    void* new_data = vRealloc(array->data, new_capacity * array->element_size);
    array->data = new_data;
    array->capacity = new_capacity;
}

void* vArrayGet(vArray array, u32 index) {
    if(index >= array->size) {
        return NULL;
    }
    return V_ARRAY_ADDRESS(array, index);
}

const void* vArrayGetConst(vArray array, u32 index) {
    if(index >= array->size) {
        return NULL;
    }
    return V_ARRAY_ADDRESS(array, index);
}

vArray vArrayFilter(vArray array, vArrayPredicateFn predicate) {
    vArray new_array = vArrayCreate(
        array->element_size,
        array->capacity
    );

    for(u32 i = 0; i < array->size; ++i) {
        const void* element = vArrayGetConst(array, i);

        if(predicate(element)) {
            vArrayPush(new_array, element);
        }
    }

    return new_array;
}

void* vArrayFind(vArray array, vArrayPredicateFn predicate) {
    for(u32 i = 0; i < array->size; ++i) {
        const void* element = vArrayGetConst(array, i);
        if(predicate(element)) {
            return vArrayGet(array, i);
        }
    }
    return NULL;
}

bool vArrayContains(vArray array, const void* element, vArrayCompareFn compare) {
    return vArrayIndexOf(array, element, compare) != (u32)-1;
}

u32 vArrayIndexOf(vArray array, const void* element, vArrayCompareFn compare) {
    for(u32 i = 0; i < array->size; ++i) {
        const void* current_element = vArrayGetConst(array, i);
        if(compare(current_element, element)) {
            return i;
        }
    }
    return (u32)-1;
}

u32 vArrayRemoveIf(vArray array, vArrayPredicateFn predicate) {
    u32 removed_count = 0;
    for(u32 i = 0; i < array->size; ++i) {
        const void* element = vArrayGetConst(array, i);
        if(predicate(element)) {
            vArrayRemove(array, i);
            removed_count++;
            i--; // Adjust index after removal
        }
    }
    return removed_count;
}