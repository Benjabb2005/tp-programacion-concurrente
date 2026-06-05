#include "ProcessingQueue.h"
#include <chrono>
#include <thread>
#include <iostream>

int consumos = 0;
extern bool sistema_activo;
extern std::mutex coutMutex;

long long espera_prioridad_0 = 0;
long long espera_prioridad_1 = 0;

int cantidad_prioridad_0 = 0;
int cantidad_prioridad_1 = 0;
std::mutex mtx_metricas;

 void consumir_paquete(ProcessingQueue &cola){
    while(sistema_activo){
    //ADQUISICION DEL MUTEX
    std::unique_lock<std::mutex> lock(cola.acceso_cola);

    while(cola.cinta_transportadora.empty() && sistema_activo){
        cola.cv_consumidores.wait(lock);
    }

    if (cola.cinta_transportadora.empty() && !sistema_activo) {
    return;
}
    //TOMAR PAQUETE DESPACHADO
    //PaqueteEnProcesamiento paquete = cola.cinta_transportadora.front();

    //PROCESAMIENTO DEL PAQUETE
    auto ahora = std::chrono::steady_clock::now();
    auto tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - cola.cinta_transportadora.front().hora_ingreso);
    auto tiempo_minimo = std::chrono::milliseconds(550);

    //Si no pasaron 550ms el hilo duerme
    /*while (tiempo_pasado < tiempo_minimo) {
        // wait_for duerme el hilo, pero SUELTA EL CANDADO para que los productores sigan metiendo cajas
        cola.cv_consumidores.wait_for(lock, tiempo_minimo - tiempo_pasado);

        // Cuando el hilo se despierta, miramos la hora de nuevo a ver si ya pasaron los 550ms
        ahora = std::chrono::steady_clock::now();
        //tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - paquete.hora_ingreso);
        tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - cola.cinta_transportadora.front().hora_ingreso);
    }*/

    if (tiempo_pasado < tiempo_minimo) {
        cola.cv_consumidores.wait_for(lock, tiempo_minimo - tiempo_pasado);
        continue;
    }

    PaqueteEnProcesamiento paquete = cola.cinta_transportadora.front();
    cola.cinta_transportadora.pop();
    auto tiempo_espera_total = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - paquete.datos.fecha_de_creacion).count();

    mtx_metricas.lock();

    if(paquete.datos.nivel_de_prioridad == 0){
        espera_prioridad_0 += tiempo_espera_total;
        cantidad_prioridad_0++;
    } else {
        espera_prioridad_1 += tiempo_espera_total;
        cantidad_prioridad_1++;
    }

    mtx_metricas.unlock();

    std::lock_guard<std::mutex> lock2(coutMutex);
    std::cout << "Paquete " << paquete.datos.identificador_unico << " procesado pasados " << tiempo_pasado.count() << " ms en la cinta." << std::endl;
    consumos++;
    //AVISA QUE HAY LUGAR EN LA COLA
    cola.cv_productores.notify_one();
    lock.unlock();

    //Tiempo de procesamiento
    std::this_thread::sleep_for(std::chrono::milliseconds(270));
}
 }
