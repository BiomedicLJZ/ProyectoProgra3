// =============================================================================
//  Generador.cpp — NO MODIFICAR
// -----------------------------------------------------------------------------
//  Simula sólo el CONJUNTO de ids vivos y sus valores (incluido el efecto de
//  ROLLBACK) para poder elegir operaciones válidas e inválidas. No contiene
//  ninguna estructura de datos del proyecto.
// =============================================================================
#include "Generador.h"

namespace {

class Rng {
public:
    explicit Rng(unsigned long long semilla) : s_(semilla) {}
    unsigned long long siguiente() {
        unsigned long long z = (s_ += 0x9E3779B97F4A7C15ULL);
        z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
        z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
        return z ^ (z >> 31);
    }
    int rango(int lo, int hi) {   // [lo, hi]
        unsigned long long ancho = static_cast<unsigned long long>(static_cast<long long>(hi) - lo + 1);
        return static_cast<int>(lo + static_cast<long long>(siguiente() % ancho));
    }
    bool prob(int porMil) { return rango(1, 1000) <= porMil; }

private:
    unsigned long long s_;
};

unsigned long long fnv1a(const char* s) {
    unsigned long long h = 1469598103934665603ULL;
    for (; *s != '\0'; ++s) {
        h ^= static_cast<unsigned char>(*s);
        h *= 1099511628211ULL;
    }
    return h;
}

struct Deshacer {
    enum Tipo { INS, DEL, UPD } tipo;
    int id;
    int valorPrevio;
};

class Simulacion {
public:
    explicit Simulacion(int maxId)
        : maxId_(maxId), vivo_(new bool[maxId + 1]), valor_(new int[maxId + 1]),
          pos_(new int[maxId + 1]), vivos_(new int[maxId + 1]), nv_(0),
          log_(nullptr), nLog_(0), capLog_(0), marcas_(new int[64]), prof_(0) {
        for (int i = 0; i <= maxId; ++i) { vivo_[i] = false; valor_[i] = 0; pos_[i] = -1; }
    }
    ~Simulacion() {
        delete[] vivo_; delete[] valor_; delete[] pos_; delete[] vivos_;
        delete[] log_; delete[] marcas_;
    }
    Simulacion(const Simulacion&) = delete;
    Simulacion& operator=(const Simulacion&) = delete;

    int  vivos() const { return nv_; }
    bool vivo(int id) const { return id >= 1 && id <= maxId_ && vivo_[id]; }
    int  valor(int id) const { return valor_[id]; }
    int  idVivo(int i) const { return vivos_[i]; }
    int  profundidad() const { return prof_; }

    void insertar(int id, int v, bool registrar = true) {
        vivo_[id] = true; valor_[id] = v; pos_[id] = nv_; vivos_[nv_++] = id;
        if (registrar && prof_ > 0) apilar(Deshacer{Deshacer::INS, id, 0});
    }
    void eliminar(int id, bool registrar = true) {
        if (registrar && prof_ > 0) apilar(Deshacer{Deshacer::DEL, id, valor_[id]});
        int p = pos_[id];
        int ultimo = vivos_[--nv_];
        vivos_[p] = ultimo; pos_[ultimo] = p;
        vivo_[id] = false; pos_[id] = -1;
    }
    void actualizar(int id, int v, bool registrar = true) {
        if (registrar && prof_ > 0) apilar(Deshacer{Deshacer::UPD, id, valor_[id]});
        valor_[id] = v;
    }
    void begin() { marcas_[prof_++] = nLog_; }
    void commit() { --prof_; if (prof_ == 0) nLog_ = 0; }
    void rollback() {
        int marca = marcas_[--prof_];
        while (nLog_ > marca) {
            Deshacer d = log_[--nLog_];
            if (d.tipo == Deshacer::INS)      eliminar(d.id, false);
            else if (d.tipo == Deshacer::DEL) insertar(d.id, d.valorPrevio, false);
            else                              actualizar(d.id, d.valorPrevio, false);
        }
    }

private:
    void apilar(const Deshacer& d) {
        if (nLog_ == capLog_) {
            int nueva = capLog_ ? capLog_ * 2 : 64;
            Deshacer* n = new Deshacer[nueva];
            for (int i = 0; i < nLog_; ++i) n[i] = log_[i];
            delete[] log_;
            log_ = n; capLog_ = nueva;
        }
        log_[nLog_++] = d;
    }

    int maxId_;
    bool* vivo_;
    int* valor_;
    int* pos_;
    int* vivos_;
    int nv_;
    Deshacer* log_;
    int nLog_, capLog_;
    int* marcas_;
    int prof_;
};

void etiqueta(Rng& r, std::ostream& os) {
    int len = r.rango(4, 8);
    for (int i = 0; i < len; ++i) os << static_cast<char>('a' + r.rango(0, 25));
}

}  // namespace

void generarCarga(const char* semilla, TamanoCarga tamano, std::ostream& os) {
    Rng r(fnv1a(semilla) ^ 0xDB2026ULL);

    int nA, nB, cadaVerif, cadaAltura, cadaCache, limiteMax, porMilTop;
    const char* nombre;
    switch (tamano) {
        case TamanoCarga::PEQUENA:
            nA = 300; nB = 2700; cadaVerif = 100; cadaAltura = 50; cadaCache = 150;
            limiteMax = 3; porMilTop = 30; nombre = "pequena"; break;
        case TamanoCarga::MEDIANA:
            nA = 5000; nB = 25000; cadaVerif = 2500; cadaAltura = 1000; cadaCache = 2000;
            limiteMax = 2; porMilTop = 5; nombre = "mediana"; break;
        default:
            nA = 100000; nB = 200000; cadaVerif = 20000; cadaAltura = 5000; cadaCache = 10000;
            limiteMax = 1; porMilTop = 1; nombre = "grande"; break;
    }

    static const int CAPACIDADES[] = {8, 12, 16, 24, 32};
    const int capacidad = CAPACIDADES[r.rango(0, 4)];
    const int patron = r.rango(0, 2);          // 0 ascendente, 1 descendente, 2 zigzag
    const int categorias = 8;
    const int span = (patron == 2) ? 10 * nA + 10 : 5 * nA + 5;
    int ancho = span * 20 / nA;
    if (ancho < 1) ancho = 1;

    Simulacion sim(nA + nB + 16);
    int siguienteId = 1;

    static const char* PATRONES[] = {"ascendente", "descendente", "zigzag"};
    os << "# MiniDB - carga " << nombre << " - semilla " << semilla << "\n"
       << "# patron de carga inicial: " << PATRONES[patron] << ", capacidad de cache: " << capacidad << "\n"
       << "CACHE CAPACIDAD " << capacidad << "\n";

    // ---------------- Fase A: carga inicial con ids consecutivos -------------
    os << "# Fase A: carga inicial\n";
    for (int i = 1; i <= nA; ++i) {
        int v;
        int j = r.rango(0, 4);
        if (patron == 0)      v = i * 5 + j;
        else if (patron == 1) v = (nA - i) * 5 + j;
        else                  v = (i % 2 == 1) ? (i / 2) * 5 + j : 2 * (5 * nA + 5) - (i / 2) * 5 - j;
        int id = siguienteId++;
        os << "INSERT " << id << ' ' << r.rango(1, categorias) << ' ' << v << ' ';
        etiqueta(r, os);
        os << '\n';
        sim.insertar(id, v);
        if (i % cadaAltura == 0) os << "ALTURA\n";
        if (i % cadaVerif == 0) os << "VERIFICAR\n";
    }
    os << "ALTURA\nVERIFICAR\n";

    // ---------------- Fase B: mezcla ----------------------------------------
    os << "# Fase B: operaciones mezcladas\n";
    const int nCalientes = 2 * capacidad;
    int calA[64], calB[64];
    for (int i = 0; i < nCalientes; ++i) {
        calA[i] = r.rango(0, span);
        calB[i] = calA[i] + r.rango(0, 2 * ancho);
    }

    int opsEnTx = 0;
    for (int op = 1; op <= nB; ++op) {
        // Control de transacciones
        if (sim.profundidad() == 0) {
            if (r.prob(10)) { os << "BEGIN\n"; sim.begin(); opsEnTx = 0; }
            else if (r.prob(2)) os << (r.rango(0, 1) ? "COMMIT\n" : "ROLLBACK\n");   // error
        } else {
            ++opsEnTx;
            if (sim.profundidad() < 3 && r.prob(30)) {
                os << "BEGIN\n"; sim.begin();
            } else if (r.prob(40) || opsEnTx > 60) {
                if (r.rango(0, 1)) { os << "COMMIT\n"; sim.commit(); }
                else               { os << "ROLLBACK\n"; sim.rollback(); }
                if (sim.profundidad() == 0) opsEnTx = 0;
            }
        }

        int t = r.rango(1, 1000);
        if (r.prob(porMilTop)) {
            os << "TOP " << r.rango(1, 20) << " CAT " << r.rango(1, categorias) << '\n';
        } else if (t <= 140) {                                   // INSERT
            if (sim.vivos() > 0 && r.prob(100)) {
                int id = sim.idVivo(r.rango(0, sim.vivos() - 1));
                os << "INSERT " << id << ' ' << r.rango(1, categorias) << ' ' << r.rango(0, span) << ' ';
                etiqueta(r, os);
                os << '\n';
            } else {
                int id = siguienteId++;
                int v = r.rango(0, span);
                os << "INSERT " << id << ' ' << r.rango(1, categorias) << ' ' << v << ' ';
                etiqueta(r, os);
                os << '\n';
                sim.insertar(id, v);
            }
        } else if (t <= 220) {                                   // DELETE
            if (sim.vivos() > 0 && !r.prob(100)) {
                int id = sim.idVivo(r.rango(0, sim.vivos() - 1));
                os << "DELETE " << id << '\n';
                sim.eliminar(id);
            } else {
                os << "DELETE " << (siguienteId + r.rango(0, 1000)) << '\n';
            }
        } else if (t <= 340) {                                   // UPDATE
            if (sim.vivos() > 0 && !r.prob(100)) {
                int id = sim.idVivo(r.rango(0, sim.vivos() - 1));
                int v = r.rango(0, span);
                os << "UPDATE " << id << ' ' << v << '\n';
                sim.actualizar(id, v);
            } else {
                os << "UPDATE " << (siguienteId + r.rango(0, 1000)) << ' ' << r.rango(0, span) << '\n';
            }
        } else if (t <= 480) {                                   // GET
            if (sim.vivos() > 0 && !r.prob(200)) os << "GET " << sim.idVivo(r.rango(0, sim.vivos() - 1)) << '\n';
            else                                 os << "GET " << r.rango(1, siguienteId + 1000) << '\n';
        } else if (t <= 700) {                                   // RANGE
            int a, b;
            if (r.prob(500)) { int h = r.rango(0, nCalientes - 1); a = calA[h]; b = calB[h]; }
            else             { a = r.rango(0, span); b = a + r.rango(0, 2 * ancho); }
            int lim = r.rango(0, limiteMax);
            os << "RANGE " << a << ' ' << b;
            if (lim > 0) os << " LIMIT " << lim;
            os << '\n';
        } else if (t <= 860) {                                   // COUNT
            int a = r.rango(0, span);
            int b = r.prob(50) ? a - r.rango(1, ancho) : a + r.rango(0, span / 2);
            os << "COUNT " << a << ' ' << b << '\n';
        } else {                                                 // KESIMO
            os << "KESIMO " << r.rango(1, sim.vivos() + 3) << '\n';
        }

        if (op % cadaAltura == 0) os << "ALTURA\n";
        if (op % cadaCache == 0)  os << "CACHE\n";
        if (op % cadaVerif == 0)  os << "VERIFICAR\n";
    }

    while (sim.profundidad() > 0) { os << "COMMIT\n"; sim.commit(); }
    os << "# Fin\nALTURA\nCACHE\nVERIFICAR\n";
}
