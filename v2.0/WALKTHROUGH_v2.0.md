# Walkthrough — v2.0

Esta versión parte de una auditoría de vulnerabilidades lógicas sobre `v1.0` y avanza
en tres fases: parches de negocio + configuración inyectada, y persistencia CSV
bidireccional con un parser robusto. Este documento explica qué cambió, por qué, y
cómo está organizado el código.

## 1. Punto de partida: hallazgos de la auditoría

Sobre `v1.0` se identificaron dos fallas de negocio explícitas y una serie de
hallazgos adicionales:

- **Eliminación de estudiante sin verificar solicitudes pendientes.** Se podía borrar
  un `Estudiante` aunque tuviera una `Solicitud` esperando en la cola de prioridad,
  dejando esa solicitud huérfana (referenciando un código que ya no existe).
- **Eliminación de recurso en estado `Ocupado`.** Se podía borrar un `Recurso`
  reservado por una solicitud activa, dejando el `codigoRecurso` de esa solicitud
  apuntando a algo que dejó de existir.
- **`estudiantes`/`cursos` en `std::unordered_map`**, violando la restricción de no
  usar STL en las colecciones principales de datos.
- Validación de campos vacíos incompleta (`escuela`, `tipo`, `descripcion` no se
  chequeaban).
- Sin control de límites configurables (prioridad máxima hardcodeada en el código).
- Sin persistencia: todo vivía y moría en memoria.

Nada de fugas de memoria: los destructores de `ListaEnlazada`, `Pila` y
`ColaPrioridad` ya liberaban correctamente sus nodos/arreglos en `v1.0`.

## 2. Refactor de colecciones y parches de negocio

`estudiantes` y `cursos` migraron de `std::unordered_map` a `ListaEnlazada<T>` propia
(búsqueda/eliminación O(n) por predicado — costo aceptado a cambio de cumplir la
restricción de no-STL). Para eso se agregaron dos métodos genéricos reutilizables en
el resto del proyecto:

- `ListaEnlazada<T>::recorrer(Accion)` — itera aplicando una función a cada elemento
  sin exponer `Nodo<T>*` al exterior. Se usa para listar y, más adelante, para
  exportar a CSV.
- `ColaPrioridad<T>::existe(Predicado)` — recorre el arreglo interno del heap en O(n)
  evaluando un predicado, **sin desencolar nada**. Es lo que permite responder
  "¿este estudiante tiene una solicitud pendiente?" sin destruir la estructura.
  Se agregó el equivalente en `Cola<T>` por simetría de API.

Con esas dos piezas, los parches quedaron así:

```cpp
// eliminarEstudiante: bloquea si hay una Solicitud pendiente para ese codigo
bool tieneSolicitudPendiente = solicitudes.existe([&codigo](const Solicitud& s) {
    return s.getCodigoEstudiante() == codigo;
});
if (tieneSolicitudPendiente) { /* rechazar */ }
```

```cpp
// eliminarRecurso: bloquea si el estado no es "Disponible"
if (encontrado->dato.getEstado() != "Disponible") { /* rechazar */ }
```

### Configuración inyectada (`ConfigManager`)

Las reglas de negocio que antes estaban hardcodeadas (prioridad máxima, longitud
mínima de nombre, estados válidos de un recurso) ahora viven en `config.json` y se
cargan una sola vez al iniciar `main()` mediante un parser manual
(`include/Configuracion.hpp`, `src/Configuracion.cpp`) que usa únicamente
`<fstream>`, `find()` y `substr()` — sin ninguna librería de JSON de terceros. Si el
archivo no existe, `ConfigManager` cae en sus valores por defecto sin interrumpir el
arranque.

## 3. Persistencia CSV bidireccional

### El parser (`include/ParserCSV.hpp`, `src/ParserCSV.cpp`)

`splitCSV()` es un autómata de estados finitos con dos estados (`dentroComillas` /
fuera de comillas) que recorre cada registro carácter a carácter:

- Fuera de comillas, una coma delimita campo.
- Dentro de comillas, la coma es texto normal.
- `""` dentro de un campo entre comillas se interpreta como una comilla literal
  escapada (regla estándar de CSV / RFC 4180).

`escaparCampoCSV()` hace el camino inverso al guardar: envuelve en comillas dobles
solo cuando el campo contiene coma, comilla o salto de línea, duplicando las comillas
internas.

Para el caso de campos con saltos de línea embebidos, `leerRegistroCSV()` cuenta
comillas sin cerrar en la línea leída y, si el conteo es impar, sigue concatenando
líneas físicas del archivo (con `\n`) hasta que el registro queda balanceado — mismo
patrón que ya usaba el parser de `config.json` para arreglos multilínea.

### Loaders y writers por entidad

| Entidad     | Archivo                  | Campos                                              |
|--------------|---------------------------|------------------------------------------------------|
| Estudiante   | `data/estudiantes.csv`   | `codigo,nombre,escuela,semestre`                     |
| Curso        | `data/cursos.csv`        | `codigo,nombre,creditos`                              |
| Recurso      | `data/recursos.csv`      | `codigo,tipo,descripcion,estado`                      |
| Solicitud    | `data/solicitudes.csv`   | `numero,codigoEstudiante,tipo,prioridad,codigoRecurso` |
| Historial    | `data/historial.csv`     | `accion`                                              |

Un detalle particular es el historial: `Pila<T>` solo puede recorrerse de tope a
base (es lo único que permite su estructura enlazada simple sin desapilar). Para que,
al recargar con `apilar()`, el tope original vuelva a quedar en el tope, `guardarHistorial`
primero invierte el orden con una `ListaEnlazada<std::string>` auxiliar (cada
`insertarInicio` invierte una posición) y recién entonces escribe el archivo de base a
tope. Al recargar, se lee el archivo en ese mismo orden y se apila tal cual, quedando
todo reconstruido correctamente.

Para `Solicitud` no hace falta preservar orden alguno: al recargar con `insertar()`,
el Max-Heap se reconstruye solo, sin importar en qué secuencia entren los elementos.
Lo único que se recalcula tras la carga es `contadorSiguiente = max(numero) + 1`, para
que el próximo número de solicitud no colisione con los ya persistidos.

Los *loaders* no fallan si el `.csv` correspondiente no existe (primera ejecución del
programa): lo informan por `stderr` y la estructura arranca vacía. Los *writers*
avisan explícitamente si la carpeta destino no existe, en vez de fallar en silencio.

## 4. Estructura de archivos (`v2.0/`)

```
v2.0/
├── main.cpp                  # Menu principal, orquesta todos los modulos
├── config.json                # Reglas de negocio y rutas de archivo
├── data/                      # CSVs de persistencia (se generan en runtime)
├── include/
│   ├── Entidades.hpp          # Estudiante, Curso, Recurso, Solicitud
│   ├── ListaEnlazada.hpp
│   ├── Pila.hpp
│   ├── Cola.hpp
│   ├── ColaPrioridad.hpp
│   ├── Configuracion.hpp      # ConfigManager + parser de config.json
│   └── ParserCSV.hpp          # splitCSV, escaparCampoCSV, loaders/writers
└── src/
    ├── Entidades.cpp
    ├── ListaEnlazada.cpp
    ├── Pila.cpp
    ├── Cola.cpp
    ├── ColaPrioridad.cpp
    ├── Configuracion.cpp
    └── ParserCSV.cpp
```

## 5. Compilar, ejecutar y probar

```bash
cd v2.0
g++ -std=c++11 -Wall -Wextra -o sistema main.cpp src/Entidades.cpp src/Configuracion.cpp src/ParserCSV.cpp
./sistema
```

Flujo típico de verificación manual:

1. Registrar un estudiante y un recurso.
2. Registrar una solicitud de reserva sobre ese recurso (queda `Ocupado`).
3. Intentar eliminar el estudiante o el recurso → ambos deben rechazarse.
4. Salir con `0` → se generan/actualizan los `.csv` en `data/`.
5. Volver a abrir el sistema → todo debe recargarse exactamente igual, incluyendo
   campos con comas o comillas.

## 6. Limitaciones conocidas / trabajo futuro

- "Deshacer" en el historial (Pila) hoy solo retira el registro textual de la
  acción; no revierte el efecto real sobre estudiantes, cursos, recursos o
  solicitudes. Revertir el efecto real requeriría guardar el estado anterior junto
  con cada acción (patrón *memento*).
- `Cola<T>` (FIFO) está implementada y expone `existe()`/`mostrar()` por simetría con
  `ColaPrioridad<T>`, pero `main.cpp` no la instancia activamente: todas las
  solicitudes (regulares y prioritarias) se despachan hoy desde un único Max-Heap.
- La búsqueda en `ListaEnlazada<T>` es O(n). Para los volúmenes de datos de un
  sistema académico esto es aceptable; si el dataset creciera significativamente,
  el siguiente paso natural sería una tabla hash propia (con manejo de colisiones)
  en vez de la lista enlazada.
