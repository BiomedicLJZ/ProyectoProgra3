// =============================================================================
//  Parser.cpp — NO MODIFICAR
// =============================================================================
#include "Comando.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>

namespace {

const int MAX_TOKENS = 8;
const int MAX_TOKEN  = 32;

struct Tokens {
    int  n;
    char t[MAX_TOKENS][MAX_TOKEN];
};

// false si hay demasiados tokens o alguno es demasiado largo.
bool tokenizar(const char* s, Tokens& out) {
    out.n = 0;
    while (*s != '\0') {
        while (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n') ++s;
        if (*s == '\0') break;
        if (out.n == MAX_TOKENS) return false;
        int len = 0;
        while (*s != '\0' && *s != ' ' && *s != '\t' && *s != '\r' && *s != '\n') {
            if (len == MAX_TOKEN - 1) return false;
            out.t[out.n][len++] = *s++;
        }
        out.t[out.n][len] = '\0';
        ++out.n;
    }
    return true;
}

bool entero(const char* s, long long lo, long long hi, int& out) {
    if (*s == '\0') return false;
    errno = 0;
    char* fin = nullptr;
    long long v = std::strtoll(s, &fin, 10);
    if (errno != 0 || *fin != '\0' || v < lo || v > hi) return false;
    out = static_cast<int>(v);
    return true;
}

bool etiquetaValida(const char* s) {
    int len = static_cast<int>(std::strlen(s));
    if (len < 1 || len > 24) return false;
    for (int i = 0; i < len; ++i) {
        char c = s[i];
        bool ok = (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_';
        if (!ok) return false;
    }
    return true;
}

bool es(const char* a, const char* b) { return std::strcmp(a, b) == 0; }

const long long ID_MAX  = 2147483647LL;
const long long VAL_MAX = 1000000000LL;

}  // namespace

Comando parsear(const char* linea) {
    Comando c;
    std::memset(&c, 0, sizeof(c));
    c.tipo = Comando::ERROR_SINTAXIS;

    const char* p = linea;
    while (*p == ' ' || *p == '\t') ++p;
    if (*p == '#' || *p == '\0' || *p == '\r' || *p == '\n') { c.tipo = Comando::VACIO; return c; }

    Tokens tk;
    if (!tokenizar(linea, tk)) return c;
    if (tk.n == 0) { c.tipo = Comando::VACIO; return c; }

    const char* op = tk.t[0];
    bool ok = false;

    if (es(op, "INSERT") && tk.n == 5) {
        ok = entero(tk.t[1], 1, ID_MAX, c.reg.id) &&
             entero(tk.t[2], 1, 99, c.reg.categoria) &&
             entero(tk.t[3], -VAL_MAX, VAL_MAX, c.reg.valor) &&
             etiquetaValida(tk.t[4]);
        if (ok) { std::strcpy(c.reg.etiqueta, tk.t[4]); c.tipo = Comando::INSERT; }
    } else if (es(op, "DELETE") && tk.n == 2) {
        ok = entero(tk.t[1], 1, ID_MAX, c.reg.id);
        if (ok) c.tipo = Comando::DELETE;
    } else if (es(op, "UPDATE") && tk.n == 3) {
        ok = entero(tk.t[1], 1, ID_MAX, c.reg.id) && entero(tk.t[2], -VAL_MAX, VAL_MAX, c.reg.valor);
        if (ok) c.tipo = Comando::UPDATE;
    } else if (es(op, "GET") && tk.n == 2) {
        ok = entero(tk.t[1], 1, ID_MAX, c.reg.id);
        if (ok) c.tipo = Comando::GET;
    } else if (es(op, "RANGE") && (tk.n == 3 || tk.n == 5)) {
        ok = entero(tk.t[1], -VAL_MAX, VAL_MAX, c.a) && entero(tk.t[2], -VAL_MAX, VAL_MAX, c.b);
        if (ok && tk.n == 5) ok = es(tk.t[3], "LIMIT") && entero(tk.t[4], 0, ResultadoRango::MAX_FILAS, c.limite);
        if (ok) c.tipo = Comando::RANGE;
    } else if (es(op, "COUNT") && tk.n == 3) {
        ok = entero(tk.t[1], -VAL_MAX, VAL_MAX, c.a) && entero(tk.t[2], -VAL_MAX, VAL_MAX, c.b);
        if (ok) c.tipo = Comando::COUNT;
    } else if (es(op, "KESIMO") && tk.n == 2) {
        ok = entero(tk.t[1], 1, ID_MAX, c.k);
        if (ok) c.tipo = Comando::KESIMO;
    } else if (es(op, "TOP") && tk.n == 4) {
        ok = entero(tk.t[1], 1, 100, c.k) && es(tk.t[2], "CAT") && entero(tk.t[3], 1, 99, c.categoria);
        if (ok) c.tipo = Comando::TOP;
    } else if (es(op, "CACHE") && tk.n == 1) {
        c.tipo = Comando::CACHE;
    } else if (es(op, "CACHE") && tk.n == 3) {
        ok = es(tk.t[1], "CAPACIDAD") && entero(tk.t[2], 1, 1000, c.k);
        if (ok) c.tipo = Comando::CACHE_CAPACIDAD;
    } else if (tk.n == 1) {
        if      (es(op, "BEGIN"))     c.tipo = Comando::BEGIN;
        else if (es(op, "COMMIT"))    c.tipo = Comando::COMMIT;
        else if (es(op, "ROLLBACK"))  c.tipo = Comando::ROLLBACK;
        else if (es(op, "ALTURA"))    c.tipo = Comando::ALTURA;
        else if (es(op, "VERIFICAR")) c.tipo = Comando::VERIFICAR;
        else if (es(op, "STATS"))     c.tipo = Comando::STATS;
    }
    return c;
}
