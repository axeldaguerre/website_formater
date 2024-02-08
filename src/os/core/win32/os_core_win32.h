#include <Windows.h>

struct W32_FileIter
{
    HANDLE hnd;
    WIN32_FIND_DATAW find_data;
};
