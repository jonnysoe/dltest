#ifndef LIBPLUGIN_H
#define LIBPLUGIN_H

#ifdef _WIN32
    #ifdef BUILD_LIBPLUGIN
        #define LIBPLUGIN_DLL __declspec(dllexport)
    #else
        #define LIBPLUGIN_DLL __declspec(dllimport)
    #endif
#else
    #define LIBPLUGIN_DLL
#endif

LIBPLUGIN_DLL void lib1_A();
LIBPLUGIN_DLL void lib1_B();
LIBPLUGIN_DLL void lib1_C();

#endif