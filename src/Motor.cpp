// =============================================================================
//  Motor.cpp — ESTE ES TU PROYECTO. Todo lo de aquí es un esqueleto.
// =============================================================================
#include "Motor.h"

#include <cstring>

Motor::Motor() {}
Motor::~Motor() {}

bool Motor::insertar(const Registro& r) { (void)r; return false; }
bool Motor::eliminar(int id) { (void)id; return false; }
bool Motor::actualizar(int id, int nuevoValor) { (void)id; (void)nuevoValor; return false; }

bool Motor::obtener(int id, Registro& salida) { (void)id; (void)salida; return false; }

void Motor::rango(int a, int b, int limite, ResultadoRango& salida) {
    (void)a; (void)b; (void)limite;
    salida.n = 0;
    salida.firma = 0;
    salida.nFilas = 0;
}

int  Motor::contar(int a, int b) { (void)a; (void)b; return 0; }
bool Motor::kesimo(int k, Registro& salida) { (void)k; (void)salida; return false; }
int  Motor::top(int k, int categoria, Registro* salida) { (void)k; (void)categoria; (void)salida; return 0; }

void Motor::iniciarTransaccion() {}
bool Motor::confirmar() { return false; }
bool Motor::revertir() { return false; }

int  Motor::altura() const { return -1; }
void Motor::configurarCache(int capacidad) { (void)capacidad; }

EstadisticasCache Motor::estadisticasCache() const {
    EstadisticasCache e;
    std::memset(&e, 0, sizeof(e));
    return e;
}

bool Motor::verificar(char* mensaje, int capacidad) {
    if (capacidad > 0) mensaje[0] = '\0';
    return true;
}

void Motor::imprimirStats(std::ostream& os) { os << "(sin estadisticas)\n"; }
