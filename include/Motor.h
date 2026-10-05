// =============================================================================
//  Motor.h — El motor de MiniDB. ESTE ES TU PROYECTO.
// -----------------------------------------------------------------------------
//  La INTERFAZ PÚBLICA es fija: Ejecutor.cpp la usa y la calificación
//  automática depende de ella. No cambies ni agregues métodos públicos.
//
//  La sección PRIVADA es tuya: agrega los miembros que necesites. Cada
//  estructura de datos debe vivir en su propio par .h/.cpp dentro de
//  include/ y src/ (CMake y el Makefile los detectan solos).
//
//  Semántica exacta de cada operación: ENUNCIADO.md §2.
//  Complejidad exigida de cada operación: ENUNCIADO.md §3.
// =============================================================================
#ifndef MOTOR_H
#define MOTOR_H

#include <iostream>

#include "Registro.h"

class Motor {
public:
    Motor();
    ~Motor();
    Motor(const Motor&) = delete;
    Motor& operator=(const Motor&) = delete;

    // --- Escrituras (false = error; no modifican nada si fallan) ------------
    bool insertar(const Registro& r);           // false si el id ya existe
    bool eliminar(int id);                      // false si el id no existe
    bool actualizar(int id, int nuevoValor);    // false si el id no existe

    // --- Lecturas ------------------------------------------------------------
    bool obtener(int id, Registro& salida);
    void rango(int a, int b, int limite, ResultadoRango& salida);
    int  contar(int a, int b);
    bool kesimo(int k, Registro& salida);       // k-ésimo menor por (valor, id), 1-indexado
    int  top(int k, int categoria, Registro* salida);   // devuelve cuántas filas escribió

    // --- Transacciones anidadas --------------------------------------------
    void iniciarTransaccion();
    bool confirmar();                           // false si no hay transacción abierta
    bool revertir();                            // false si no hay transacción abierta

    // --- Diagnóstico ---------------------------------------------------------
    int  altura() const;                        // altura del índice ordenado (vacío = -1)
    void configurarCache(int capacidad);
    EstadisticasCache estadisticasCache() const;

    // Revisa TODOS los invariantes de TODAS tus estructuras. Si alguno falla,
    // escribe una descripción en 'mensaje' (máx. 'capacidad' bytes con el
    // '\0') y devuelve false.
    bool verificar(char* mensaje, int capacidad);

    // Formato libre: métricas internas para tus experimentos (sondas,
    // rotaciones, factor de carga...). No se califica automáticamente.
    void imprimirStats(std::ostream& os);

private:
    // TODO: tus estructuras
};

#endif
