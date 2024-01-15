#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
    #ifdef BUILD_COMMON
        #define COMMON_DLL __declspec(dllexport)
    #else
        #define COMMON_DLL __declspec(dllimport)
    #endif
#else
    #define COMMON_DLL
#endif

COMMON_DLL void DisplayNumbers();
COMMON_DLL void AddNumbers(int num);

#endif