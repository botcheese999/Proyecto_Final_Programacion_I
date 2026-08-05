#ifndef PILA_HPP
#define PILA_HPP

#include <iostream>
#include <stdexcept>

// Nodo propio de la pila (independiente de Nodo<T> de ListaEnlazada,
// para que cada estructura sea autocontenida como pide el mega-prompt).
template <typename T>
struct NodoPila {
    T dato;
    NodoPila<T>* siguiente;

    explicit NodoPila(const T& valor) : dato(valor), siguiente(nullptr) {}
};

// Pila LIFO implementada con nodos enlazados.
// Uso previsto: Historial de Acciones (deshacer / Ctrl+Z).
template <typename T>
class Pila {
private:
    NodoPila<T>* tope;
    size_t tamanio;

public:
    Pila();
    ~Pila();

    // Complejidad O(1): se enlaza el nuevo nodo directamente sobre el tope actual.
    void apilar(const T& valor);

    // Complejidad O(1): retira el nodo tope y libera su memoria.
    // Lanza std::underflow_error si la pila está vacía.
    T desapilar();

    // Complejidad O(1): consulta sin remover.
    // Lanza std::underflow_error si la pila está vacía.
    T obtenerTope() const;

    // Complejidad O(n): recorre desde el tope hasta la base.
    void mostrar() const;

    // Complejidad O(n): aplica 'accion' a cada elemento de tope a base,
    // sin desapilar. Uso previsto: exportar el historial a CSV (Fase 3).
    template <typename Accion>
    void recorrer(Accion accion) const;

    // Complejidad O(n): libera cada nodo con delete. Se llama desde el destructor.
    void limpiar();

    bool vacia() const;
    size_t size() const;
};

// Implementación de templates visible en el punto de instanciación.
#include "../src/Pila.cpp"

#endif // PILA_HPP
