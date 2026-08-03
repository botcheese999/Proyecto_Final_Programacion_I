#ifndef ENTIDADES_HPP
#define ENTIDADES_HPP

#include <iostream>
#include <string>

// ---------------------------------------------------------------------
// Estudiante
// ---------------------------------------------------------------------
class Estudiante {
private:
    std::string codigo; // formato UNSAAC: "2026001", etc. Se guarda como texto por seguridad de IDs.
    std::string nombre;
    std::string escuela;
    std::string semestre; // formato UNSAAC: "2026-I", "2026-II", etc.

public:
    Estudiante();
    Estudiante(const std::string& codigo, const std::string& nombre, const std::string& escuela, const std::string& semestre);

    const std::string& getCodigo() const;
    const std::string& getNombre() const;
    const std::string& getEscuela() const;
    const std::string& getSemestre() const;

    void setNombre(const std::string& nombre);
    void setEscuela(const std::string& escuela);
    void setSemestre(const std::string& semestre);

    friend std::ostream& operator<<(std::ostream& os, const Estudiante& e);
};

// ---------------------------------------------------------------------
// Curso
// ---------------------------------------------------------------------
class Curso {
private:
    std::string codigo; // formato UNSAAC: "IFI03AIN", etc.
    std::string nombre;
    int creditos;

public:
    Curso();
    Curso(const std::string& codigo, const std::string& nombre, int creditos);

    const std::string& getCodigo() const;
    const std::string& getNombre() const;
    int getCreditos() const;

    friend std::ostream& operator<<(std::ostream& os, const Curso& c);
};

// ---------------------------------------------------------------------
// Recurso (almacenado en ListaEnlazada<Recurso>)
// ---------------------------------------------------------------------
class Recurso {
private:
    std::string codigo; // formato UNSAAC: "LAB01", etc.
    std::string tipo;         // ej: laboratorio, equipo, libro
    std::string descripcion;
    std::string estado;       // Disponible, Ocupado, Mantenimiento

public:
    Recurso();
    Recurso(const std::string& codigo, const std::string& tipo, const std::string& descripcion,
            const std::string& estado = "Disponible");

    const std::string& getCodigo() const;
    const std::string& getTipo() const;
    const std::string& getDescripcion() const;
    const std::string& getEstado() const;

    void setEstado(const std::string& estado);

    friend std::ostream& operator<<(std::ostream& os, const Recurso& r);
};

// ---------------------------------------------------------------------
// Solicitud (almacenada en Cola<Solicitud> y ColaPrioridad<Solicitud>)
// ---------------------------------------------------------------------
class Solicitud {
private:
    int numero;
    std::string codigoEstudiante;
    std::string tipo;
    int prioridad; // 1..5, donde 5 es Critica. Para solicitudes regulares se deja en 0.
    std::string codigoRecurso; // vacio si la solicitud no es una reserva de recurso

public:
    Solicitud();
    Solicitud(int numero, const std::string& codigoEstudiante, const std::string& tipo, int prioridad = 0,
              const std::string& codigoRecurso = "");

    int getNumero() const;
    const std::string& getCodigoEstudiante() const;
    const std::string& getTipo() const;
    int getPrioridad() const;
    const std::string& getCodigoRecurso() const;
    bool esReservaDeRecurso() const; // true si codigoRecurso no esta vacio

    // Requisito de ColaPrioridad<T>: T debe soportar operator>.
    // Se compara únicamente por el campo prioridad, que es el criterio
    // de orden del Max-Heap.
    bool operator>(const Solicitud& otra) const;

    friend std::ostream& operator<<(std::ostream& os, const Solicitud& s);
};

#endif // ENTIDADES_HPP
