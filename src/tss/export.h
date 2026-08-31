#if defined(_WIN32) || defined(__CYGWIN__)
  #ifdef TSS_API_EXPORTS
    #define TSS_API __declspec(dllexport)
  #else
    #define TSS_API
  #endif
#elif defined(__GNUC__) || defined(__clang__)
  #define TSS_API __attribute__((visibility("default")))
#else
  #define TSS_API
#endif