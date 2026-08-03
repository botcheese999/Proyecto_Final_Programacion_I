#ifndef COLA_PRIORIDAD_CPP
#define COLA_PRIORIDAD_CPP

#include "../include/ColaPrioridad.hpp"

template <typename T>
ColaPrioridad<T>::ColaPrioridad(size_t capacidadInicial)
    : elementos(new T[capacidadInicial]), tamanio(0), capacidad(capacidadInicial) {}

template <typename T>
ColaPrioridad<T>::~ColaPrioridad() {
    limpiar();
}

template <typename T>
size_t ColaPrioridad<T>::padre(size_t indice) const {
    return (indice - 1) / 2;
}

template <typename T>
size_t ColaPrioridad<T>::hijoIzquierdo(size_t indice) const {
    return 2 * indice + 1;
}

template <typename T>
size_t ColaPrioridad<T>::hijoDerecho(size_t indice) const {
    return 2 * indice + 2;
}

template <typename T>
void ColaPrioridad<T>::redimensionar() {
    size_t nuevaCapacidad = (capacidad == 0) ? 1 : capacidad * 2;
    T* nuevoArreglo = new T[nuevaCapacidad];

    for (size_t i = 0; i < tamanio; ++i) {
        nuevoArreglo[i] = elementos[i];
    }

    delete[] elementos;
    elementos = nuevoArreglo;
    capacidad = nuevaCapacidad;
}

template <typename T>
void ColaPrioridad<T>::subir(size_t indice) {
    while (indice > 0 && elementos[indice] > elementos[padre(indice)]) {
        T temp = elementos[indice];
        elementos[indice] = elementos[padre(indice)];
        elementos[padre(indice)] = temp;
        indice = padre(indice);
    }
}

template <typename T>
void ColaPrioridad<T>::bajar(size_t indice) {
    while (true) {
        size_t izquierdo = hijoIzquierdo(indice);
        size_t derecho = hijoDerecho(indice);
        size_t mayor = indice;

        if (izquierdo < tamanio && elementos[izquierdo] > elementos[mayor]) {
            mayor = izquierdo;
        }
        if (derecho < tamanio && elementos[derecho] > elementos[mayor]) {
            mayor = derecho;
        }
        if (mayor == indice) {
            break; // ya cumple la propiedad de heap
        }

        T temp = elementos[indice];
        elementos[indice] = elementos[mayor];
        elementos[mayor] = temp;
        indice = mayor;
    }
}

template <typename T>
void ColaPrioridad<T>::insertar(const T& valor) {
    if (tamanio == capacidad) {
        redimensionar();
    }
    elementos[tamanio] = valor;
    subir(tamanio);
    ++tamanio;
}

template <typename T>
T ColaPrioridad<T>::extraerMayorPrioridad() {
    if (tamanio == 0) {
        throw std::underflow_error("ColaPrioridad vacia: no se puede extraer.");
    }

    T raiz = elementos[0];
    elementos[0] = elementos[tamanio - 1];
    --tamanio;

    if (tamanio > 0) {
        bajar(0);
    }

    return raiz;
}

template <typename T>
T ColaPrioridad<T>::consultarMayorPrioridad() const {
    if (tamanio == 0) {
        throw std::underflow_error("ColaPrioridad vacia: no hay elemento de mayor prioridad.");
    }
    return elementos[0];
}

template <typename T>
void ColaPrioridad<T>::mostrar() const {
    if (tamanio == 0) {
        std::cout << "(cola de prioridad vacia)\n";
        return;
    }
    for (size_t i = 0; i < tamanio; ++i) {
        std::cout << elementos[i];
        if (i + 1 < tamanio) std::cout << " | ";
    }
    std::cout << "\n";
}

template <typename T>
void ColaPrioridad<T>::limpiar() {
    delete[] elementos;
    elementos = nullptr;
    tamanio = 0;
    capacidad = 0;
}

template <typename T>
bool ColaPrioridad<T>::vacia() const {
    return tamanio == 0;
}

template <typename T>
size_t ColaPrioridad<T>::size() const {
    return tamanio;
}

template <typename T>
template <typename Predicado>
bool ColaPrioridad<T>::existe(Predicado pred) const {
    for (size_t i = 0; i < tamanio; ++i) {
        if (pred(elementos[i])) {
            return true;
        }
    }
    return false;
}

#endif // COLA_PRIORIDAD_CPP
