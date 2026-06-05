#include "Despachador.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

extern std::atomic<bool> sistema_activo;
extern std::mutex coutMutex;

void despachador(WaitingQueue& waiting_queue, ProcessingQueue& processing_queue) {
    while (sistema_activo.load() || !waiting_queue.esta_vacia()) {
        Paquete p = waiting_queue.extraer_paquete(sistema_activo);

        if (p.identificador_unico == -1) {
            continue;
        }

        PaqueteEnProcesamiento paquete_cinta;
        paquete_cinta.datos = p;
        paquete_cinta.hora_ingreso = std::chrono::steady_clock::now();

        std::unique_lock<std::mutex> lock(processing_queue.acceso_cola);

        while (processing_queue.cinta_transportadora.size() >= 5) {
            processing_queue.cv_productores.wait(lock);
        }

        processing_queue.cinta_transportadora.push(paquete_cinta);
        processing_queue.cv_consumidores.notify_one();
        lock.unlock();

        {
            std::lock_guard<std::mutex> lock_cout(coutMutex);
            std::cout << ">> Paquete " << p.identificador_unico
                      << " asignado a cinta (prioridad " << p.nivel_de_prioridad << ")\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(420));
    }
}
