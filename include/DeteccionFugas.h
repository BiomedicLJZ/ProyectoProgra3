// =============================================================================
//  DeteccionFugas.h — NO MODIFICAR
// -----------------------------------------------------------------------------
//  Windows + MSVC, perfil Debug SIN ASan: activa el reporte de fugas del heap
//  de depuración de la CRT. Al terminar el programa, cada bloque no liberado
//  se imprime en stderr como:
//      {123} normal block at 0x000001F2..., 24 bytes long.
//  En cualquier otra configuración no hace nada:
//    Linux  -> LeakSanitizer ya viene incluido en el perfil asan.
//    macOS  -> usa:  leaks --atExit -- ./examen <mat> lista   (perfil debug)
// =============================================================================
#ifndef DETECCION_FUGAS_H
#define DETECCION_FUGAS_H

void activarDeteccionFugas();

#endif
