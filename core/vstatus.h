#pragma once


typedef enum vStatus {
    V_STATUS_OK = 0,
    V_STATUS_ERROR = 1,
    V_STATUS_NOT_FOUND = 2,
    V_STATUS_INVALID_ARGUMENT = 3,
    V_STATUS_OUT_OF_MEMORY = 4,
    V_STATUS_UNSUPPORTED = 5,
} vStatus;