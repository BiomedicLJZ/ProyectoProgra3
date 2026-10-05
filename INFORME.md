# Informe — MiniDB — <matrícula>

## 1. Plataforma
Sistema operativo / compilador / arquitectura:

## 2. Arquitectura
<!-- Diagrama de estructuras y referencias entre ellas. ¿Quién es dueño de cada Registro y quién lo libera? -->

## 3. Decisiones de diseño
### 3.1 Tabla hash (índice primario)
- Contexto:
- Alternativas consideradas:
- Decisión:
- Consecuencias (incluida la política de rehash):

### 3.2 Índice ordenado (AVL aumentado)
### 3.3 Montículo para TOP
### 3.4 Transacciones (pila de deshacer)
### 3.5 Caché LRU

## 4. Complejidades de mi implementación
| Operación | Complejidad | Justificación |
|---|---|---|
| GET | | |
| INSERT | | |
| DELETE | | |
| UPDATE | | |
| RANGE (miss / hit) | | |
| COUNT | | |
| KESIMO | | |
| TOP | | |
| BEGIN / COMMIT / ROLLBACK | | |
| VERIFICAR | | |

## 5. Experimentos
### E1. ¿Vale la pena balancear?
- Hipótesis:
- Método:
- Resultados:
- Interpretación:

### E2. Aumentar el árbol
### E3. Top-k
### E4. El caché
### E5. Mi tabla hash
### E6. El costo de deshacer

## 6. Lo que no funcionó
1.
2.
3.
