// =============================================================================
//  Generador.h — Cargas de trabajo deterministas a partir de una semilla.
//  NO MODIFICAR.
// -----------------------------------------------------------------------------
//  La semilla es tu matrícula (o cualquier texto). La misma semilla produce
//  siempre la misma carga, byte por byte, en cualquier plataforma.
//
//  Tamaños:
//    PEQUENA  ~3 000 comandos, ~300 filas. Para depurar: legible a mano.
//    MEDIANA  ~30 000 comandos, ~5 000 filas.
//    GRANDE   ~300 000 comandos, ~120 000 filas. Mide rendimiento.
//
//  Al calificar se usan, además de tus cargas, cargas generadas con semillas
//  que no conoces.
// =============================================================================
#ifndef GENERADOR_H
#define GENERADOR_H

#include <iostream>

enum class TamanoCarga { PEQUENA, MEDIANA, GRANDE };

void generarCarga(const char* semilla, TamanoCarga tamano, std::ostream& os);

#endif
