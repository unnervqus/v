#pragma once

#include "core/vtypes.h"


// vArray only store items by value
// For refence-related array use vArrayRef 
typedef struct vArray* vArray;
typedef bool (*vArrayPredicateFn)(const void* element);
typedef bool (*vArrayCompareFn)(const void* a, const void* b);

vArray vArrayCreate(u32 element_size, u32 initial_capacity);
void vArrayDestroy(vArray);

u32 vArrayGetSize(vArray);
u32 vArrayGetCapacity(vArray);
u32 vArrayGetElementSize(vArray);

bool vArrayIsEmpty(vArray);
void vArrayClear(vArray);

bool vArrayPush(vArray, const void* element);
bool vArrayPop(vArray, void* out_element);

bool vArraySet(vArray, u32 index, const void* element);
bool vArrayRemove(vArray, u32 index);

void vArrayResize(vArray, u32 new_size);
void vArrayReserve(vArray, u32 new_capacity);

void* vArrayGet(vArray array, u32 index);
const void* vArrayGetConst(vArray array, u32 index);

vArray vArrayFilter(vArray, vArrayPredicateFn predicate);

void* vArrayFind(vArray array, vArrayPredicateFn predicate);

bool vArrayContains(vArray, const void* element, vArrayCompareFn compare);

u32 vArrayIndexOf(vArray, const void* element, vArrayCompareFn compare);

u32 vArrayRemoveIf(vArray, vArrayPredicateFn predicate);