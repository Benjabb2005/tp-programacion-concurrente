#include "WaitingQueue.h"
#include <thread>
#include <chrono>
#include <cstdlib>

int contador_global_paquetes_generados = 0;
std::mutex mtx_contador_global;

int generador_global_ids = 1;
std::mutex mtx_generador_ids;

void WaitingQueue::insertar_paquete(const Paquete& p) {
    mtx.lock();
    // Guardamos el paquete en la cola correspondiente a su prioridad
    if (p.nivel_de_prioridad == 1) {
        estanteria_alta.push(p);
    } else {
        estanteria_baja.push(p);
    }

    mtx.unlock();
}

Paquete WaitingQueue::extraer_paquete() {
    //Creamos paquete vacio por default
    Paquete p = {-1, -1, std::chrono::steady_clock::now()};

    mtx.lock();

    bool forzar_baja_por_starvation = false;


    if (!estanteria_baja.empty()) {
        auto ahora = std::chrono::steady_clock::now(); //Tiempo actual
        // Calculamos la diferencia en milisegundos entre ahora y cuando se creó la caja
        auto tiempo_esperando = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - estanteria_baja.front().fecha_de_creacion).count();

        // Si lleva esperando 6000ms o más, activamos el booleano para que sea prioridad
        if (tiempo_esperando >= 6000) {
            forzar_baja_por_starvation = true;
        }
    }
    //Si se tuvo que activar sale primero el de baja
    if (forzar_baja_por_starvation) {
        p = estanteria_baja.front();
        estanteria_baja.pop();
    }
    // Si no se activo sigue normal priorizando alta
    else if (!estanteria_alta.empty()) {
        p = estanteria_alta.front();
        estanteria_alta.pop();
    }
    // Y si no hay alta vamos con la baja
    else if (!estanteria_baja.empty()) {
        p = estanteria_baja.front();
        estanteria_baja.pop();
    }

    mtx.unlock();


    return p;
}
bool WaitingQueue::esta_vacia() {
    mtx.lock();
    bool vacia = estanteria_alta.empty() && estanteria_baja.empty();
    mtx.unlock();
    return vacia;
}

void productor_operario(WaitingQueue& waiting_queue, bool& sistema_activo, int modo_prueba) {
    while (sistema_activo) { //Si esta activo
        Paquete p;//creamos el paquete

        mtx_generador_ids.lock();
        p.identificador_unico = generador_global_ids; //le asigna el identificador
        generador_global_ids++; // y suma 1 para el proximo paquete
        mtx_generador_ids.unlock();

        //Control segun el test a probar
        if (modo_prueba == 0) {
            p.nivel_de_prioridad = rand() % 2; // Aleatorio
        } else if (modo_prueba == 1) {
            p.nivel_de_prioridad = 1; // Forzar Alta
        } else if (modo_prueba == 2) {
            p.nivel_de_prioridad = 0; // Forzar Baja
        } else if (modo_prueba == 3) {
            if(p.identificador_unico == 1) { // Prueba equidad
                p.nivel_de_prioridad = 0;
            } else{
                p.nivel_de_prioridad = 1;
            }
        }

        p.fecha_de_creacion = std::chrono::steady_clock::now();

        std::this_thread::sleep_for(std::chrono::milliseconds(90));

        waiting_queue.insertar_paquete(p);

        mtx_contador_global.lock();
        contador_global_paquetes_generados++;
        mtx_contador_global.unlock();
    }
}
