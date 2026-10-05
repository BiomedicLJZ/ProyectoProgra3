#include "Cronometro.h"

Cronometro::Cronometro() : inicio_(std::chrono::steady_clock::now()) {}

void Cronometro::reiniciar() { inicio_ = std::chrono::steady_clock::now(); }

double Cronometro::milisegundos() const {
    std::chrono::duration<double, std::milli> d = std::chrono::steady_clock::now() - inicio_;
    return d.count();
}
