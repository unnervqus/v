#include "core/vstring.h"
#include "core/vmemory.h"
#include "core/vlogging.h"

#include <string.h>

struct vString {
    u32 capacity;
    u32 size;
    char* data;
};

static u32 vStringConvertAtoW(vString string, wchar_t* out, u32 wcapacity) {
    if (!string || !out || wcapacity == 0)
        return 0;

    size_t length = mbstowcs(out, string->data, wcapacity - 1);

    if (length == (size_t)-1) {
        out[0] = L'\0';
        return 0;
    }

    out[length] = L'\0';

    return (u32)length;
}

vString vStringCreate(const char* data, u32 capacity) {
    vString string = vMalloc(sizeof(struct vString));

    u32 data_len = strlen(data);
    if(data_len > capacity) {
        vLog("[STRING] Data length is bigger then capacity: %u > %u", data_len, capacity);
        return NULL;
    }

    string->data = vMalloc(sizeof(char) * capacity);
    vMemoryCopy(string->data, data, data_len);

    string->data[data_len] = '\0';
    string->size = data_len;
    string->capacity = capacity;

    return string;
}
void vStringDestroy(vString string) {
    vFree(string->data);
    vFree(string);
}

const char* vStringGetData(vString string) {
    return string->data;
}

u32 vStringGetDataW(vString string, wchar_t* out, u32 wcapacity) {
    return vStringConvertAtoW(string, out, wcapacity);
}

u32 vStringGetCapacity(vString string) {
    return string->capacity;
}

u32 vStringGetSize(vString string) {
    return string->size;
}

vString vStringCopy(vString string) {
    vString new_string = vStringCreate(string->data, string->capacity);
    return new_string;
}