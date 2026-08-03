#ifndef COLA_PRIORIDAD_HPP
#define COLA_PRIORIDAD_HPP

#include <iostream>
#include <stdexcept>

// Cola de prioridad implementada como Max-Heap sobre un arreglo dinámico.
// Requisito de T: debe soportar operator> (para int ya viene dado;
// para tipos de dominio como Solicitud, se sobrecargará operator> más adelante).
// Uso previsto: Solicitudes Prioritarias.
template <typename T>
class ColaPrioridad {
private:
    T* elementos;
    size_t tamanio;     // cantidad de elementos actualmente almacenados
    size_t capacidad;   // tamaño actual del arreglo reservado

    // Índices del heap binario almacenado en arreglo (base 0):
    // padre(i)        = (i - 1) / 2
    // hijoIzquierdo(i) = 2*i + 1
    // hijoDerecho(i)   = 2*i + 2
    size_t padre(size_t indice) const;
    size_t hijoIzquierdo(size_t indice) const;
    size_t hijoDerecho(size_t indice) const;

    // Complejidad O(n): reserva un nuevo arreglo (el doble de capacidad)
    // y copia los elementos existentes. Se invoca solo cuando tamanio == capacidad.
    void redimensionar();

    // Complejidad O(log n): reubica elementos[indice] hacia arriba
    // intercambiando con su padre mientras lo supere. Usado tras insertar.
    void subir(size_t indice);

    // Complejidad O(log n): reubica elementos[indice] hacia abajo
    // intercambiando con el mayor de sus hijos mientras corresponda.
    // Usado tras extraer la raíz.
    void bajar(size_t indice);

public:
    explicit ColaPrioridad(size_t capacidadInicial = 8);
    ~ColaPrioridad();

    // Complejidad O(log n) amortizado (O(n) solo en el redimensionamiento ocasional):
    // coloca el nuevo elemento al final y lo sube hasta restaurar la propiedad de heap.
    void insertar(const T& valor);

    // Complejidad O(log n): intercambia la raíz con el último elemento,
    // la retira, y baja la nueva raíz hasta restaurar la propiedad de heap.
    // Lanza std::underflow_error si la cola está vacía.
    T extraerMayorPrioridad();

    // Complejidad O(1): consulta sin remover.
    // Lanza std::underflow_error si la cola está vacía.
    T consultarMayorPrioridad() const;

    // Complejidad O(n): imprime el arreglo en su orden interno de heap
    // (no es el orden de extracción, solo refleja cómo está almacenado).
    void mostrar() const;

    // Complejidad O(1): libera el arreglo con delete[]. Se llama desde el destructor.
    void limpiar();

    bool vacia() const;
    size_t size() const;

    // Complejidad O(n): recorre los elementos almacenados (sin extraerlos
    // ni alterar el heap) y devuelve true si al menos uno cumple pred.
    // Pensado para validaciones de integridad referencial, por ejemplo
    // impedir eliminar una entidad (estudiante, recurso, etc.) mientras
    // exista una Solicitud pendiente que la referencie.
    template <typename Predicado>
    bool existe(Predicado pred) const;
};

// Implementación de templates visible en el punto de instanciación.
#include "../src/ColaPrioridad.cpp"

#endif // COLA_PRIORIDAD_HPP
