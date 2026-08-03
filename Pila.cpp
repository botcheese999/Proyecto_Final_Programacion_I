#ifndef PILA_CPP
#define PILA_CPP

#include "../include/Pila.hpp"

template <typename T>
Pila<T>::Pila() : tope(nullptr), tamanio(0) {}

template <typename T>
Pila<T>::~Pila() {
    limpiar();
}

template <typename T>
void Pila<T>::apilar(const T& valor) {
    NodoPila<T>* nuevo = new NodoPila<T>(valor);
    nuevo->siguiente = tope;
    tope = nuevo;
    ++tamanio;
}

template <typename T>
T Pila<T>::desapilar() {
    if (tope == nullptr) {
        throw std::underflow_error("Pila vacia: no se puede desapilar.");
    }
    NodoPila<T>* nodoAEliminar = tope;
    T valor = nodoAEliminar->dato;
    tope = tope->siguiente;
    delete nodoAEliminar;
    --tamanio;
    return valor;
}

template <typename T>
T Pila<T>::obtenerTope() const {
    if (tope == nullptr) {
        throw std::underflow_error("Pila vacia: no hay tope.");
    }
    return tope->dato;
}

template <typename T>
void Pila<T>::mostrar() const {
    if (tope == nullptr) {
        std::cout << "(pila vacia)\n";
        return;
    }
    NodoPila<T>* actual = tope;
    std::cout << "TOPE -> ";
    while (actual != nullptr) {
        std::cout << actual->dato;
        if (actual->siguiente != nullptr) std::cout << " -> ";
        actual = actual->siguiente;
    }
    std::cout << "\n";
}

template <typename T>
void Pila<T>::limpiar() {
    NodoPila<T>* actual = tope;
    while (actual != nullptr) {
        NodoPila<T>* siguienteNodo = actual->siguiente;
        delete actual;
        actual = siguienteNodo;
    }
    tope = nullptr;
    tamanio = 0;
}

template <typename T>
bool Pila<T>::vacia() const {
    return tope == nullptr;
}

template <typename T>
size_t Pila<T>::size() const {
    return tamanio;
}

#endif // PILA_CPP
