#ifndef LISTA_ENLAZADA_CPP
#define LISTA_ENLAZADA_CPP

#include "../include/ListaEnlazada.hpp"

template <typename T>
ListaEnlazada<T>::ListaEnlazada() : cabeza(nullptr), cola(nullptr), tamanio(0) {}

template <typename T>
ListaEnlazada<T>::~ListaEnlazada() {
    limpiar();
}

template <typename T>
void ListaEnlazada<T>::insertarInicio(const T& valor) {
    Nodo<T>* nuevo = new Nodo<T>(valor);
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
    if (cola == nullptr) {
        cola = nuevo; // primer elemento: cabeza y cola coinciden
    }
    ++tamanio;
}

template <typename T>
void ListaEnlazada<T>::insertarFinal(const T& valor) {
    Nodo<T>* nuevo = new Nodo<T>(valor);
    if (cabeza == nullptr) {
        cabeza = nuevo;
        cola = nuevo;
    } else {
        cola->siguiente = nuevo;
        cola = nuevo;
    }
    ++tamanio;
}

template <typename T>
template <typename Predicado>
Nodo<T>* ListaEnlazada<T>::buscar(Predicado pred) const {
    Nodo<T>* actual = cabeza;
    while (actual != nullptr) {
        if (pred(actual->dato)) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

template <typename T>
template <typename Predicado>
bool ListaEnlazada<T>::eliminar(Predicado pred) {
    Nodo<T>* actual = cabeza;
    Nodo<T>* anterior = nullptr;

    while (actual != nullptr) {
        if (pred(actual->dato)) {
            if (anterior == nullptr) {
                cabeza = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            if (actual == cola) {
                cola = anterior;
            }
            delete actual;
            --tamanio;
            return true;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return false;
}

template <typename T>
void ListaEnlazada<T>::mostrar() const {
    if (cabeza == nullptr) {
        std::cout << "(lista vacia)\n";
        return;
    }
    Nodo<T>* actual = cabeza;
    while (actual != nullptr) {
        std::cout << actual->dato;
        if (actual->siguiente != nullptr) std::cout << " -> ";
        actual = actual->siguiente;
    }
    std::cout << "\n";
}

template <typename T>
void ListaEnlazada<T>::limpiar() {
    Nodo<T>* actual = cabeza;
    while (actual != nullptr) {
        Nodo<T>* siguienteNodo = actual->siguiente;
        delete actual;
        actual = siguienteNodo;
    }
    cabeza = nullptr;
    cola = nullptr;
    tamanio = 0;
}

template <typename T>
bool ListaEnlazada<T>::vacia() const {
    return cabeza == nullptr;
}

template <typename T>
size_t ListaEnlazada<T>::size() const {
    return tamanio;
}

#endif // LISTA_ENLAZADA_CPP
