#include "Evaluador.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include "WaitingQueue.h"
#include "ProcessingQueue.h"
#include "Despachador.h"
#include <mutex>

using namespace std;
bool sistema_activo = true;
std::mutex coutMutex;

std::mutex sistemaMtx;

void ejecutarEscenario(const char* nombre, int cantidadProductores, int cantidadConsumidores, int cantidadPaquetes, int modo_prueba){
    cout << "\n==============================" << endl;
    cout << "Escenario: " << nombre << endl;
    cout << "Productores: " << cantidadProductores << endl;
    cout << "Consumidores: " << cantidadConsumidores << endl;
    cout << "Paquetes: " << cantidadPaquetes << endl;
    cout << "==============================" << endl;

    auto inicio = chrono::high_resolution_clock::now(); //guarda cuando empieza
{
    //reseteo el contador
    std::lock_guard<std::mutex> lk(mtx_contador_global);
    contador_global_paquetes_generados = 0;
}
{
    //reseteo ids
    std::lock_guard<std::mutex> lk(mtx_generador_ids);
    generador_global_ids = 1;
}
{
    std::lock_guard<std::mutex> lk(mtx_metricas);
    espera_prioridad_0 = 0;
    espera_prioridad_1 = 0;
    cantidad_prioridad_0 = 0;
    cantidad_prioridad_1 = 0;
}
    if (cantidadPaquetes == 0) {

        cout << "No se generaron paquetes." << endl;

        auto fin = chrono::high_resolution_clock::now();
        auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio);

        cout << "Tiempo total: " << duracion.count() << " ms" << endl;
        return;
    }
    // se crean hilos
    vector<thread> productores;
    vector<thread> consumidores;
    // esto es para los parametros del productor y luego poder detenerlo
    WaitingQueue waitingQueue;
    sistemaMtx.lock();
    sistema_activo = true;
    sistemaMtx.unlock();
    // parametro consumidor
    ProcessingQueue processingQueue;



    std::thread hiloDespachador(despachador, std::ref(waitingQueue), std::ref(processingQueue), cantidadPaquetes);

    for(int i =0; i < cantidadProductores; i++){  // recorre la cantidad de productores y lo agrega al vector
        productores.emplace_back(productor_operario, std::ref(waitingQueue), std::ref(sistema_activo), modo_prueba, cantidadPaquetes); // agrega en el vector el hilo std::thread t1(productor_operario, i, std::ref(waitingQueue), std::ref(sistema_activo))
    }
    for(int i =0; i < cantidadConsumidores; i++){
        consumidores.emplace_back(consumir_paquete, std::ref(processingQueue));
    }

    //DETENER SISTEMA CUANDO LA CANTIDAD ES IGUAL A LOS PAQUETES PEDIDOS
    while (true) {
        {
        //Mutex para leer el contador global
        std::lock_guard<std::mutex> lk(mtx_contador_global);
        if (contador_global_paquetes_generados >= cantidadPaquetes) break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    sistemaMtx.lock();
    sistema_activo = false; // una vez llego a la cantidad de paquetes le pongo false para que termine
    sistemaMtx.unlock();

    for(auto& t: productores){  //recorre el vector y hace el join de cada uno
        t.join();
    }

    hiloDespachador.join();

    for(auto& t: consumidores){
        t.join();
    }


    auto fin = chrono::high_resolution_clock::now(); // guarda cuando termina

    auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio); // guarda su duracion

    cout << "Total paquetes producidos: " << contador_global_paquetes_generados << endl;

    if(cantidad_prioridad_0 > 0){
        cout << "Promedio espera prioridad 0: "
             << espera_prioridad_0 / cantidad_prioridad_0
             << " ms" << endl;
    } else {
        cout << "Promedio espera prioridad 0: sin paquetes" << endl;
    }

    if(cantidad_prioridad_1 > 0){
        cout << "Promedio espera prioridad 1: "
             << espera_prioridad_1 / cantidad_prioridad_1
             << " ms" << endl;
    } else {
        cout << "Promedio espera prioridad 1: sin paquetes" << endl;
    }

    cout << "Tiempo total: " << duracion.count() << " ms" << endl;
};

//
void pruebaCargaMasiva(){
    ejecutarEscenario("Carga masiva A", 1, 2, 1550, 0);
    ejecutarEscenario("Carga masiva B", 3, 1, 1550, 0);
    ejecutarEscenario("Carga masiva C", 3, 3, 1550, 0);
};

//
void pruebaVacuidad(){
    ejecutarEscenario("Vacuidad A", 1, 2, 0, 0);
    ejecutarEscenario("Vacuidad B", 3, 1, 0, 0);
    ejecutarEscenario("Vacuidad C", 3, 3, 0, 0);
};

//
void pruebaSaturacion(){
    ejecutarEscenario("Saturacion A", 1, 2, 8, 1);
    ejecutarEscenario("Saturacion B", 3, 1, 8, 1);
    ejecutarEscenario("Saturacion C", 3, 3, 8, 1);
};

//
void pruebaEquidad(){
    ejecutarEscenario("Equidad A", 1, 2, 20, 3);
    ejecutarEscenario("Equidad B", 3, 1, 20, 3);
    ejecutarEscenario("Equidad C", 3, 3, 20, 3);
};
