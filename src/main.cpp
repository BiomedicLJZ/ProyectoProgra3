// =============================================================================
//  main.cpp — NO MODIFICAR
//
//  minidb generar <semilla> {pequena|mediana|grande}   escribe la carga en stdout
//  minidb ejecutar <archivo> [--tiempo]                corre una carga
//  minidb repl                                         modo interactivo
//
//  En CLion: Run > Edit Configurations > Program arguments. Para guardar la
//  salida en un archivo, marca "Save console output to file" en la misma
//  ventana o usa la terminal integrada.
// =============================================================================
#include <cstring>
#include <fstream>
#include <iostream>

#include "Cronometro.h"
#include "DeteccionFugas.h"
#include "Ejecutor.h"
#include "Generador.h"
#include "Motor.h"

namespace {

void uso(const char* p) {
    std::cerr << "Uso:\n"
              << "  " << p << " generar <semilla> {pequena|mediana|grande}\n"
              << "  " << p << " ejecutar <archivo> [--tiempo]\n"
              << "  " << p << " repl\n";
}

}  // namespace

int main(int argc, char** argv) {
    activarDeteccionFugas();
    std::ios::sync_with_stdio(false);

    if (argc >= 4 && std::strcmp(argv[1], "generar") == 0) {
        TamanoCarga t;
        if      (std::strcmp(argv[3], "pequena") == 0) t = TamanoCarga::PEQUENA;
        else if (std::strcmp(argv[3], "mediana") == 0) t = TamanoCarga::MEDIANA;
        else if (std::strcmp(argv[3], "grande") == 0)  t = TamanoCarga::GRANDE;
        else { uso(argv[0]); return 1; }
        generarCarga(argv[2], t, std::cout);
        std::cout.flush();
        return 0;
    }

    if (argc >= 3 && std::strcmp(argv[1], "ejecutar") == 0) {
        std::ifstream f(argv[2]);
        if (!f) { std::cerr << "No se pudo abrir " << argv[2] << "\n"; return 1; }
        bool tiempo = (argc >= 4 && std::strcmp(argv[3], "--tiempo") == 0);
        Cronometro c;
        long long n;
        {
            Motor m;
            n = ejecutarFlujo(m, f, std::cout);
        }   // el destructor del Motor entra en la medición
        if (tiempo) std::cerr << "comandos=" << n << " ms=" << c.milisegundos() << "\n";
        return 0;
    }

    if (argc >= 2 && std::strcmp(argv[1], "repl") == 0) {
        std::ios::sync_with_stdio(true);
        Motor m;
        char linea[512];
        std::cout << "MiniDB. Ctrl+D (macOS) o Ctrl+Z Enter (Windows) para salir.\n> " << std::flush;
        while (std::cin.getline(linea, sizeof(linea))) {
            ejecutar(m, parsear(linea), std::cout);
            std::cout << "> " << std::flush;
        }
        return 0;
    }

    uso(argv[0]);
    return 1;
}
