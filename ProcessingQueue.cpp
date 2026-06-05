#include "ProcessingQueue.h"
#include <chrono>
#include <thread>
#include <iostream>
int consumos = 0;
extern bool sistema_activo;
extern std::mutex coutMutex;
 void consumir_paquete(ProcessingQueue &cola){
     while(sistema_activo){
    //ADQUISICION DEL MUTEX
    std::unique_lock<std::mutex> lock(cola.acceso_cola);

    if(cola.cinta_transportadora.empty()){
        cola.cv_consumidores.wait(lock);
    }
    //TOMAR PAQUETE DESPACHADO
    PaqueteEnProcesamiento paquete = cola.cinta_transportadora.front();

    //PROCESAMIENTO DEL PAQUETE
    auto ahora = std::chrono::steady_clock::now();
    auto tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - paquete.hora_ingreso);
    auto tiempo_minimo = std::chrono::milliseconds(550);

    //Si no pasaron 550ms el hilo duerme
    while (tiempo_pasado < tiempo_minimo) {
        // wait_for duerme el hilo, pero SUELTA EL CANDADO para que los productores sigan metiendo cajas
        cola.cv_consumidores.wait_for(lock, tiempo_minimo - tiempo_pasado);

        // Cuando el hilo se despierta, miramos la hora de nuevo a ver si ya pasaron los 550ms
        ahora = std::chrono::steady_clock::now();
        tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - paquete.hora_ingreso);
    }

    cola.cinta_transportadora.pop();
    std::lock_guard<std::mutex> lock2(coutMutex);
    std::cout << "Paquete " << paquete.datos.identificador_unico << " procesado pasados " << tiempo_pasado.count() << " segundos en la cinta." << std::endl;
    consumos++;
    //AVISA QUE HAY LUGAR EN LA COLA
    cola.cv_productores.notify_one();
    lock.unlock();

    //Tiempo de procesamiento
    std::this_thread::sleep_for(std::chrono::milliseconds(270));
}
 }
