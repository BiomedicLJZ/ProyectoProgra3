// =============================================================================
//  Registro.h — Una fila de la tabla. NO MODIFICAR.
// -----------------------------------------------------------------------------
//  Restricciones (el parser ya las valida):
//    id         1 .. 2 147 483 647      (llave primaria, única)
//    categoria  1 .. 99
//    valor      -1 000 000 000 .. 1 000 000 000   (puede repetirse)
//    etiqueta   1..24 caracteres [a-z0-9_]
// =============================================================================
#ifndef REGISTRO_H
#define REGISTRO_H

struct Registro {
    int  id;
    int  categoria;
    int  valor;
    char etiqueta[25];
};

// Resultado de RANGE. 'firma' resume TODAS las filas del rango en orden:
//   firma = ( sum_{i=0}^{n-1} (i+1) * id_i )  mod 1 000 000 007
// donde id_i es el id de la i-ésima fila en orden (valor asc, id asc).
// 'filas' contiene las primeras min(limite, n) filas.
struct ResultadoRango {
    static const int MAX_FILAS = 10;
    int       n;
    long long firma;
    int       nFilas;
    Registro  filas[MAX_FILAS];
};

struct EstadisticasCache {
    long long hits;
    long long misses;
    long long expulsiones;
    long long invalidadas;
    int       tamano;
    int       capacidad;
};

#endif
