#ifndef COLA_HPP
#define COLA_HPP

#include <iostream>
#include <stdexcept>

// Nodo propio de la cola (independiente de Nodo<T> y NodoPila<T>,
// cada estructura es autocontenida).
template <typename T>
struct NodoCola {
    T dato;
    NodoCola<T>* siguiente;

    explicit NodoCola(const T& valor) : dato(valor), siguiente(nullptr) {}
};

// Cola FIFO implementada con nodos enlazados.
// Mantiene punteros a 'frente' y 'final' para garantizar
// encolar/desencolar en O(1). Uso previsto: Solicitudes Regulares.
template <typename T>
class Cola {
private:
    NodoCola<T>* frente;
    NodoCola<T>* final;
    size_t tamanio;

public:
    Cola();
    ~Cola();

    // Complejidad O(1): se enlaza el nuevo nodo al final actual.
    void encolar(const T& valor);

    // Complejidad O(1): retira el nodo frente y libera su memoria.
    // Lanza std::underflow_error si la cola está vacía.
    T desencolar();

    // Complejidad O(1): consulta sin remover.
    // Lanza std::underflow_error si la cola está vacía.
    T obtenerFrente() const;

    // Complejidad O(n): recorre desde el frente hasta el final.
    void mostrar() const;

    // Complejidad O(n): libera cada nodo con delete. Se llama desde el destructor.
    void limpiar();

    bool vacia() const;
    size_t size() const;
};

// Implementación de templates visible en el punto de instanciación.
#include "../src/Cola.cpp"

#endif // COLA_HPP
