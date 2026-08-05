# Sistema de Gestión Académica — Estructuras de Datos en C++

Sistema de consola en C++ para la gestión de estudiantes, cursos, recursos académicos
(laboratorios, equipos, libros) y solicitudes/incidencias, implementado desde cero sobre
estructuras de datos dinámicas propias (listas enlazadas, pilas, colas y colas de
prioridad basadas en heap), sin STL en las colecciones principales, sin bases de datos
y sin librerías externas de terceros.

## Estructura del repositorio

```
.
├── v1.0/   # Version inicial: estructuras basicas + logica de negocio en memoria
└── v2.0/   # Version auditada, parcheada y con persistencia CSV bidireccional
```

`v1.0/` se conserva como snapshot de referencia — es el punto de partida sobre el que
se hizo la auditoría de vulnerabilidades lógicas que derivó en `v2.0/`. El desarrollo
activo continúa exclusivamente en `v2.0/`.

## Restricciones de diseño

- **Sin STL en las colecciones principales**: nada de `std::vector`, `std::list`,
  `std::stack`, `std::unordered_map`, etc. para almacenar datos de dominio. Toda
  colección (`ListaEnlazada<T>`, `Pila<T>`, `Cola<T>`, `ColaPrioridad<T>`) está
  implementada con nodos y punteros propios.
- **Sin bases de datos ni frameworks externos.**
- **Persistencia y configuración solo con librería estándar**: `<fstream>`,
  `<string>`, `<iostream>`. El parser de `config.json` y el parser de CSV son
  autómatas manuales, no usan ninguna librería de parseo.

## Estructuras implementadas

| Estructura          | Uso en el sistema                                              |
|----------------------|------------------------------------------------------------------|
| `ListaEnlazada<T>`  | Estudiantes, Cursos, Recursos Académicos                        |
| `Pila<T>`            | Historial de acciones (LIFO)                                     |
| `Cola<T>`            | Disponible para uso FIFO (no instanciada activamente en `main`) |
| `ColaPrioridad<T>`  | Solicitudes/incidencias, despachadas por prioridad (Max-Heap)   |

## Compilar y ejecutar (v2.0)

```bash
cd v2.0
g++ -std=c++11 -Wall -Wextra -o sistema main.cpp src/Entidades.cpp src/Configuracion.cpp src/ParserCSV.cpp
./sistema
```

El sistema lee `config.json` (reglas de negocio y rutas de archivo) y persiste todo en
`data/*.csv` al cerrar. Ver `v2.0/WALKTHROUGH.md` para el detalle completo de la
arquitectura, las vulnerabilidades corregidas y las decisiones de diseño.

## Autor

Rodrigo — Ingeniería Informática y de Sistemas, UNSAAC (código 220551).
