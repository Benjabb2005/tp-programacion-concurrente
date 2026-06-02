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
    estanteria.push(p);
    mtx.unlock();
}

Paquete WaitingQueue::extraer_paquete() {
    //Creamos paquete vacio por default
    Paquete p = {-1, -1, std::chrono::steady_clock::now()};

    mtx.lock();

    //Zona critica, si no esta vacio
    if (!estanteria.empty()) {
        p = estanteria.front(); //Copia el primero
        estanteria.pop();       //Y lo saca de la estanteria
    }

    mtx.unlock();


    return p;
}
bool WaitingQueue::esta_vacia() {
    mtx.lock();
    bool vacia = estanteria.empty();
    mtx.unlock();
    return vacia;
}

void productor_operario(int id_operario, WaitingQueue& waiting_queue, bool& sistema_activo) {
    while (sistema_activo) { //Si esta activo
        Paquete p;//creamos el paquete

        mtx_generador_ids.lock();
        p.identificador_unico = generador_global_ids; //le asigna el identificador
        generador_global_ids++; // y suma 1 para el proximo paquete
        mtx_generador_ids.unlock();

        p.nivel_de_prioridad = rand() % 2; //Nivel de prioridad aleatorio
        p.fecha_de_creacion = std::chrono::steady_clock::now();//le asignamos fecha actual

        std::this_thread::sleep_for(std::chrono::milliseconds(90));//delay

        waiting_queue.insertar_paquete(p);

        mtx_contador_global.lock();
        contador_global_paquetes_generados++;
        mtx_contador_global.unlock();
    }
}
