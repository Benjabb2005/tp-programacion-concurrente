#ifndef PAQUETE_H_INCLUDED
#define PAQUETE_H_INCLUDED

#include <chrono>

struct Paquete {
    int identificador_unico;
    int nivel_de_prioridad; // 0 para baja, 1 para alta
    std::chrono::steady_clock::time_point fecha_de_creacion; //Usa el reloj interno de la PC
};

#endif // PAQUETE_H_INCLUDED
