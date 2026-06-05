#include "Evaluador.h"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>
#include <mutex>
#include <atomic>
#include "WaitingQueue.h"
#include "ProcessingQueue.h"
#include "Despachador.h"

using namespace std;

std::atomic<bool> sistema_activo(true);
std::atomic<bool> despachador_activo(true);
std::mutex coutMutex;

void ejecutarEscenario(const char* nombre, int cantidadProductores, int cantidadConsumidores, int cantidadPaquetes, int modo_prueba) {
    cout << "\n==============================" << endl;
    cout << "Escenario: " << nombre << endl;
    cout << "Productores: " << cantidadProductores << endl;
    cout << "Consumidores: " << cantidadConsumidores << endl;
    cout << "Paquetes: " << cantidadPaquetes << endl;
    cout << "==============================" << endl;

    auto inicio = chrono::high_resolution_clock::now();

    {
        lock_guard<mutex> lock(mtx_contador_global);
        contador_global_paquetes_generados = 0;
    }

    {
        lock_guard<mutex> lock(mtx_generador_ids);
        generador_global_ids = 1;
    }

    {
        lock_guard<mutex> lock(mtx_metricas);
        espera_prioridad_0 = 0;
        espera_prioridad_1 = 0;
        cantidad_prioridad_0 = 0;
        cantidad_prioridad_1 = 0;
        paquetes_procesados = 0;
    }

    WaitingQueue waitingQueue;
    ProcessingQueue processingQueue;
    vector<thread> productores;
    vector<thread> consumidores;

    sistema_activo.store(true);
    despachador_activo.store(true);

    thread hiloDespachador(despachador, ref(waitingQueue), ref(processingQueue));

    for (int i = 0; i < cantidadConsumidores; i++) {
        consumidores.emplace_back(consumir_paquete, ref(processingQueue));
    }

    for (int i = 0; i < cantidadProductores && cantidadPaquetes > 0; i++) {
        productores.emplace_back(productor_operario, ref(waitingQueue), ref(sistema_activo), modo_prueba, cantidadPaquetes);
    }

    if (cantidadPaquetes == 0) {
        this_thread::sleep_for(chrono::milliseconds(500));
        cout << "No se generaron paquetes. Consumidores en espera pasiva." << endl;
    }

    for (auto& t : productores) {
        t.join();
    }

    sistema_activo.store(false);
    waitingQueue.notificar_cierre();

    hiloDespachador.join();

    despachador_activo.store(false);
    processingQueue.cv_consumidores.notify_all();
    processingQueue.cv_productores.notify_all();

    for (auto& t : consumidores) {
        t.join();
    }

    auto fin = chrono::high_resolution_clock::now();
    auto duracion = chrono::duration_cast<chrono::milliseconds>(fin - inicio);

    int total_producidos;
    long long espera_baja;
    long long espera_alta;
    int cantidad_baja;
    int cantidad_alta;
    int total_procesados;

    {
        lock_guard<mutex> lock_contador(mtx_contador_global);
        total_producidos = contador_global_paquetes_generados;
    }

    {
        lock_guard<mutex> lock_metricas(mtx_metricas);
        espera_baja = espera_prioridad_0;
        espera_alta = espera_prioridad_1;
        cantidad_baja = cantidad_prioridad_0;
        cantidad_alta = cantidad_prioridad_1;
        total_procesados = paquetes_procesados;
    }

    cout << "Total paquetes producidos: " << total_producidos << endl;
    cout << "Total paquetes procesados: " << total_procesados << endl;
    cout << "Paquetes prioridad 0 procesados: " << cantidad_baja << endl;
    cout << "Paquetes prioridad 1 procesados: " << cantidad_alta << endl;

    if (cantidad_baja > 0) {
        cout << "Promedio espera prioridad 0: "
             << espera_baja / cantidad_baja
             << " ms" << endl;
    } else {
        cout << "Promedio espera prioridad 0: sin paquetes" << endl;
    }

    if (cantidad_alta > 0) {
        cout << "Promedio espera prioridad 1: "
             << espera_alta / cantidad_alta
             << " ms" << endl;
    } else {
        cout << "Promedio espera prioridad 1: sin paquetes" << endl;
    }

    if (total_producidos == total_procesados) {
        cout << "Validacion: producidos == procesados" << endl;
    } else {
        cout << "Validacion: diferencia entre producidos y procesados" << endl;
    }

    cout << "Tiempo total: " << duracion.count() << " ms" << endl;
}

void pruebaCargaMasiva() {
    ejecutarEscenario("Carga masiva A", 1, 2, 1550, 0);
    ejecutarEscenario("Carga masiva B", 3, 1, 1550, 0);
    ejecutarEscenario("Carga masiva C", 3, 3, 1550, 0);
}

void pruebaVacuidad() {
    ejecutarEscenario("Vacuidad A", 1, 2, 0, 0);
    ejecutarEscenario("Vacuidad B", 3, 1, 0, 0);
    ejecutarEscenario("Vacuidad C", 3, 3, 0, 0);
}

void pruebaSaturacion() {
    ejecutarEscenario("Saturacion A", 1, 2, 8, 1);
    ejecutarEscenario("Saturacion B", 3, 1, 8, 1);
    ejecutarEscenario("Saturacion C", 3, 3, 8, 1);
}

void pruebaEquidad() {
    ejecutarEscenario("Equidad A", 1, 2, 10, 3);
    ejecutarEscenario("Equidad B", 3, 1, 10, 3);
    ejecutarEscenario("Equidad C", 3, 3, 10, 3);
}
