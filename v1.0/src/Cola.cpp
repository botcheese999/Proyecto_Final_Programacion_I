#ifndef COLA_CPP
#define COLA_CPP

#include "../include/Cola.hpp"

template <typename T>
Cola<T>::Cola() : frente(nullptr), final(nullptr), tamanio(0) {}

template <typename T>
Cola<T>::~Cola() {
    limpiar();
}

template <typename T>
void Cola<T>::encolar(const T& valor) {
    NodoCola<T>* nuevo = new NodoCola<T>(valor);
    if (final == nullptr) {
        frente = nuevo;
        final = nuevo;
    } else {
        final->siguiente = nuevo;
        final = nuevo;
    }
    ++tamanio;
}

template <typename T>
T Cola<T>::desencolar() {
    if (frente == nullptr) {
        throw std::underflow_error("Cola vacia: no se puede desencolar.");
    }
    NodoCola<T>* nodoAEliminar = frente;
    T valor = nodoAEliminar->dato;
    frente = frente->siguiente;
    if (frente == nullptr) {
        final = nullptr; // la cola quedó vacía
    }
    delete nodoAEliminar;
    --tamanio;
    return valor;
}

template <typename T>
T Cola<T>::obtenerFrente() const {
    if (frente == nullptr) {
        throw std::underflow_error("Cola vacia: no hay frente.");
    }
    return frente->dato;
}

template <typename T>
void Cola<T>::mostrar() const {
    if (frente == nullptr) {
        std::cout << "(cola vacia)\n";
        return;
    }
    NodoCola<T>* actual = frente;
    std::cout << "FRENTE -> ";
    while (actual != nullptr) {
        std::cout << actual->dato;
        if (actual->siguiente != nullptr) std::cout << " -> ";
        actual = actual->siguiente;
    }
    std::cout << " <- FINAL\n";
}

template <typename T>
void Cola<T>::limpiar() {
    NodoCola<T>* actual = frente;
    while (actual != nullptr) {
        NodoCola<T>* siguienteNodo = actual->siguiente;
        delete actual;
        actual = siguienteNodo;
    }
    frente = nullptr;
    final = nullptr;
    tamanio = 0;
}

template <typename T>
bool Cola<T>::vacia() const {
    return frente == nullptr;
}

template <typename T>
size_t Cola<T>::size() const {
    return tamanio;
}

#endif // COLA_CPP
