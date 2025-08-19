#pragma once

// Ngăn chặn macro định nghĩa lại
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Bao gồm Winsock2 trước Windows.h để tránh xung đột
#include <winsock2.h>
#include "windows.h"
