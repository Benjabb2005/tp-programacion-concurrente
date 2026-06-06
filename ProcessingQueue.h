#ifndef PROCESSINGQUEUE_H_INCLUDED
#define PROCESSINGQUEUE_H_INCLUDED
#include <queue>
#include <mutex>
#include <condition_variable>
#include "Paquete.h"

extern long long espera_prioridad_0;
extern long long espera_prioridad_1;

extern int cantidad_prioridad_0;
extern int cantidad_prioridad_1;
extern std::mutex mtx_metricas;

//Para identificar cuando el paquete llegó a la cinta transportadora se pasa con un nuevo struct (esto sirve para calcular el tiempo en la cinta)
struct PaqueteEnProcesamiento{
    Paquete datos;
    std::chrono::steady_clock::time_point hora_ingreso; //para calcular tiempo en cinta
};
struct ProcessingQueue{
    std::queue<PaqueteEnProcesamiento> cinta_transportadora;
    std::mutex acceso_cola;
    std::condition_variable cv_productores; //condicion para los "productores" que pasan paquetes de la estanteria a la cinta transportadora
    std::condition_variable cv_consumidores; //condicion para los "consumidores" que procesan los paquetes en la cinta
    bool despacho_terminado = false;
};

void consumir_paquete(ProcessingQueue &cola);

#endif // PROCESSINGQUEUE_H_INCLUDED
