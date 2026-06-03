#include "ProcessingQueue.h"
#include <chrono>
#include <thread>

int consumos = 0;

 void consumir_paquete(ProcessingQueue &cola){
    //ADQUISICION DEL MUTEX
    std::unique_lock<std::mutex> lock(cola.acceso_cola);

    if(cola.cinta_transportadora.empty()){
        cola.cv_consumidores.wait(lock);
    }
    //TOMAR PAQUETE DESPACHADO
    PaqueteEnProcesamiento paquete = cola.cinta_transportadora.front();
    cola.cinta_transportadora.pop();

    //AVISA QUE HAY LUGAR EN LA COLA
    cola.cv_productores.notify_one();
    lock.unlock();

    //PROCESAMIENTO DEL PAQUETE
    auto ahora = std::chrono::steady_clock::now();
    auto tiempo_pasado = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - paquete.hora_ingreso);
    auto tiempo_minimo = std::chrono::milliseconds(550);

    //Si no pasaron 550ms el hilo duerme
    if (tiempo_pasado < tiempo_minimo) {
        std::this_thread::sleep_for(tiempo_minimo - tiempo_pasado);
    }

    //Tiempo de procesamiento
    std::this_thread::sleep_for(std::chrono::milliseconds(270));

    //ADQUIRIR LOCK PARA GUARDAR CONSUMOS
    lock.lock();
    consumos++;
    lock.unlock();
 }
