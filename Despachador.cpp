#include "Despachador.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>

// Variables globales deifnidas en main
//extern bool sistema_activo;  // bandera para detener el sistema
extern std::mutex coutMutex; // mutx para proteger la salida de consola

void despachador(WaitingQueue& waiting_queue, ProcessingQueue& processing_queue, int cantidadPaquetes) {
    int producciones = 0;

    while (producciones < cantidadPaquetes) {
        // Extraer paquete
        Paquete p = waiting_queue.extraer_paquete();
        // Si la cola esta vacia, espera un poco y reintenta
        if (p.identificador_unico == -1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        // Prepara paquete para la cinta
        PaqueteEnProcesamiento paquete_cinta;
        paquete_cinta.datos = p;
        paquete_cinta.hora_ingreso = std::chrono::steady_clock::now();
        {
        // Inserta paquete
        std::unique_lock<std::mutex> lock(processing_queue.acceso_cola);

        // Si la cinta esta llena(maximo de 5), esperar espacio
        
        while (processing_queue.cinta_transportadora.size() < 5) {
            processing_queue.cv_productores.wait(lock);
        }

        processing_queue.cinta_transportadora.push(paquete_cinta);
        producciones++;
        processing_queue.cv_consumidores.notify_one();
        }
        // Retardo obligatorio entre asignaciones
        std::this_thread::sleep_for(std::chrono::milliseconds(420));

        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::cout << ">> Paquete " << p.identificador_unico
                      << " asignado a cinta (prioridad " << p.nivel_de_prioridad << ")\n";
        }

        // Retardo obligatorio de 420ms entre asignaciones
        std::this_thread::sleep_for(std::chrono::milliseconds(420));
    }
//NOTIFICAR DESPACHO TERMINADO
{
    std::unique_lock<std::mutex> lock(processing_queue.acceso_cola);
        processing_queue.despacho_terminado = true;
}
processing_queue.cv_consumidores.notify_all();

    std::cout << "Despachador: " << cantidadPaquetes << " paquetes enviados a la cinta.\n";
}
