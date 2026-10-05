# Proyecto final — MiniDB: un motor de base de datos en memoria

**Modalidad:** individual · **Duración:** 4 semanas · **Entrega:** única, al final · **Lenguaje:** C++17 sin STL

Vas a construir el núcleo de un motor de base de datos: una tabla en memoria con un índice primario (hash), un índice ordenado (AVL aumentado), consultas por rango con caché LRU, consultas top-k con un montículo, y transacciones anidadas con ROLLBACK. Todo sin STL y sin fugas de memoria.

El proyecto está pensado para unas **50–60 horas** de trabajo. No es algo que se pueda hacer la última semana.

---

## 0. Reglas

**Permitido:** apuntes, libros, cualquier página web, documentación, foros (leer, no publicar el proyecto), tu código del examen, y **preguntar a cualquier profesor**.

**Prohibido:**
- Herramientas de IA generativa de cualquier tipo: ChatGPT, Claude, Gemini, Copilot, asistentes de IA del IDE (desactiva *AI Assistant* y *Full Line Code Completion* en CLion), resúmenes de IA del buscador usados como fuente.
- Recibir ayuda de otras personas que no sean profesores, o copiar código de compañeros.
- La STL y cualquier contenedor o algoritmo de la biblioteca estándar. En tus archivos sólo puedes incluir `<iostream>`, `<iomanip>`, `<fstream>`, `<cstdio>`, `<cstdlib>`, `<cstring>`, `<climits>`, `<cmath>`, `<chrono>` y `<new>`.

**Defensa final (20 min, obligatoria).** Con tu proyecto abierto en tu máquina, tendrás que:
1. explicar las partes que el profesor elija;
2. hacer **en vivo** una modificación pequeña que no conoces de antemano (por ejemplo, un comando nuevo o una regla de desempate distinta) y predecir su efecto antes de correrla.

Si no puedes explicar o modificar una parte de tu propio código, esa sección vale **0** aunque pase todas las pruebas.

**Proceso.** La entrega es única, así que la única evidencia de cómo trabajaste es tu **historial de git** y tu **bitácora** (§7). Ambos se califican y son lo primero que se revisa en la defensa.

---

## 1. Puesta en marcha

### 1.1 CLion

Es la misma configuración del examen. **File → Open** sobre la carpeta con `CMakeLists.txt` y activa los tres perfiles en *Settings → Build, Execution, Deployment → CMake*:

| Perfil | Para qué |
|---|---|
| `debug` | Trabajar y depurar |
| `asan` | Detectar errores de memoria. Corre aquí las cargas `pequena` y `mediana` |
| `exp` | -O2. Corre aquí la carga `grande` y tus experimentos de tiempo |

| | macOS | Windows |
|---|---|---|
| Toolchain | *Default* (Apple clang) | *Visual Studio* (Build Tools 2022) |
| AddressSanitizer | Incluido | Componente **C++ AddressSanitizer** del Visual Studio Installer |
| Fugas | `leaks --atExit -- ./build/debug/minidb ejecutar carga.txt` | Perfil `debug`: la CRT imprime los bloques no liberados al terminar |

### 1.2 Uso

```
minidb generar <tu_matricula> pequena > cargas/pequena.txt
minidb generar <tu_matricula> mediana > cargas/mediana.txt
minidb generar <tu_matricula> grande  > cargas/grande.txt
minidb ejecutar cargas/pequena.txt > salidas/pequena.txt
minidb ejecutar cargas/grande.txt --tiempo > salidas/grande.txt
minidb repl                              (modo interactivo, para probar a mano)
```

En CLion los argumentos van en *Run → Edit Configurations → Program arguments*. En Windows PowerShell, redirige con `| Out-File -Encoding utf8 archivo.txt`.

### 1.3 Qué es tuyo y qué no

| Archivo | |
|---|---|
| `include/Motor.h`, `src/Motor.cpp` | **Tuyos.** La interfaz pública de `Motor` es fija; la parte privada la decides tú |
| Cualquier `.h/.cpp` nuevo en `include/` y `src/` | **Tuyos.** Cada estructura de datos va en su propio par `.h/.cpp` |
| `experimentos/*.cpp` | **Tuyos.** Se compilan en un ejecutable aparte (ver `experimentos/LEEME.md`) |
| `Registro.h`, `Comando.h`, `Parser.cpp`, `Ejecutor.*`, `Generador.*`, `Cronometro.*`, `DeteccionFugas.*`, `main.cpp`, `CMakeLists.txt`, `CMakePresets.json`, `Makefile` | **NO MODIFICAR.** Al calificar se reemplazan por los originales |

Tu código se vuelve a compilar y a ejecutar en **Linux con GCC**, así que no uses nada exclusivo de tu compilador ni de tu sistema operativo.

---

## 2. Especificación

La tabla tiene filas `(id, categoria, valor, etiqueta)` (ver `Registro.h`). `id` es la llave primaria y `valor` puede repetirse. El **orden de la tabla** es por la llave compuesta **(valor, id)**, ascendente.

Una instrucción por línea. Las líneas vacías o que empiezan con `#` no producen salida. Una línea mal formada produce `ERROR sintaxis` (eso ya lo hace el parser). Cada comando produce exactamente la salida indicada, y el formato lo imprime `Ejecutor.cpp`: tú sólo devuelves los datos.

### 2.1 Comandos

| Comando | Salida | Semántica |
|---|---|---|
| `INSERT id cat val etq` | `OK` · `ERROR duplicado` | Inserta la fila. Si el id existe, no cambia nada |
| `DELETE id` | `OK` · `ERROR no existe` | |
| `UPDATE id val` | `OK` · `ERROR no existe` | Cambia sólo `valor`. En el índice ordenado equivale a **eliminar (viejo, id) e insertar (nuevo, id)**, aunque el valor no cambie |
| `GET id` | `id cat val etq` · `NULL` | |
| `RANGE a b [LIMIT m]` | `n=<k> firma=<f>` y luego las primeras `m` filas (0 ≤ m ≤ 10), con sangría de dos espacios | Filas con `a ≤ valor ≤ b`, en orden (valor, id). Si `a > b`, el rango es vacío. `firma = Σ (i+1)·idᵢ mod 1 000 000 007`, con i desde 0 en ese orden |
| `COUNT a b` | un entero | Cuántas filas tienen `a ≤ valor ≤ b` (0 si `a > b`) |
| `KESIMO k` | fila · `NULL` | La k-ésima fila en orden (valor, id), empezando en 1 |
| `TOP k CAT c` | `top=<t>` y t filas | Las k filas de categoría `c` con **mayor** valor; en empate, **menor** id primero. `t = min(k, filas de esa categoría)` |
| `BEGIN` | `OK` | Abre una transacción (pueden anidarse) |
| `COMMIT` | `OK` · `ERROR sin transaccion` | Cierra la más interna. Sus cambios pasan a la externa, si existe. Si era la más externa, ya no se pueden deshacer |
| `ROLLBACK` | `OK` · `ERROR sin transaccion` | Deshace **todas** las escrituras exitosas de la transacción más interna (incluidas las de transacciones internas ya confirmadas) y la cierra |
| `ALTURA` | `altura=<h>` | Altura del índice ordenado (vacío = −1, un nodo = 0) |
| `CACHE` | `hits=… misses=… expulsiones=… invalidadas=… tam=… cap=…` | |
| `CACHE CAPACIDAD c` | `OK` | Vacía el caché, reinicia sus contadores y fija su capacidad. La capacidad inicial es 16 |
| `VERIFICAR` | `VERIFICAR OK` · `VERIFICAR FALLA <msg>` | Revisa todos los invariantes (§3.2) |
| `STATS` | libre | Tus métricas internas. No se califica automáticamente |

### 2.2 Ejemplo

```
CACHE CAPACIDAD 2         OK
INSERT 1 3 50 alfa        OK
INSERT 2 3 20 beta        OK
INSERT 3 1 50 gamma       OK
INSERT 4 3 35 delta       OK
INSERT 2 1 99 repetido    ERROR duplicado
GET 2                     2 3 20 beta
GET 9                     NULL
RANGE 20 50 LIMIT 3       n=4 firma=25          (1·2 + 2·4 + 3·1 + 4·3)
                            2 3 20 beta
                            4 3 35 delta
                            1 3 50 alfa
RANGE 20 50               n=4 firma=25          (hit de caché)
COUNT 20 49               2
COUNT 60 10               0
KESIMO 2                  4 3 35 delta
KESIMO 7                  NULL
TOP 2 CAT 3               top=2
                            1 3 50 alfa
                            4 3 35 delta
ALTURA                    altura=2
BEGIN                     OK
UPDATE 2 60               OK                    (invalida [20,50]: contiene 20)
DELETE 1                  OK
BEGIN                     OK
INSERT 5 2 10 epsilon     OK
ROLLBACK                  OK                    (deshace sólo el INSERT 5)
RANGE 20 50               n=2 firma=10
COMMIT                    OK
COMMIT                    ERROR sin transaccion
CACHE                     hits=1 misses=2 expulsiones=0 invalidadas=1 tam=1 cap=2
VERIFICAR                 VERIFICAR OK
```

### 2.3 Índice ordenado: AVL con reglas exactas

`ALTURA` se compara contra la referencia, así que tu árbol debe tener **exactamente** la misma forma. Para eso, sigue estas reglas:

- Llave `(valor, id)`, comparación lexicográfica. Factor de balance `bf(n) = altura(izq) − altura(der)`.
- Después de insertar o eliminar, **cada ancestro** del punto modificado se rebalancea en el camino de regreso a la raíz:
  - `bf = +2`: si `bf(izq) ≥ 0`, rotación simple a la derecha; si no, doble (izquierda sobre el hijo, luego derecha).
  - `bf = −2`: si `bf(der) ≤ 0`, rotación simple a la izquierda; si no, doble (derecha sobre el hijo, luego izquierda).
- Eliminar un nodo con dos hijos: **se copia la llave del sucesor inorden** (mínimo del subárbol derecho) y se elimina el sucesor de ese subárbol.
- `UPDATE` = eliminar `(viejo, id)` y luego insertar `(nuevo, id)`.
- `ROLLBACK` aplica las operaciones inversas **en orden inverso** (la última escritura se deshace primero), por los mismos caminos de eliminar e insertar. Por eso la forma del árbol después de un ROLLBACK puede no ser la de antes del BEGIN, y es correcto que así sea.

### 2.4 Caché de RANGE (LRU)

- La llave es `(a, b)`. `LIMIT` no forma parte de la llave. Cada entrada guarda `n`, `firma` y los ids de las primeras 10 filas.
- Todo `RANGE` es un **hit** (la entrada pasa a ser la más reciente) o un **miss** (se calcula, se guarda como la más reciente y, si el caché ya estaba lleno, antes se **expulsa** la menos reciente).
- **Invalidación:** cada escritura **exitosa** (incluidas las que ejecuta un ROLLBACK) elimina del caché toda entrada cuyo intervalo contenga un valor afectado. El INSERT y el DELETE afectan el valor de la fila. El UPDATE afecta el valor viejo y el nuevo. Cada entrada eliminada suma 1 a `invalidadas`. Las escrituras fallidas no invalidan nada.
- En un hit, las filas que se imprimen se leen de la tabla **actual** usando los ids guardados.

---

## 3. Requisitos de implementación

### 3.1 Estructuras y complejidad exigidas

*n* = filas, *k* = filas del resultado, *C* = capacidad del caché, *t* = escrituras en la transacción. "Esperado" = promedio con una función hash razonable.

| Operación | Estructura | Complejidad |
|---|---|---|
| `GET`, búsqueda por id dentro de `INSERT/DELETE/UPDATE` | **Tabla hash de diseño propio**. Tú eliges encadenamiento o direccionamiento abierto y lo justificas | O(1) esperado |
| Mantener el orden en `INSERT/DELETE/UPDATE` | **AVL** (§2.3) | O(log n) |
| `RANGE` (miss) | AVL | O(log n + k) |
| `COUNT`, `KESIMO` | AVL **aumentado con el tamaño de cada subárbol** | **O(log n)**, sin recorrer las filas |
| `TOP k CAT c` | **Montículo binario acotado a k** | O(n log k) tiempo, **O(k) memoria extra**. Ordenar todas las filas está prohibido |
| `BEGIN`, `COMMIT` | **Pila** de deshacer con marcas | O(1) amortizado |
| `ROLLBACK` | La misma pila | O(t · log n) |
| `RANGE` (hit), expulsión | **Caché LRU: tabla hash + lista doblemente enlazada** | O(1) esperado |
| Invalidación | Caché | O(C) por escritura |

Además:

- **Cero fugas y cero errores de memoria** en todas las cargas, con ASan y con la herramienta de fugas de tu plataforma.
- Cada clase que maneja memoria cumple la **regla de los tres** o elimina explícitamente la copia (`= delete`).
- Tu tabla hash **no puede crecer sin límite** cuando las filas vivas se mantienen constantes (recuerda B.4 del examen). Explica tu política de rehash en el informe.

### 3.2 `VERIFICAR`

Debe revisar de verdad, en O(n), al menos:
- **AVL:** orden (valor, id) estricto; alturas y tamaños guardados correctos; |bf| ≤ 1 en cada nodo.
- **Hash:** conteo de elementos correcto, y cada elemento alcanzable desde su posición base.
- **Consistencia cruzada:** mismo número de filas en ambos índices, y cada llave del AVL corresponde a una fila de la tabla con ese valor.
- **Caché:** la lista y la tabla del caché tienen el mismo tamaño, que es ≤ C, y los enlaces de la lista son consistentes en ambos sentidos.

En la defensa se corromperá a propósito una de tus estructuras para comprobar que `VERIFICAR` lo detecta.

---

## 4. Calificación automática

Se usan **tus** tres cargas y **tres cargas ocultas** (una de cada tamaño), generadas con semillas que no conoces. Cada salida se compara **byte por byte** contra la de la implementación de referencia.

- `pequena` y `mediana` se corren con **ASan + LeakSanitizer en Linux**. Cualquier reporte cuenta como fallo de memoria.
- `grande` se corre con -O2 y tiene **límite de tiempo**: 8 veces lo que tarda la referencia en la misma máquina, con un mínimo de 3 s. Como guía: en una laptop actual, con el perfil `exp`, una buena implementación corre la carga grande en 1–3 s. Si la tuya tarda más de 10 s, algo no cumple la tabla de §3.1.

Las cargas incluyen a propósito una **carga inicial con ids consecutivos y valores monótonos o en zigzag**, que degenera cualquier árbol sin balanceo, y muchos `COUNT` sobre rangos amplios.

---

## 5. Experimentos (en `INFORME.md`)

Para cada experimento: hipótesis, método (qué mediste, cuántas repeticiones, con qué perfil), resultados (tabla o gráfica con **tus** números) e interpretación. Los experimentos van en `experimentos/`.

- **E1. ¿Vale la pena balancear?** Compara tu AVL contra un ABB sin balanceo (puedes adaptar el de tu examen) con inserción en orden, en zigzag y aleatoria, para n = 10³ … 10⁵. Reporta altura, tiempo de inserción y tiempo de búsqueda. ¿A partir de qué n deja de ser usable el ABB, y por qué justo ahí?
- **E2. Aumentar el árbol.** Compara `COUNT` con tamaños de subárbol (O(log n)) contra una versión que recorre el rango (O(log n + k)). ¿Para qué ancho de rango se cruzan? ¿Qué cuesta mantener los tamaños en cada rotación?
- **E3. Top-k.** Compara tu montículo acotado contra ordenar todas las filas de la categoría (implementa un ordenamiento O(n log n) propio sólo para el experimento), con k = 1, 10, 100, 1000 y n = 10⁵. ¿Cuándo deja de convenir el montículo?
- **E4. El caché.** Con tu carga `mediana`, varía la capacidad (cambia la línea `CACHE CAPACIDAD` del archivo) entre 1 y 256 y grafica la tasa de hits. Explica la forma de la curva a partir de cómo el generador elige los rangos (lee `Generador.cpp`). ¿Por qué LRU se comporta tan mal cuando la capacidad es menor que el número de rangos "calientes"? Propón y **mide** una política alternativa (en `experimentos/`, sin cambiar el comportamiento calificado).
- **E5. Tu tabla hash.** Mide sondas (o longitud de cadena) promedio contra factor de carga, y el efecto de una carga sostenida de altas y bajas. Justifica tu diseño con estos datos.
- **E6. El costo de deshacer.** Mide el tiempo de `ROLLBACK` contra el tamaño de la transacción. ¿Qué memoria ocupa la pila de deshacer en el peor caso de tus cargas?

---

## 6. `INFORME.md`

1. **Plataforma:** sistema operativo, compilador y arquitectura.
2. **Arquitectura:** diagrama (puede ser ASCII) de las estructuras y de cómo se referencian entre sí. ¿Quién es dueño de cada `Registro` y quién lo libera?
3. **Decisiones de diseño:** una por estructura, con contexto, alternativas consideradas, decisión y consecuencias.
4. **Tabla de complejidades** de tu implementación (no la del enunciado), con una justificación de una línea por operación.
5. **Experimentos E1–E6.**
6. **Lo que no funcionó:** al menos tres intentos fallidos o errores difíciles, con cómo los encontraste y cómo los resolviste.

---

## 7. Proceso: bitácora y git

- **Git desde el día 1.** Entrega el repositorio completo, con la carpeta `.git`. Se espera un historial que muestre trabajo repartido en las cuatro semanas: al menos **25 commits** en al menos **3 semanas distintas**, cada uno con un mensaje que diga qué cambió. Un historial de 3 commits el último día vale 0 en este rubro.
- **`BITACORA.md`:** una entrada por sesión de trabajo con fecha y horas, qué hiciste, qué falló y qué consultaste (URL exacta, libro y página, o nombre del profesor).

---

## 8. Calendario sugerido (no se califica)

| Semana | Meta | Prueba de que vas bien |
|---|---|---|
| 1 | Tabla hash propia + `INSERT/GET/DELETE` + `VERIFICAR` del hash. Empieza la bitácora y git | Los `GET` de la carga `pequena` salen bien |
| 2 | AVL con tamaños: `RANGE`, `COUNT`, `KESIMO`, `ALTURA`, `UPDATE`. Es la parte más difícil: no la dejes para después | `pequena` sin `TOP`, caché ni transacciones coincide en lo que sí está implementado |
| 3 | Montículo `TOP`, transacciones anidadas, caché LRU con invalidación | `pequena` y `mediana` completas, sin errores de ASan |
| 4 | Rendimiento de `grande`, experimentos, informe | `grande` en < 3 s con el perfil `exp` |

---

## 9. Entrega

Un `.zip` llamado `<matricula>.zip` con el repositorio completo:

```
<matricula>/
├── .git/
├── CMakeLists.txt, CMakePresets.json, Makefile
├── include/, src/            (tu motor)
├── experimentos/             (tus experimentos)
├── cargas/                   (pequena.txt, mediana.txt; NO incluyas grande.txt)
├── salidas/                  (pequena.txt, mediana.txt, grande.txt)
├── INFORME.md
└── BITACORA.md
```

**No incluyas `build/` ni ejecutables.**

Antes de entregar:
- [ ] Los perfiles `debug`, `asan` y `exp` compilan sin advertencias
- [ ] `pequena` y `mediana` sin reportes de ASan y sin fugas
- [ ] `grande` en menos de ~5 s con `exp` en tu máquina
- [ ] Ningún archivo tuyo incluye encabezados fuera de la lista permitida
- [ ] `git log --oneline | wc -l` ≥ 25
- [ ] Puedes explicar cada línea que escribiste
