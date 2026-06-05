#include "ProcessingQueue.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <atomic>

extern std::atomic<bool> despachador_activo;
extern std::mutex coutMutex;

long long espera_prioridad_0 = 0;
long long espera_prioridad_1 = 0;

int cantidad_prioridad_0 = 0;
int cantidad_prioridad_1 = 0;
int paquetes_procesados = 0;
std::mutex mtx_metricas;

bool ProcessingQueue::esta_vacia() {
    std::lock_guard<std::mutex> lock(acceso_cola);
    return cinta_transportadora.empty();
}

void consumir_paquete(ProcessingQueue& cola) {
    while (despachador_activo.load() || !cola.esta_vacia()) {
        std::unique_lock<std::mutex> lock(cola.acceso_cola);

        while (cola.cinta_transportadora.empty() && despachador_activo.load()) {
            cola.cv_consumidores.wait(lock);
        }

        if (cola.cinta_transportadora.empty() && !despachador_activo.load()) {
            return;
        }

        auto ahora = std::chrono::steady_clock::now();
        auto tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(
            ahora - cola.cinta_transportadora.front().hora_ingreso
        );
        auto tiempo_minimo = std::chrono::milliseconds(550);

        if (tiempo_pasado < tiempo_minimo) {
            cola.cv_consumidores.wait_for(lock, tiempo_minimo - tiempo_pasado);
            continue;
        }

        PaqueteEnProcesamiento paquete = cola.cinta_transportadora.front();
        cola.cinta_transportadora.pop();
        cola.cv_productores.notify_one();
        lock.unlock();

        auto tiempo_espera_total = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - paquete.datos.fecha_de_creacion
        ).count();

        {
            std::lock_guard<std::mutex> lock_metricas(mtx_metricas);

            if (paquete.datos.nivel_de_prioridad == 0) {
                espera_prioridad_0 += tiempo_espera_total;
                cantidad_prioridad_0++;
            } else {
                espera_prioridad_1 += tiempo_espera_total;
                cantidad_prioridad_1++;
            }

            paquetes_procesados++;
        }

        {
            std::lock_guard<std::mutex> lock_cout(coutMutex);
            std::cout << "Paquete " << paquete.datos.identificador_unico
                      << " procesado pasados " << tiempo_pasado.count()
                      << " ms en la cinta." << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(270));
    }
}
