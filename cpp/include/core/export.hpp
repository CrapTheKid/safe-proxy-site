#pragma once

#ifdef _WIN32
  #ifdef SAFE_PROXY_BUILD_DLL
    #define SAFE_PROXY_API __declspec(dllexport)
  #else
    #define SAFE_PROXY_API __declspec(dllimport)
  #endif
#else
  #define SAFE_PROXY_API
#endif
