#include "WaitingQueue.h"
#include <thread>
#include <chrono>
#include <cstdlib>

int contador_global_paquetes_generados = 0;
std::mutex mtx_contador_global;

int generador_global_ids = 1;
std::mutex mtx_generador_ids;

void WaitingQueue::insertar_paquete(const Paquete& p) {
    {
        std::lock_guard<std::mutex> lock(mtx);

        if (p.nivel_de_prioridad == 1) {
            estanteria_alta.push(p);
        } else {
            estanteria_baja.push(p);
        }
    }

    cv_paquetes.notify_one();
}

Paquete WaitingQueue::extraer_paquete(std::atomic<bool>& sistema_activo) {
    Paquete p = {-1, -1, std::chrono::steady_clock::now()};
    std::unique_lock<std::mutex> lock(mtx);

    cv_paquetes.wait(lock, [&]() {
        return !estanteria_alta.empty() || !estanteria_baja.empty() || !sistema_activo.load();
    });

    if (estanteria_alta.empty() && estanteria_baja.empty()) {
        return p;
    }

    bool baja_en_starvation = false;

    if (!estanteria_baja.empty() && !estanteria_alta.empty()) {
        auto ahora = std::chrono::steady_clock::now();
        auto tiempo_esperando = std::chrono::duration_cast<std::chrono::milliseconds>(
            ahora - estanteria_baja.front().fecha_de_creacion
        ).count();

        baja_en_starvation = tiempo_esperando >= 6000;
    }

    if (baja_en_starvation) {
        p = estanteria_baja.front();
        estanteria_baja.pop();
    } else if (!estanteria_alta.empty()) {
        p = estanteria_alta.front();
        estanteria_alta.pop();
    } else {
        p = estanteria_baja.front();
        estanteria_baja.pop();
    }

    return p;
}

bool WaitingQueue::esta_vacia() {
    std::lock_guard<std::mutex> lock(mtx);
    return estanteria_alta.empty() && estanteria_baja.empty();
}

void WaitingQueue::notificar_cierre() {
    cv_paquetes.notify_all();
}

void productor_operario(WaitingQueue& waiting_queue, std::atomic<bool>& sistema_activo, int modo_prueba, int cantidad_maxima) {
    while (sistema_activo.load()) {
        Paquete p;

        {
            std::lock_guard<std::mutex> lock(mtx_generador_ids);

            if (generador_global_ids > cantidad_maxima) {
                break;
            }

            p.identificador_unico = generador_global_ids;
            generador_global_ids++;
        }

        if (modo_prueba == 0) {
            p.nivel_de_prioridad = rand() % 2;
        } else if (modo_prueba == 1) {
            p.nivel_de_prioridad = 1;
        } else if (modo_prueba == 2) {
            p.nivel_de_prioridad = 0;
        } else if (modo_prueba == 3) {
            if (p.identificador_unico == 1) {
                p.nivel_de_prioridad = 0;
            } else {
                p.nivel_de_prioridad = 1;
            }
        } else {
            p.nivel_de_prioridad = 0;
        }

        p.fecha_de_creacion = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(90));
        waiting_queue.insertar_paquete(p);

        {
            std::lock_guard<std::mutex> lock(mtx_contador_global);
            contador_global_paquetes_generados++;
        }
    }
}
