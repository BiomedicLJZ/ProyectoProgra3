// =============================================================================
//  Ejecutor.h — Ejecuta comandos sobre el Motor e imprime la salida en el
//  formato EXACTO contra el que se califica. NO MODIFICAR.
// =============================================================================
#ifndef EJECUTOR_H
#define EJECUTOR_H

#include <iostream>

#include "Comando.h"
#include "Motor.h"

// Ejecuta un comando e imprime su salida (nada para VACIO).
void ejecutar(Motor& m, const Comando& c, std::ostream& os);

// Ejecuta todas las líneas de 'entrada'. Devuelve el número de comandos.
long long ejecutarFlujo(Motor& m, std::istream& entrada, std::ostream& os);

#endif
