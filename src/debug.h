#ifndef DEBUG_H_
#define DEBUG_H_

enum DEBUG{D_ACT, D_WARN, D_ERR, D_SCRIPT_ERR, D_SCRIPT_ACT};

void SetupDebug();
void DebugLog(int type, const char *format, ...);
void QuitDebug();

#endif