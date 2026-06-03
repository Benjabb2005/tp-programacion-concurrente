#include "Evaluador.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include "WaitingQueue.h"

using namespace std;

void ejecutarEscenario(const char* nombre, int cantidadProductores, int cantidadConsumidores, int cantidadPaquetes){
    cout << "\n==============================" << endl;
    cout << "Escenario: " << nombre << endl;
    cout << "Productores: " << cantidadProductores << endl;
    cout << "Consumidores: " << cantidadConsumidores << endl;
    cout << "Paquetes: " << cantidadPaquetes << endl;
    cout << "==============================" << endl;

    auto inicio = chrono::high_resolution_clock::now(); //guarda cuando empieza

    //reseteo el contador
    mtx_contador_global.lock();
    contador_global_paquetes_generados = 0;
    mtx_contador_global.unlock();

    // se crean hilos
    vector<thread> productores;
    vector<thread> consumidores;
    // esto es para los parametros del producor y luego poder detenerlo
    WaitingQueue waitingQueue;
    bool sistema_activo = true;
    // parametro consumidor
    ProcessingQueue processingQueue;

    for(int i =0; i < cantidadProductores; i++){  // recorre la cantidad de productores y lo agrega al vector
        productores.emplace_back(productor_operario, std::ref(waitingQueue), std::ref(sistema_activo)); // agrega en el vector el hilo std::thread t1(productor_operario, i, std::ref(waitingQueue), std::ref(sistema_activo))
    }
    for(int i =0; i < cantidadConsumidores; i++){
        consumidores.emplace_back(consumir_paquete, std::ref(processingQueue));
    }
    while(contador_global_paquetes_generados < cantidadPaquetes){   // espera a q el productor llegue a la cantidad de paquetes
        this_thread::sleep_for(chrono::milliseconds(10));
    }

    sistema_activo = false; // una vez llego a la cantidad de paquetes le pongo false para que termine

    for(auto& t: productores){  //recorre el vector y hace el join de cada uno
        t.join();
    }
    for(auto& t: consumidores){
        t.join();
    }

    auto fin = chrono::high_resolution_clock::now(); // guarda cuando termina

    auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio); // guarda su duracion

    cout << "Tiempo total: " << duracion.count() << " ms" << endl;
};

//
void pruebaCargaMasiva(){
    ejecutarEscenario("Carga masiva A", 1, 2, 1550);
    ejecutarEscenario("Carga masiva B", 3, 1, 1550);
    ejecutarEscenario("Carga masiva C", 3, 3, 1550);
};

//
void pruebaVacuidad(){
    ejecutarEscenario("Vacuidad A", 1, 2, 0);
    ejecutarEscenario("Vacuidad B", 3, 1, 0);
    ejecutarEscenario("Vacuidad C", 3, 3, 0);
};

//
void pruebaSaturacion(){
    ejecutarEscenario("Saturacion A", 1, 2, 8);
    ejecutarEscenario("Saturacion B", 3, 1, 8);
    ejecutarEscenario("Saturacion C", 3, 3, 8);
};

//
void pruebaEquidad(){
    ejecutarEscenario("Equidad A", 1, 2, 0);
    ejecutarEscenario("Equidad B", 3, 1, 0);
    ejecutarEscenario("Equidad C", 3, 3, 0);
};
