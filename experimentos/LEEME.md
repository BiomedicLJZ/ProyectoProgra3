# experimentos/

Aquí van los programas de tus experimentos (ENUNCIADO §5). Cualquier `.cpp`
que pongas aquí se compila junto con `src/` (sin `src/main.cpp`) en un
ejecutable llamado `experimentos`. Tú escribes su `main`.

Ejemplo mínimo, `experimentos/exp_main.cpp`:

```cpp
#include <iostream>
#include "Cronometro.h"
#include "Motor.h"

int main() {
    Motor m;
    Cronometro c;
    // ... tu experimento ...
    std::cout << "ms=" << c.milisegundos() << "\n";
}
```

En CLion, después de agregar el primer `.cpp` aquí: *File → Reload CMake
Project*, y aparecerá la configuración `experimentos` en el selector.
