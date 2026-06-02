#ifndef WAITINGQUEUE_H_INCLUDED
#define WAITINGQUEUE_H_INCLUDED

#include <queue>
#include <mutex>
#include "Paquete.h"

struct WaitingQueue {
    // Variables internas
    std::queue<Paquete> estanteria;
    std::mutex mtx;

    // Constructor default vacio
    WaitingQueue() = default;

    // Funciones para manipular la estantería
    void insertar_paquete(const Paquete& p);
    Paquete extraer_paquete();
    bool esta_vacia(); //V o F si esta vacia
};

// Función que ejecuta cada hilo
void productor_operario(int id_operario, WaitingQueue& waiting_queue, bool& sistema_activo);


#endif // WAITINGQUEUE_H_INCLUDED
