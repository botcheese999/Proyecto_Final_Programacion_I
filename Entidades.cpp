#include "../include/Entidades.hpp"

// ---------------------------------------------------------------------
// Estudiante
// ---------------------------------------------------------------------
Estudiante::Estudiante() : codigo(""), nombre(""), escuela(""), semestre("") {}

Estudiante::Estudiante(const std::string& codigo, const std::string& nombre, const std::string& escuela, const std::string& semestre)
    : codigo(codigo), nombre(nombre), escuela(escuela), semestre(semestre) {}

const std::string& Estudiante::getCodigo() const { return codigo; }
const std::string& Estudiante::getNombre() const { return nombre; }
const std::string& Estudiante::getEscuela() const { return escuela; }
const std::string& Estudiante::getSemestre() const { return semestre; }

void Estudiante::setNombre(const std::string& nombre_) { nombre = nombre_; }
void Estudiante::setEscuela(const std::string& escuela_) { escuela = escuela_; }
void Estudiante::setSemestre(const std::string& semestre_) { semestre = semestre_; }

std::ostream& operator<<(std::ostream& os, const Estudiante& e) {
    os << "[" << e.codigo << "] " << e.nombre
       << " | " << e.escuela << " | Sem. " << e.semestre;
    return os;
}

// ---------------------------------------------------------------------
// Curso
// ---------------------------------------------------------------------
Curso::Curso() : codigo(""), nombre(""), creditos(0) {}

Curso::Curso(const std::string& codigo, const std::string& nombre, int creditos)
    : codigo(codigo), nombre(nombre), creditos(creditos) {}

const std::string& Curso::getCodigo() const { return codigo; }
const std::string& Curso::getNombre() const { return nombre; }
int Curso::getCreditos() const { return creditos; }

std::ostream& operator<<(std::ostream& os, const Curso& c) {
    os << "[" << c.codigo << "] " << c.nombre << " | " << c.creditos << " creditos";
    return os;
}

// ---------------------------------------------------------------------
// Recurso
// ---------------------------------------------------------------------
Recurso::Recurso() : codigo(""), tipo(""), descripcion(""), estado("Disponible") {}

Recurso::Recurso(const std::string& codigo, const std::string& tipo, const std::string& descripcion,
                  const std::string& estado)
    : codigo(codigo), tipo(tipo), descripcion(descripcion), estado(estado) {}

const std::string& Recurso::getCodigo() const { return codigo; }
const std::string& Recurso::getTipo() const { return tipo; }
const std::string& Recurso::getDescripcion() const { return descripcion; }
const std::string& Recurso::getEstado() const { return estado; }

void Recurso::setEstado(const std::string& estado_) { estado = estado_; }

std::ostream& operator<<(std::ostream& os, const Recurso& r) {
    os << "[" << r.codigo << "] " << r.tipo << " - " << r.descripcion
       << " (" << r.estado << ")";
    return os;
}

// ---------------------------------------------------------------------
// Solicitud
// ---------------------------------------------------------------------
Solicitud::Solicitud() : numero(0), codigoEstudiante(""), tipo(""), prioridad(0), codigoRecurso("") {}

Solicitud::Solicitud(int numero, const std::string& codigoEstudiante, const std::string& tipo, int prioridad,
                      const std::string& codigoRecurso)
    : numero(numero), codigoEstudiante(codigoEstudiante), tipo(tipo), prioridad(prioridad),
      codigoRecurso(codigoRecurso) {}

int Solicitud::getNumero() const { return numero; }
const std::string& Solicitud::getCodigoEstudiante() const { return codigoEstudiante; }
const std::string& Solicitud::getTipo() const { return tipo; }
int Solicitud::getPrioridad() const { return prioridad; }
const std::string& Solicitud::getCodigoRecurso() const { return codigoRecurso; }
bool Solicitud::esReservaDeRecurso() const { return !codigoRecurso.empty(); }

bool Solicitud::operator>(const Solicitud& otra) const {
    // Si las prioridades son distintas, gana la de mayor prioridad
    if (prioridad != otra.prioridad) {
        return prioridad > otra.prioridad;
    }
    // Si tienen la misma prioridad (empate), gana la que llegó PRIMERO.
    // Menor número de solicitud = llegó antes = mayor urgencia de atención (FIFO).
    return numero < otra.numero;
}

std::ostream& operator<<(std::ostream& os, const Solicitud& s) {
    os << "Solicitud #" << s.numero << " (Est. " << s.codigoEstudiante << ") "
       << s.tipo << " - Prioridad " << s.prioridad;
    if (s.esReservaDeRecurso()) {
        os << " - Reserva de recurso: " << s.codigoRecurso;
    }
    return os;
}
