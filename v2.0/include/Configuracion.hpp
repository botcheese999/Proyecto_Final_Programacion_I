#ifndef CONFIGURACION_HPP
#define CONFIGURACION_HPP

#include <string>

// ============================================================================
// ConfigManager — centraliza las reglas de negocio del sistema (limites,
// estados validos, rutas de archivos) leyendas desde config.json.
//
// Se parsea manualmente linea por linea con <fstream>/find()/substr(),
// sin ninguna libreria externa de JSON (regla del proyecto).
// ============================================================================
struct ConfigManager {
    // ----- Limites y reglas de validacion -----
    int longitud_minima_nombre;   // aplica a nombre de Estudiante/Curso
    int longitud_minima_semestre; // semestre acepta "3" o "2026-I": solo se
                                   // exige no vacio y una longitud minima
    int prioridad_maxima;         // tope superior de Solicitud.prioridad

    // ----- Estados validos de Recurso (arreglo estatico, sin STL) -----
    static const int MAX_ESTADOS = 5;
    std::string estados_validos_recurso[MAX_ESTADOS];
    int numEstadosValidos;

    // ----- Rutas de archivos (usadas desde Fase 3 en adelante) -----
    std::string ruta_estudiantes;
    std::string ruta_cursos;
    std::string ruta_recursos;
    std::string ruta_solicitudes;
    std::string ruta_historial;

    ConfigManager();

    // true si 'estado' aparece en estados_validos_recurso.
    bool esEstadoValido(const std::string& estado) const;
};

// Carga y parsea manualmente config.json. Si el archivo no existe o esta
// incompleto, cae en los valores por defecto definidos en el constructor
// de ConfigManager (mismo criterio defensivo que el ParserJSON de referencia).
ConfigManager cargarConfiguracion(const std::string& ruta);

#endif // CONFIGURACION_HPP
