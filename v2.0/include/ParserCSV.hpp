#ifndef PARSER_CSV_HPP
#define PARSER_CSV_HPP

#include <string>
#include "ListaEnlazada.hpp"
#include "Pila.hpp"
#include "ColaPrioridad.hpp"
#include "Entidades.hpp"

// ============================================================================
// ParserCSV — persistencia bidireccional manual (sin STL en las colecciones,
// sin librerias de terceros). Cada entidad tiene su cargarX/guardarX.
//
// splitCSV() es un automata de estados finitos que recorre la linea
// caracter a caracter con un flag 'dentroComillas': mientras esta activo,
// una coma NO delimita campo (es texto), y '""' dentro de comillas se
// interpreta como una comilla literal escapada (regla RFC4180).
//
// leerRegistroCSV() resuelve el caso de campos con saltos de linea: cuenta
// comillas sin cerrar y sigue concatenando lineas fisicas hasta que el
// registro logico queda balanceado, antes de pasarlo a splitCSV().
// ============================================================================

static const int MAX_CAMPOS_CSV = 10;

// Separa una linea (o registro multi-linea ya concatenado) en campos.
// Retorna la cantidad de campos parseados, hasta maxCampos.
int splitCSV(const std::string& registro, std::string campos[], int maxCampos);

// Envuelve 'campo' en comillas dobles (escapando comillas internas como "")
// si contiene coma, comilla o salto de linea. Si no, lo retorna tal cual.
std::string escaparCampoCSV(const std::string& campo);

// ----- Loaders: leen su CSV y llenan la estructura recibida. --------------
// Si el archivo no existe (primera ejecucion), no es un error: se informa
// y la estructura queda vacia.

void cargarEstudiantes(const std::string& ruta, ListaEnlazada<Estudiante>& lista);
void cargarCursos(const std::string& ruta, ListaEnlazada<Curso>& lista);
void cargarRecursos(const std::string& ruta, ListaEnlazada<Recurso>& lista);

// Reconstruye el Max-Heap de solicitudes con insertar() (el orden de
// insercion no afecta la correccion del heap) y deja en 'contadorSiguiente'
// el proximo numero de solicitud disponible (max(numero) + 1, o 1 si vacio).
void cargarSolicitudes(const std::string& ruta, ColaPrioridad<Solicitud>& solicitudes,
                        int& contadorSiguiente);

// Reconstruye la Pila preservando el orden original (el tope guardado
// vuelve a quedar en el tope al recargar).
void cargarHistorial(const std::string& ruta, Pila<std::string>& historial);

// ----- Writers: persisten desde memoria hacia CSV. -------------------------
// Si la carpeta destino no existe, informan el error sin interrumpir el
// cierre del programa (no se pierde lo que ya se guardo en otros archivos).

void guardarEstudiantes(const std::string& ruta, const ListaEnlazada<Estudiante>& lista);
void guardarCursos(const std::string& ruta, const ListaEnlazada<Curso>& lista);
void guardarRecursos(const std::string& ruta, const ListaEnlazada<Recurso>& lista);
void guardarSolicitudes(const std::string& ruta, const ColaPrioridad<Solicitud>& solicitudes);
void guardarHistorial(const std::string& ruta, const Pila<std::string>& historial);

#endif // PARSER_CSV_HPP
