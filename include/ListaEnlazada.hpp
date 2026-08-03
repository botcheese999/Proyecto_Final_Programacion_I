#ifndef LISTA_ENLAZADA_HPP
#define LISTA_ENLAZADA_HPP

#include <iostream>
#include <stdexcept>

// Nodo genérico de la lista simplemente enlazada.
// Se deja como struct porque no necesita encapsulamiento propio:
// solo es un contenedor manipulado internamente por ListaEnlazada.
template <typename T>
struct Nodo {
    T dato;
    Nodo<T>* siguiente;

    explicit Nodo(const T& valor) : dato(valor), siguiente(nullptr) {}
};

// Lista simplemente enlazada genérica.
// Uso previsto: Módulo de Recursos Académicos (T = Recurso).
// Se deja como template para poder probarla con tipos simples (int, string)
// antes de integrar la clase Recurso en la siguiente iteración.
template <typename T>
class ListaEnlazada {
private:
    Nodo<T>* cabeza;
    Nodo<T>* cola;   // puntero al último nodo, para insertarFinal en O(1)
    size_t tamanio;

public:
    ListaEnlazada();
    ~ListaEnlazada();

    // Complejidad O(1): inserta directamente enlazando con la cabeza actual.
    void insertarInicio(const T& valor);

    // Complejidad O(1): gracias al puntero 'cola' no es necesario recorrer la lista.
    void insertarFinal(const T& valor);

    // Complejidad O(n): en el peor caso recorre toda la lista.
    // pred es un predicado (lambda o functor) que recibe const T& y devuelve bool.
    template <typename Predicado>
    Nodo<T>* buscar(Predicado pred) const;

    // Complejidad O(n): requiere localizar el nodo y actualizar el nodo anterior.
    // Devuelve true si se eliminó, false si no se encontró ningún elemento que cumpla pred.
    template <typename Predicado>
    bool eliminar(Predicado pred);

    // Complejidad O(n): recorre e imprime todos los elementos.
    void mostrar() const;

    // Complejidad O(n): libera cada nodo con delete. Se llama desde el destructor.
    void limpiar();

    bool vacia() const;
    size_t size() const;
};

// La implementación de templates debe ser visible en el punto de instanciación,
// por eso se incluye el .cpp al final del header (patrón estándar en C++ para templates).
#include "../src/ListaEnlazada.cpp"

#endif // LISTA_ENLAZADA_HPP
