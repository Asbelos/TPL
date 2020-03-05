void Serialprint(const char* input...);
#ifndef DIAG_ENABLED
 #define DIAG_ENABLED true
#endif
#define DIAG if (DIAG_ENABLED) Serialprint
