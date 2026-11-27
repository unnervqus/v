#include "core/vfile.h"

#include <stdio.h>

u64 vFileLength(const char* path) {
    FILE* file;

    fopen_s(&file, path, "rb");

    fseek(file, 0, SEEK_END);

    u64 len = ftell(file);

    fclose(file);
    
    return len;
}

void vFileRead(const char* path, u8* buffer, u32 buffer_size) {
    FILE* file;
    u32 file_len = vFileLength(path);
    
    fopen_s(&file, path, "rb");
    
    fread_s(
        buffer, buffer_size, 
        sizeof(u8), file_len, 
        file
    );

    fclose(file);
}