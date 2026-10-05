// =============================================================================
//  Cronometro.h — medición de tiempo de pared con reloj monotónico
// =============================================================================
#ifndef CRONOMETRO_H
#define CRONOMETRO_H

#include <chrono>

class Cronometro {
public:
    Cronometro();
    void   reiniciar();
    double milisegundos() const;   // transcurridos desde el último reiniciar()

private:
    std::chrono::steady_clock::time_point inicio_;
};

#endif
