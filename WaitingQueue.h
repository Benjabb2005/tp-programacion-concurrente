#ifndef WAITINGQUEUE_H_INCLUDED
#define WAITINGQUEUE_H_INCLUDED

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "Paquete.h"

extern int contador_global_paquetes_generados;
extern std::mutex mtx_contador_global;

extern int generador_global_ids;
extern std::mutex mtx_generador_ids;

struct WaitingQueue {
    std::queue<Paquete> estanteria_alta;
    std::queue<Paquete> estanteria_baja;
    std::mutex mtx;
    std::condition_variable cv_paquetes;

    WaitingQueue() = default;

    void insertar_paquete(const Paquete& p);
    Paquete extraer_paquete(std::atomic<bool>& sistema_activo);
    bool esta_vacia();
    void notificar_cierre();
};

void productor_operario(WaitingQueue& waiting_queue, std::atomic<bool>& sistema_activo, int modo_prueba, int cantidad_maxima);

#endif // WAITINGQUEUE_H_INCLUDED
