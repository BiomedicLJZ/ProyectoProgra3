// =============================================================================
//  Comando.h — Lenguaje de consultas de MiniDB. NO MODIFICAR.
// -----------------------------------------------------------------------------
//  Una instrucción por línea. Las líneas vacías y las que empiezan con '#'
//  se ignoran. Palabras clave en MAYÚSCULAS. La especificación completa de
//  cada comando está en ENUNCIADO.md §2.
//
//    INSERT <id> <categoria> <valor> <etiqueta>
//    DELETE <id>
//    UPDATE <id> <valor>
//    GET <id>
//    RANGE <a> <b> [LIMIT <m>]          m en 0..10
//    COUNT <a> <b>
//    KESIMO <k>
//    TOP <k> CAT <c>                    k en 1..100
//    BEGIN | COMMIT | ROLLBACK
//    ALTURA
//    CACHE
//    CACHE CAPACIDAD <c>                c en 1..1000
//    VERIFICAR
//    STATS
// =============================================================================
#ifndef COMANDO_H
#define COMANDO_H

#include "Registro.h"

struct Comando {
    enum Tipo {
        VACIO, ERROR_SINTAXIS,
        INSERT, DELETE, UPDATE, GET,
        RANGE, COUNT, KESIMO, TOP,
        BEGIN, COMMIT, ROLLBACK,
        ALTURA, CACHE, CACHE_CAPACIDAD, VERIFICAR, STATS
    };
    Tipo     tipo;
    Registro reg;      // INSERT (completo); DELETE/GET/UPDATE usan reg.id; UPDATE usa reg.valor
    int      a, b;     // RANGE, COUNT
    int      k;        // KESIMO, TOP, CACHE CAPACIDAD
    int      limite;   // RANGE (0 si no se indica)
    int      categoria;// TOP
};

// Convierte una línea en un Comando. Nunca falla: si la línea es inválida,
// devuelve tipo ERROR_SINTAXIS.
Comando parsear(const char* linea);

#endif
