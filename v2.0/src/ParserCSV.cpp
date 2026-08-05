#include "../include/ParserCSV.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

// ----------------------------------------------------------------------
// splitCSV — automata de estados finitos (fuera-de-comillas / dentro-de-
// comillas) recorriendo el registro caracter a caracter. Declarada en el
// header con enlace externo porque se usa tambien fuera de este archivo.
// ----------------------------------------------------------------------
int splitCSV(const std::string& registro, std::string campos[], int maxCampos) {
    int cantidad = 0;
    std::string actual;
    bool dentroComillas = false;
    size_t i = 0;

    while (i < registro.size() && cantidad < maxCampos) {
        char c = registro[i];

        if (dentroComillas) {
            if (c == '"') {
                // Comilla escapada: "" dentro de un campo entre comillas
                // representa una comilla literal.
                if (i + 1 < registro.size() && registro[i + 1] == '"') {
                    actual += '"';
                    i += 2;
                    continue;
                }
                dentroComillas = false; // cierre de comillas
                ++i;
                continue;
            }
            actual += c;
            ++i;
            continue;
        }

        // Fuera de comillas:
        if (c == '"') {
            dentroComillas = true;
            ++i;
        } else if (c == ',') {
            campos[cantidad++] = actual;
            actual.clear();
            ++i;
        } else {
            actual += c;
            ++i;
        }
    }

    if (cantidad < maxCampos) {
        campos[cantidad++] = actual;
    }
    return cantidad;
}

std::string escaparCampoCSV(const std::string& campo) {
    bool necesitaComillas = campo.find(',') != std::string::npos ||
                             campo.find('"') != std::string::npos ||
                             campo.find('\n') != std::string::npos ||
                             campo.find('\r') != std::string::npos;
    if (!necesitaComillas) return campo;

    std::string resultado = "\"";
    for (char c : campo) {
        if (c == '"') resultado += "\"\"";
        else resultado += c;
    }
    resultado += "\"";
    return resultado;
}

namespace {

// ----------------------------------------------------------------------
// Utilidades internas de lectura/escritura, no expuestas en el header.
// ----------------------------------------------------------------------

// true si 'texto' tiene un numero par de comillas (registro CSV balanceado,
// sin campos entre comillas que sigan "abiertos" por un salto de linea).
bool registroBalanceado(const std::string& texto) {
    int comillas = 0;
    for (char c : texto) {
        if (c == '"') ++comillas;
    }
    return (comillas % 2) == 0;
}

// Lee un registro logico completo: si 'primeraLinea' termina con comillas
// sin cerrar (campo con salto de linea embebido), sigue leyendo lineas
// fisicas del archivo y las concatena con '\n' hasta balancear. Mismo
// patron que 'parsearArregloStrings' en Configuracion.cpp.
std::string leerRegistroCSV(std::ifstream& archivo, const std::string& primeraLinea) {
    std::string registro = primeraLinea;
    std::string siguiente;
    int limiteLineas = 500;

    while (!registroBalanceado(registro) && limiteLineas-- > 0 &&
           std::getline(archivo, siguiente)) {
        registro += "\n" + siguiente;
    }
    return registro;
}

// Escribe la cabecera y abre el archivo; retorna false si no se pudo abrir
// (ej. la carpeta 'data/' no existe) e imprime un aviso claro.
bool abrirParaEscritura(std::ofstream& archivo, const std::string& ruta,
                         const std::string& cabecera) {
    archivo.open(ruta.c_str());
    if (!archivo.is_open()) {
        std::cerr << "[ParserCSV] ERROR: no se pudo escribir en '" << ruta
                   << "'. Verifique que la carpeta contenedora exista.\n";
        return false;
    }
    archivo << cabecera << "\n";
    return true;
}

// Abre el archivo para lectura; si no existe, no es un error (primera
// ejecucion del sistema), solo se informa y se deja la estructura vacia.
bool abrirParaLectura(std::ifstream& archivo, const std::string& ruta) {
    archivo.open(ruta.c_str());
    if (!archivo.is_open()) {
        std::cerr << "[ParserCSV] AVISO: '" << ruta
                   << "' no existe todavia. Se inicia vacio.\n";
        return false;
    }
    return true;
}

} // namespace anonimo

// ============================================================================
// Estudiante — campos: codigo,nombre,escuela,semestre
// ============================================================================
void cargarEstudiantes(const std::string& ruta, ListaEnlazada<Estudiante>& lista) {
    std::ifstream archivo;
    if (!abrirParaLectura(archivo, ruta)) return;

    std::string linea;
    bool esCabecera = true;
    while (std::getline(archivo, linea)) {
        if (esCabecera) { esCabecera = false; continue; }
        if (linea.empty()) continue;

        std::string registro = leerRegistroCSV(archivo, linea);
        std::string campos[MAX_CAMPOS_CSV];
        int n = splitCSV(registro, campos, MAX_CAMPOS_CSV);
        if (n < 4) {
            std::cerr << "[ParserCSV] ADVERTENCIA: registro de estudiante incompleto, se omite.\n";
            continue;
        }
        lista.insertarFinal(Estudiante(campos[0], campos[1], campos[2], campos[3]));
    }
}

void guardarEstudiantes(const std::string& ruta, const ListaEnlazada<Estudiante>& lista) {
    std::ofstream archivo;
    if (!abrirParaEscritura(archivo, ruta, "codigo,nombre,escuela,semestre")) return;

    lista.recorrer([&archivo](const Estudiante& e) {
        archivo << escaparCampoCSV(e.getCodigo()) << ","
                << escaparCampoCSV(e.getNombre()) << ","
                << escaparCampoCSV(e.getEscuela()) << ","
                << escaparCampoCSV(e.getSemestre()) << "\n";
    });
}

// ============================================================================
// Curso — campos: codigo,nombre,creditos
// ============================================================================
void cargarCursos(const std::string& ruta, ListaEnlazada<Curso>& lista) {
    std::ifstream archivo;
    if (!abrirParaLectura(archivo, ruta)) return;

    std::string linea;
    bool esCabecera = true;
    while (std::getline(archivo, linea)) {
        if (esCabecera) { esCabecera = false; continue; }
        if (linea.empty()) continue;

        std::string registro = leerRegistroCSV(archivo, linea);
        std::string campos[MAX_CAMPOS_CSV];
        int n = splitCSV(registro, campos, MAX_CAMPOS_CSV);
        if (n < 3) {
            std::cerr << "[ParserCSV] ADVERTENCIA: registro de curso incompleto, se omite.\n";
            continue;
        }
        int creditos = std::atoi(campos[2].c_str());
        lista.insertarFinal(Curso(campos[0], campos[1], creditos));
    }
}

void guardarCursos(const std::string& ruta, const ListaEnlazada<Curso>& lista) {
    std::ofstream archivo;
    if (!abrirParaEscritura(archivo, ruta, "codigo,nombre,creditos")) return;

    lista.recorrer([&archivo](const Curso& c) {
        archivo << escaparCampoCSV(c.getCodigo()) << ","
                << escaparCampoCSV(c.getNombre()) << ","
                << c.getCreditos() << "\n";
    });
}

// ============================================================================
// Recurso — campos: codigo,tipo,descripcion,estado
// ============================================================================
void cargarRecursos(const std::string& ruta, ListaEnlazada<Recurso>& lista) {
    std::ifstream archivo;
    if (!abrirParaLectura(archivo, ruta)) return;

    std::string linea;
    bool esCabecera = true;
    while (std::getline(archivo, linea)) {
        if (esCabecera) { esCabecera = false; continue; }
        if (linea.empty()) continue;

        std::string registro = leerRegistroCSV(archivo, linea);
        std::string campos[MAX_CAMPOS_CSV];
        int n = splitCSV(registro, campos, MAX_CAMPOS_CSV);
        if (n < 4) {
            std::cerr << "[ParserCSV] ADVERTENCIA: registro de recurso incompleto, se omite.\n";
            continue;
        }
        lista.insertarFinal(Recurso(campos[0], campos[1], campos[2], campos[3]));
    }
}

void guardarRecursos(const std::string& ruta, const ListaEnlazada<Recurso>& lista) {
    std::ofstream archivo;
    if (!abrirParaEscritura(archivo, ruta, "codigo,tipo,descripcion,estado")) return;

    lista.recorrer([&archivo](const Recurso& r) {
        archivo << escaparCampoCSV(r.getCodigo()) << ","
                << escaparCampoCSV(r.getTipo()) << ","
                << escaparCampoCSV(r.getDescripcion()) << ","
                << escaparCampoCSV(r.getEstado()) << "\n";
    });
}

// ============================================================================
// Solicitud — campos: numero,codigoEstudiante,tipo,prioridad,codigoRecurso
// El orden de insercion no importa para el heap; solo se recalcula el
// contador de numero de solicitud al terminar de cargar.
// ============================================================================
void cargarSolicitudes(const std::string& ruta, ColaPrioridad<Solicitud>& solicitudes,
                        int& contadorSiguiente) {
    std::ifstream archivo;
    contadorSiguiente = 1;
    if (!abrirParaLectura(archivo, ruta)) return;

    std::string linea;
    bool esCabecera = true;
    int maxNumero = 0;
    while (std::getline(archivo, linea)) {
        if (esCabecera) { esCabecera = false; continue; }
        if (linea.empty()) continue;

        std::string registro = leerRegistroCSV(archivo, linea);
        std::string campos[MAX_CAMPOS_CSV];
        int n = splitCSV(registro, campos, MAX_CAMPOS_CSV);
        if (n < 5) {
            std::cerr << "[ParserCSV] ADVERTENCIA: registro de solicitud incompleto, se omite.\n";
            continue;
        }
        int numero = std::atoi(campos[0].c_str());
        int prioridad = std::atoi(campos[3].c_str());
        solicitudes.insertar(Solicitud(numero, campos[1], campos[2], prioridad, campos[4]));
        if (numero > maxNumero) maxNumero = numero;
    }
    contadorSiguiente = maxNumero + 1;
}

void guardarSolicitudes(const std::string& ruta, const ColaPrioridad<Solicitud>& solicitudes) {
    std::ofstream archivo;
    if (!abrirParaEscritura(archivo, ruta, "numero,codigoEstudiante,tipo,prioridad,codigoRecurso")) return;

    solicitudes.recorrer([&archivo](const Solicitud& s) {
        archivo << s.getNumero() << ","
                << escaparCampoCSV(s.getCodigoEstudiante()) << ","
                << escaparCampoCSV(s.getTipo()) << ","
                << s.getPrioridad() << ","
                << escaparCampoCSV(s.getCodigoRecurso()) << "\n";
    });
}

// ============================================================================
// Historial (Pila<std::string>) — una sola columna: accion
// Se guarda de base a tope (recorrer() da tope->base, por eso se invierte
// primero con insertarInicio) para que al recargar en orden de archivo con
// apilar() el tope original vuelva a quedar en el tope.
// ============================================================================
void guardarHistorial(const std::string& ruta, const Pila<std::string>& historial) {
    ListaEnlazada<std::string> invertida;
    historial.recorrer([&invertida](const std::string& accion) {
        invertida.insertarInicio(accion); // tope->base se invierte a base->tope
    });

    std::ofstream archivo;
    if (!abrirParaEscritura(archivo, ruta, "accion")) return;

    invertida.recorrer([&archivo](const std::string& accion) {
        archivo << escaparCampoCSV(accion) << "\n";
    });
}

void cargarHistorial(const std::string& ruta, Pila<std::string>& historial) {
    std::ifstream archivo;
    if (!abrirParaLectura(archivo, ruta)) return;

    std::string linea;
    bool esCabecera = true;
    while (std::getline(archivo, linea)) {
        if (esCabecera) { esCabecera = false; continue; }
        if (linea.empty()) continue;

        std::string registro = leerRegistroCSV(archivo, linea);
        std::string campos[MAX_CAMPOS_CSV];
        int n = splitCSV(registro, campos, MAX_CAMPOS_CSV);
        if (n < 1) continue;
        historial.apilar(campos[0]); // orden de archivo (base->tope) reconstruye el tope original
    }
}
