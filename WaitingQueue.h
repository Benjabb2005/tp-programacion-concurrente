#ifndef WAITINGQUEUE_H_INCLUDED
#define WAITINGQUEUE_H_INCLUDED

#include <queue>
#include <mutex>
#include "Paquete.h"

extern int contador_global_paquetes_generados;
extern std::mutex mtx_contador_global;

extern int generador_global_ids;
extern std::mutex mtx_generador_ids;

struct WaitingQueue {
    // Variables internas
    std::queue<Paquete> estanteria_alta;
    std::queue<Paquete> estanteria_baja;
    std::mutex mtx;

    // Constructor default vacio
    WaitingQueue() = default;

    // Funciones para manipular la estantería
    void insertar_paquete(const Paquete& p);
    Paquete extraer_paquete();
    bool esta_vacia(); //V o F si esta vacia
};

// Función que ejecuta cada hilo
// Función que ejecuta cada hilo
void productor_operario(WaitingQueue& waiting_queue, bool& sistema_activo, int modo_prueba, int cantidadPaquetes);

#endif // WAITINGQUEUE_H_INCLUDED
