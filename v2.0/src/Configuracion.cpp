#include "../include/Configuracion.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>

// ============================================================================
// Utilidades internas de parseo (identicas en espiritu al ParserJSON de
// referencia: trim, extraccion de enteros, extraccion de arreglos de strings).
// ============================================================================
namespace {

std::string trim(const std::string& s) {
    size_t inicio = 0;
    while (inicio < s.size() && (s[inicio] == ' ' || s[inicio] == '\t' ||
           s[inicio] == '\r' || s[inicio] == '\n'))
        ++inicio;
    if (inicio == s.size()) return "";
    size_t fin = s.size() - 1;
    while (fin > inicio && (s[fin] == ' ' || s[fin] == '\t' ||
           s[fin] == '\r' || s[fin] == '\n'))
        --fin;
    return s.substr(inicio, fin - inicio + 1);
}

int extraerEntero(const std::string& linea) {
    size_t pos = linea.find(':');
    if (pos == std::string::npos) return 0;
    std::string valor = trim(linea.substr(pos + 1));
    if (!valor.empty() && valor[valor.size() - 1] == ',')
        valor.erase(valor.size() - 1);
    return std::atoi(valor.c_str());
}

// Extrae el string entre comillas en una linea de la forma "clave": "valor".
std::string extraerString(const std::string& linea) {
    size_t primeraComilla = linea.find('"', linea.find(':'));
    if (primeraComilla == std::string::npos) return "";
    size_t segundaComilla = linea.find('"', primeraComilla + 1);
    if (segundaComilla == std::string::npos) return "";
    return linea.substr(primeraComilla + 1, segundaComilla - primeraComilla - 1);
}

// Parsea un arreglo JSON de strings, potencialmente repartido en varias
// lineas, hasta encontrar ']'. Misma logica que parsearArregloStrings del
// ParserJSON de referencia.
void parsearArregloStrings(std::ifstream& archivo, const std::string& lineaInicial,
                            std::string destino[], int maxDestino, int& cantidad) {
    cantidad = 0;
    std::string contenido = lineaInicial;

    size_t posAbre = contenido.find('[');
    if (posAbre == std::string::npos) return;

    size_t posCierra = contenido.find(']');
    if (posCierra == std::string::npos) {
        std::string lineaSig;
        int lineasLeidas = 0;
        const int MAX_LINEAS_ARRAY = 50;
        while (std::getline(archivo, lineaSig) && lineasLeidas < MAX_LINEAS_ARRAY) {
            ++lineasLeidas;
            contenido += " " + lineaSig;
            if (contenido.size() > 10000) {
                std::cerr << "[Configuracion] ADVERTENCIA: arreglo JSON excede tamano maximo." << std::endl;
                return;
            }
            if (lineaSig.find(']') != std::string::npos) break;
        }
        posCierra = contenido.find(']');
        if (posCierra == std::string::npos) {
            std::cerr << "[Configuracion] ADVERTENCIA: arreglo JSON sin cierre ']'." << std::endl;
            return;
        }
    }

    std::string interior = contenido.substr(posAbre + 1, posCierra - posAbre - 1);

    size_t i = 0;
    while (i < interior.size() && cantidad < maxDestino) {
        size_t comillaAbre = interior.find('"', i);
        if (comillaAbre == std::string::npos) break;
        size_t comillaCierra = interior.find('"', comillaAbre + 1);
        if (comillaCierra == std::string::npos) break;

        size_t largo = comillaCierra - comillaAbre - 1;
        if (largo > 200) largo = 200;
        destino[cantidad] = interior.substr(comillaAbre + 1, largo);
        ++cantidad;
        i = comillaCierra + 1;
    }
}

} // namespace anonimo

// ============================================================================
// ConfigManager
// ============================================================================
ConfigManager::ConfigManager()
    : longitud_minima_nombre(2), longitud_minima_semestre(1), prioridad_maxima(5),
      numEstadosValidos(0),
      ruta_estudiantes("data/estudiantes.csv"), ruta_cursos("data/cursos.csv"),
      ruta_recursos("data/recursos.csv"), ruta_solicitudes("data/solicitudes.csv"),
      ruta_historial("data/historial.csv") {
    estados_validos_recurso[0] = "Disponible";
    estados_validos_recurso[1] = "Ocupado";
    estados_validos_recurso[2] = "Mantenimiento";
    numEstadosValidos = 3;
}

bool ConfigManager::esEstadoValido(const std::string& estado) const {
    for (int i = 0; i < numEstadosValidos; ++i) {
        if (estados_validos_recurso[i] == estado) return true;
    }
    return false;
}

// ============================================================================
// cargarConfiguracion — lee y parsea config.json manualmente.
// Si el archivo no existe, se mantienen los defaults del constructor.
// ============================================================================
ConfigManager cargarConfiguracion(const std::string& ruta) {
    ConfigManager config; // ya trae los defaults cargados

    std::ifstream archivo(ruta.c_str());
    if (!archivo.is_open()) {
        std::cerr << "[Configuracion] AVISO: no se pudo abrir " << ruta
                  << ". Usando valores por defecto.\n";
        return config;
    }

    std::string linea;
    while (std::getline(archivo, linea)) {
        std::string lineaTrim = trim(linea);
        if (lineaTrim.empty() || lineaTrim[0] == '{' || lineaTrim[0] == '}')
            continue;

        if (lineaTrim.find("\"longitud_minima_nombre\"") != std::string::npos) {
            config.longitud_minima_nombre = extraerEntero(lineaTrim);
        } else if (lineaTrim.find("\"longitud_minima_semestre\"") != std::string::npos) {
            config.longitud_minima_semestre = extraerEntero(lineaTrim);
        } else if (lineaTrim.find("\"prioridad_maxima\"") != std::string::npos) {
            config.prioridad_maxima = extraerEntero(lineaTrim);
        } else if (lineaTrim.find("\"estados_validos_recurso\"") != std::string::npos) {
            parsearArregloStrings(archivo, lineaTrim, config.estados_validos_recurso,
                                   ConfigManager::MAX_ESTADOS, config.numEstadosValidos);
        } else if (lineaTrim.find("\"ruta_estudiantes\"") != std::string::npos) {
            config.ruta_estudiantes = extraerString(lineaTrim);
        } else if (lineaTrim.find("\"ruta_cursos\"") != std::string::npos) {
            config.ruta_cursos = extraerString(lineaTrim);
        } else if (lineaTrim.find("\"ruta_recursos\"") != std::string::npos) {
            config.ruta_recursos = extraerString(lineaTrim);
        } else if (lineaTrim.find("\"ruta_solicitudes\"") != std::string::npos) {
            config.ruta_solicitudes = extraerString(lineaTrim);
        } else if (lineaTrim.find("\"ruta_historial\"") != std::string::npos) {
            config.ruta_historial = extraerString(lineaTrim);
        }
    }
    archivo.close();

    // Sanitizacion defensiva, mismo criterio que el ParserJSON de referencia.
    if (config.longitud_minima_nombre < 1 || config.longitud_minima_nombre > 100) {
        std::cerr << "[Configuracion] ADVERTENCIA: longitud_minima_nombre invalida. Usando default 2.\n";
        config.longitud_minima_nombre = 2;
    }
    if (config.prioridad_maxima < 1 || config.prioridad_maxima > 10) {
        std::cerr << "[Configuracion] ADVERTENCIA: prioridad_maxima invalida. Usando default 5.\n";
        config.prioridad_maxima = 5;
    }
    if (config.numEstadosValidos == 0) {
        std::cerr << "[Configuracion] ADVERTENCIA: sin estados validos. Usando defaults.\n";
        config.estados_validos_recurso[0] = "Disponible";
        config.estados_validos_recurso[1] = "Ocupado";
        config.estados_validos_recurso[2] = "Mantenimiento";
        config.numEstadosValidos = 3;
    }

    return config;
}
