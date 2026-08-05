#include <iostream>
#include <string>
#include <limits>
#include <unordered_map>
#include <cstdlib>

#include "include/Entidades.hpp"
#include "include/ListaEnlazada.hpp"
#include "include/Pila.hpp"
#include "include/Cola.hpp"
#include "include/ColaPrioridad.hpp"

// ===========================================================================
// Sistema de Gestion Academica y Atencion de Solicitudes
// Toda la logica se mantiene en funciones libres que reciben las estructuras
// por referencia (regla del profesor Montoya: evitar variables globales).
// ===========================================================================

namespace {

void limpiarBufferEntrada() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int leerEntero(const std::string& mensaje) {
    int valor;
    std::cout << mensaje;
    while (!(std::cin >> valor)) {
        if (std::cin.eof()) {
            // Entrada agotada (ej. redireccion de archivo sin '0' final):
            // se fuerza salida del programa en vez de girar en vacio.
            std::cout << "\nEntrada agotada. Cerrando el programa.\n";
            std::exit(0);
        }
        std::cout << "Entrada invalida. Intente de nuevo: ";
        limpiarBufferEntrada();
    }
    limpiarBufferEntrada();
    return valor;
}

std::string leerLinea(const std::string& mensaje) {
    std::string valor;
    std::cout << mensaje;
    std::getline(std::cin, valor);
    return valor;
}

void registrarAccion(Pila<std::string>& historial, const std::string& accion) {
    historial.apilar(accion);
}

// ---------------------------------------------------------------------
// Modulo 1: Estudiantes
// ---------------------------------------------------------------------
void registrarEstudiante(std::unordered_map<std::string, Estudiante>& estudiantes, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo del estudiante: ");
    if (codigo.empty()) {
        std::cout << "Error: el codigo no puede estar vacio.\n";
        return;
    }
    if (estudiantes.find(codigo) != estudiantes.end()) {
        std::cout << "Error: ya existe un estudiante con ese codigo.\n";
        return;
    }
    std::string nombre = leerLinea("Nombre: ");
    if (nombre.empty()) {
        std::cout << "Error: el nombre no puede estar vacio.\n";
        return;
    }
    std::string escuela = leerLinea("Escuela profesional: ");
    std::string semestre = leerLinea("Semestre (ej. 2026-I): ");

    estudiantes[codigo] = Estudiante(codigo, nombre, escuela, semestre);
    std::cout << "Estudiante registrado.\n";
    registrarAccion(historial, "Estudiante registrado: codigo " + codigo);
}

void buscarEstudiante(const std::unordered_map<std::string, Estudiante>& estudiantes) {
    std::string codigo = leerLinea("Codigo a buscar: ");
    auto it = estudiantes.find(codigo);
    if (it == estudiantes.end()) {
        std::cout << "No se encontro ningun estudiante con ese codigo.\n";
        return;
    }
    std::cout << it->second << "\n";
}

void eliminarEstudiante(std::unordered_map<std::string, Estudiante>& estudiantes, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo a eliminar: ");
    auto it = estudiantes.find(codigo);
    if (it == estudiantes.end()) {
        std::cout << "No se encontro ningun estudiante con ese codigo.\n";
        return;
    }
    estudiantes.erase(it);
    std::cout << "Estudiante eliminado.\n";
    registrarAccion(historial, "Estudiante eliminado: codigo " + codigo);
}

void listarEstudiantes(const std::unordered_map<std::string, Estudiante>& estudiantes) {
    if (estudiantes.empty()) {
        std::cout << "(no hay estudiantes registrados)\n";
        return;
    }
    for (const auto& par : estudiantes) {
        std::cout << par.second << "\n";
    }
}

void menuEstudiantes(std::unordered_map<std::string, Estudiante>& estudiantes, Pila<std::string>& historial) {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n-- Gestion de Estudiantes --\n";
        std::cout << "1. Registrar\n2. Buscar\n3. Eliminar\n4. Listar\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: registrarEstudiante(estudiantes, historial); break;
            case 2: buscarEstudiante(estudiantes); break;
            case 3: eliminarEstudiante(estudiantes, historial); break;
            case 4: listarEstudiantes(estudiantes); break;
            case 0: break;
            default: std::cout << "Opcion invalida.\n";
        }
    }
}

// ---------------------------------------------------------------------
// Modulo 2: Cursos
// ---------------------------------------------------------------------
void registrarCurso(std::unordered_map<std::string, Curso>& cursos, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo del curso (ej. IFI03AIN): ");
    if (codigo.empty()) {
        std::cout << "Error: el codigo no puede estar vacio.\n";
        return;
    }
    if (cursos.find(codigo) != cursos.end()) {
        std::cout << "Error: ya existe un curso con ese codigo.\n";
        return;
    }
    std::string nombre = leerLinea("Nombre del curso: ");
    if (nombre.empty()) {
        std::cout << "Error: el nombre no puede estar vacio.\n";
        return;
    }
    int creditos = leerEntero("Creditos: ");
    if (creditos <= 0) {
        std::cout << "Error: los creditos deben ser un valor positivo.\n";
        return;
    }

    cursos[codigo] = Curso(codigo, nombre, creditos);
    std::cout << "Curso registrado.\n";
    registrarAccion(historial, "Curso registrado: codigo " + codigo);
}

void buscarCurso(const std::unordered_map<std::string, Curso>& cursos) {
    std::string codigo = leerLinea("Codigo a buscar: ");
    auto it = cursos.find(codigo);
    if (it == cursos.end()) {
        std::cout << "No se encontro ningun curso con ese codigo.\n";
        return;
    }
    std::cout << it->second << "\n";
}

void eliminarCurso(std::unordered_map<std::string, Curso>& cursos, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo a eliminar: ");
    auto it = cursos.find(codigo);
    
    if (it == cursos.end()) {
        std::cout << "No se encontro ningun curso con ese codigo.\n";
        return;
    }
    
    cursos.erase(it);
    std::cout << "Curso eliminado.\n";
    // Registramos la acción en nuestra Pila LIFO
    registrarAccion(historial, "Curso eliminado: codigo " + codigo);
}

void listarCursos(const std::unordered_map<std::string, Curso>& cursos) {
    if (cursos.empty()) {
        std::cout << "(no hay cursos registrados)\n";
        return;
    }
    for (const auto& par : cursos) {
        std::cout << par.second << "\n";
    }
}

void menuCursos(std::unordered_map<std::string, Curso>& cursos, Pila<std::string>& historial) {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n-- Gestion de Cursos --\n";
        std::cout << "1. Registrar\n2. Buscar\n3. Eliminar\n4. Listar\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        
        switch (opcion) {
            case 1: registrarCurso(cursos, historial); break;
            case 2: buscarCurso(cursos); break;
            case 3: eliminarCurso(cursos, historial); break;
            case 4: listarCursos(cursos); break;
            case 0: break;
            default: std::cout << "Opcion invalida.\n";
        }
    }
}

// ---------------------------------------------------------------------
// Modulo 3: Recursos Academicos (ListaEnlazada)
// ---------------------------------------------------------------------
void registrarRecurso(ListaEnlazada<Recurso>& recursos, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo del recurso (ej. LAB01): ");
    if (codigo.empty()) {
        std::cout << "Error: el codigo no puede estar vacio.\n";
        return;
    }
    Nodo<Recurso>* existente = recursos.buscar([codigo](const Recurso& r) {
        return r.getCodigo() == codigo;
    });
    if (existente != nullptr) {
        std::cout << "Error: ya existe un recurso con ese codigo.\n";
        return;
    }
    std::string tipo = leerLinea("Tipo (laboratorio/equipo/libro): ");
    std::string descripcion = leerLinea("Descripcion: ");

    recursos.insertarFinal(Recurso(codigo, tipo, descripcion, "Disponible"));
    std::cout << "Recurso registrado.\n";
    registrarAccion(historial, "Recurso registrado: codigo " + codigo);
}

void buscarRecurso(const ListaEnlazada<Recurso>& recursos) {
    std::string codigo = leerLinea("Codigo a buscar: ");
    Nodo<Recurso>* encontrado = recursos.buscar([codigo](const Recurso& r) {
        return r.getCodigo() == codigo;
    });
    if (encontrado == nullptr) {
        std::cout << "No se encontro ningun recurso con ese codigo.\n";
        return;
    }
    std::cout << encontrado->dato << "\n";
}

void eliminarRecurso(ListaEnlazada<Recurso>& recursos, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo a eliminar: ");
    bool ok = recursos.eliminar([codigo](const Recurso& r) {
        return r.getCodigo() == codigo;
    });
    if (ok) {
        std::cout << "Recurso eliminado.\n";
        registrarAccion(historial, "Recurso eliminado: codigo " + codigo);
    } else {
        std::cout << "No se encontro ningun recurso con ese codigo.\n";
    }
}

void liberarRecurso(ListaEnlazada<Recurso>& recursos, Pila<std::string>& historial) {
    std::string codigo = leerLinea("Codigo del recurso a liberar: ");
    Nodo<Recurso>* nodoRecurso = recursos.buscar([&codigo](const Recurso& r) {
        return r.getCodigo() == codigo;
    });
    if (nodoRecurso == nullptr) {
        std::cout << "No se encontro ningun recurso con ese codigo.\n";
        return;
    }
    if (nodoRecurso->dato.getEstado() == "Disponible") {
        std::cout << "El recurso ya esta Disponible.\n";
        return;
    }
    nodoRecurso->dato.setEstado("Disponible");
    std::cout << "Recurso " << codigo << " liberado. Estado actualizado a Disponible.\n";
    registrarAccion(historial, "Recurso liberado: codigo " + codigo);
}

void menuRecursos(ListaEnlazada<Recurso>& recursos, Pila<std::string>& historial) {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n-- Gestion de Recursos Academicos (Lista Enlazada) --\n";
        std::cout << "1. Registrar\n2. Buscar\n3. Eliminar\n4. Mostrar todos\n5. Liberar (marcar Disponible)\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        switch (opcion) {
            case 1: registrarRecurso(recursos, historial); break;
            case 2: buscarRecurso(recursos); break;
            case 3: eliminarRecurso(recursos, historial); break;
            case 4: recursos.mostrar(); break;
            case 5: liberarRecurso(recursos, historial); break;
            case 0: break;
            default: std::cout << "Opcion invalida.\n";
        }
    }
}

// ---------------------------------------------------------------------
// Modulo de Solicitudes (Unificado en un solo Max-Heap)
// ---------------------------------------------------------------------
void registrarSolicitud(ColaPrioridad<Solicitud>& solicitudes,
                        const std::unordered_map<std::string, Estudiante>& estudiantes,
                        ListaEnlazada<Recurso>& recursos,
                        int& contadorSolicitudes, Pila<std::string>& historial) {
    std::string codigoEstudiante = leerLinea("Codigo del estudiante: ");
    if (estudiantes.find(codigoEstudiante) == estudiantes.end()) {
        std::cout << "Error: no existe un estudiante con ese codigo.\n";
        return;
    }
    std::string tipo = leerLinea("Tipo de solicitud/incidencia: ");
    int prioridad = leerEntero("Prioridad (0 = Regular, 1 a 5 = Prioritaria): ");

    if (prioridad < 0 || prioridad > 5) {
        std::cout << "Error: la prioridad debe estar entre 0 y 5.\n";
        return;
    }

    std::string esReserva = leerLinea("Es una solicitud de reserva de un recurso? (S/N): ");
    std::string codigoRecurso = "";

    if (esReserva == "S" || esReserva == "s") {
        codigoRecurso = leerLinea("Codigo del recurso a reservar (ej. LAB01): ");

        Nodo<Recurso>* nodoRecurso = recursos.buscar([&codigoRecurso](const Recurso& r) {
            return r.getCodigo() == codigoRecurso;
        });

        if (nodoRecurso == nullptr) {
            std::cout << "Error: no existe ningun recurso con ese codigo. No se registra la solicitud.\n";
            return;
        }
        if (nodoRecurso->dato.getEstado() != "Disponible") {
            std::cout << "Error: el recurso " << codigoRecurso << " no esta disponible (estado actual: "
                       << nodoRecurso->dato.getEstado() << "). No se registra la solicitud.\n";
            return;
        }

        // Se reserva de inmediato: el recurso queda bloqueado para otras
        // solicitudes mientras esta espera ser atendida.
        nodoRecurso->dato.setEstado("Ocupado");
    }

    Solicitud nueva(contadorSolicitudes, codigoEstudiante, tipo, prioridad, codigoRecurso);
    solicitudes.insertar(nueva);

    std::string etiqueta = (prioridad == 0) ? "regular" : "prioritaria";
    std::cout << "Solicitud " << etiqueta << " registrada con numero " << contadorSolicitudes;
    if (!codigoRecurso.empty()) {
        std::cout << " (reserva de " << codigoRecurso << ", recurso marcado como Ocupado)";
    }
    std::cout << ".\n";

    registrarAccion(historial, "Solicitud " + etiqueta + " registrada: numero " + std::to_string(contadorSolicitudes)
                     + (codigoRecurso.empty() ? "" : " (reserva de " + codigoRecurso + ")"));
    ++contadorSolicitudes;
}

void atenderSiguiente(ColaPrioridad<Solicitud>& solicitudes, Pila<std::string>& historial) {
    try {
        Solicitud atendida = solicitudes.extraerMayorPrioridad();
        std::string etiqueta = (atendida.getPrioridad() == 0) ? "regular" : "prioritaria";
        
        std::cout << "Atendida exitosamente:\n  -> " << atendida << "\n";
        registrarAccion(historial, "Solicitud " + etiqueta + " atendida: numero " + std::to_string(atendida.getNumero()));
    } catch (const std::underflow_error& e) {
        std::cout << "Error: No hay solicitudes pendientes por atender.\n";
    }
}

void mostrarEstructuras(const ColaPrioridad<Solicitud>& solicitudes) {
    std::cout << "\n-- Solicitudes Pendientes (Max-Heap Inteligente) --\n";
    solicitudes.mostrar();
}

// ---------------------------------------------------------------------
// Modulo 8: Historial (Pila) y Deshacer
// ---------------------------------------------------------------------
void mostrarHistorialYDeshacer(Pila<std::string>& historial) {
    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n-- Historial de Acciones (Pila LIFO) --\n";
        historial.mostrar();
        std::cout << "1. Deshacer ultima accion\n0. Volver\n";
        opcion = leerEntero("Opcion: ");
        if (opcion == 1) {
            try {
                std::string accion = historial.desapilar();
                std::cout << "Deshecho: \"" << accion << "\"\n";
                std::cout << "Nota: esta version solo retira el registro del historial;\n"
                          << "revertir el efecto de cada accion sobre estudiantes, cursos,\n"
                          << "recursos o solicitudes queda para una siguiente iteracion.\n";
            } catch (const std::underflow_error& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        } else if (opcion != 0) {
            std::cout << "Opcion invalida.\n";
        }
    }
}

// ---------------------------------------------------------------------
// Modulo 9: Reportes (Actualizado)
// ---------------------------------------------------------------------
void mostrarReportes(const std::unordered_map<std::string, Estudiante>& estudiantes,
                     const std::unordered_map<std::string, Curso>& cursos,
                     const ListaEnlazada<Recurso>& recursos,
                     const ColaPrioridad<Solicitud>& solicitudes) {
    std::cout << "\n-- Reportes y Estadisticas --\n";
    std::cout << "Estudiantes registrados:  " << estudiantes.size() << "\n";
    std::cout << "Cursos registrados:       " << cursos.size() << "\n";
    std::cout << "Recursos registrados:     " << recursos.size() << "\n";
    std::cout << "Solicitudes en espera:    " << solicitudes.size() << "\n";
}

} // namespace anonimo

// ===========================================================================
int main() {
    std::unordered_map<std::string, Estudiante> estudiantes;
    std::unordered_map<std::string, Curso> cursos;
    ListaEnlazada<Recurso> recursos;
    Pila<std::string> historial;
    ColaPrioridad<Solicitud> solicitudes; // ¡Una sola estructura para gobernarlas a todas!
    int contadorSolicitudes = 1;

    int opcion = -1;
    while (opcion != 0) {
        std::cout << "\n===== Sistema de Gestion Academica y Atencion de Solicitudes =====\n";
        std::cout << "1. Gestionar Estudiantes\n";
        std::cout << "2. Gestionar Cursos\n";
        std::cout << "3. Gestionar Recursos Academicos (Lista Enlazada)\n";
        std::cout << "4. Registrar Nueva Solicitud (Regular o Prioritaria)\n";
        std::cout << "5. Atender Siguiente Solicitud (Automatica por prioridad)\n";
        std::cout << "6. Mostrar Solicitudes Pendientes (Max-Heap)\n";
        std::cout << "7. Mostrar Historial de Acciones (Pila LIFO) y Deshacer\n";
        std::cout << "8. Reportes y Estadisticas Basicas\n";
        std::cout << "0. Salir\n";
        opcion = leerEntero("Opcion: ");

        switch (opcion) {
            case 1: menuEstudiantes(estudiantes, historial); break;
            case 2: menuCursos(cursos, historial); break;
            case 3: menuRecursos(recursos, historial); break;
            case 4: registrarSolicitud(solicitudes, estudiantes, recursos, contadorSolicitudes, historial); break;
            case 5: atenderSiguiente(solicitudes, historial); break;
            case 6: mostrarEstructuras(solicitudes); break;
            case 7: mostrarHistorialYDeshacer(historial); break;
            case 8: mostrarReportes(estudiantes, cursos, recursos, solicitudes); break;
            case 0: std::cout << "Saliendo. Todos los destructores liberaran su memoria.\n"; break;
            default: std::cout << "Opcion invalida.\n";
        }
    }

    return 0;
}
