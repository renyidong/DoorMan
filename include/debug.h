#ifdef DEBUG
  //#undef printf
  //#define printf printf_fast
  #define dprintf(...) printf(__VA_ARGS__);
#else
  #define dprintf(...)
#endif
