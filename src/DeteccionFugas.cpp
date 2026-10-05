// =============================================================================
//  DeteccionFugas.cpp — NO MODIFICAR
// =============================================================================
#include "DeteccionFugas.h"

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(__SANITIZE_ADDRESS__)
#include <crtdbg.h>

void activarDeteccionFugas() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
}
#else
void activarDeteccionFugas() {}
#endif
