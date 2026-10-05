// =============================================================================
//  Ejecutor.cpp — NO MODIFICAR
// =============================================================================
#include "Ejecutor.h"

namespace {

void fila(const Registro& r, std::ostream& os) {
    os << r.id << ' ' << r.categoria << ' ' << r.valor << ' ' << r.etiqueta << '\n';
}

}  // namespace

void ejecutar(Motor& m, const Comando& c, std::ostream& os) {
    switch (c.tipo) {
        case Comando::VACIO:
            return;
        case Comando::ERROR_SINTAXIS:
            os << "ERROR sintaxis\n";
            return;
        case Comando::INSERT:
            os << (m.insertar(c.reg) ? "OK\n" : "ERROR duplicado\n");
            return;
        case Comando::DELETE:
            os << (m.eliminar(c.reg.id) ? "OK\n" : "ERROR no existe\n");
            return;
        case Comando::UPDATE:
            os << (m.actualizar(c.reg.id, c.reg.valor) ? "OK\n" : "ERROR no existe\n");
            return;
        case Comando::GET: {
            Registro r;
            if (m.obtener(c.reg.id, r)) fila(r, os); else os << "NULL\n";
            return;
        }
        case Comando::RANGE: {
            ResultadoRango res;
            m.rango(c.a, c.b, c.limite, res);
            os << "n=" << res.n << " firma=" << res.firma << '\n';
            int filas = res.nFilas;
            if (filas > c.limite) filas = c.limite;
            if (filas > ResultadoRango::MAX_FILAS) filas = ResultadoRango::MAX_FILAS;
            for (int i = 0; i < filas; ++i) { os << "  "; fila(res.filas[i], os); }
            return;
        }
        case Comando::COUNT:
            os << m.contar(c.a, c.b) << '\n';
            return;
        case Comando::KESIMO: {
            Registro r;
            if (m.kesimo(c.k, r)) fila(r, os); else os << "NULL\n";
            return;
        }
        case Comando::TOP: {
            Registro buf[100];
            int n = m.top(c.k, c.categoria, buf);
            if (n > c.k) n = c.k;
            os << "top=" << n << '\n';
            for (int i = 0; i < n; ++i) { os << "  "; fila(buf[i], os); }
            return;
        }
        case Comando::BEGIN:
            m.iniciarTransaccion();
            os << "OK\n";
            return;
        case Comando::COMMIT:
            os << (m.confirmar() ? "OK\n" : "ERROR sin transaccion\n");
            return;
        case Comando::ROLLBACK:
            os << (m.revertir() ? "OK\n" : "ERROR sin transaccion\n");
            return;
        case Comando::ALTURA:
            os << "altura=" << m.altura() << '\n';
            return;
        case Comando::CACHE: {
            EstadisticasCache e = m.estadisticasCache();
            os << "hits=" << e.hits << " misses=" << e.misses << " expulsiones=" << e.expulsiones
               << " invalidadas=" << e.invalidadas << " tam=" << e.tamano << " cap=" << e.capacidad << '\n';
            return;
        }
        case Comando::CACHE_CAPACIDAD:
            m.configurarCache(c.k);
            os << "OK\n";
            return;
        case Comando::VERIFICAR: {
            char msg[256];
            msg[0] = '\0';
            if (m.verificar(msg, 256)) os << "VERIFICAR OK\n";
            else { msg[255] = '\0'; os << "VERIFICAR FALLA " << msg << '\n'; }
            return;
        }
        case Comando::STATS:
            m.imprimirStats(os);
            return;
    }
}

long long ejecutarFlujo(Motor& m, std::istream& entrada, std::ostream& os) {
    char linea[512];
    long long n = 0;
    while (entrada.getline(linea, sizeof(linea))) {
        Comando c = parsear(linea);
        ejecutar(m, c, os);
        if (c.tipo != Comando::VACIO) ++n;
    }
    // Línea demasiado larga: getline activa failbit sin llegar a EOF.
    if (!entrada.eof()) os << "ERROR sintaxis\n";
    os.flush();
    return n;
}
