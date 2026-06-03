#ifndef SEMAFORO_H_INCLUDED
#define SEMAFORO_H_INCLUDED

#include <mutex>
#include <condition_variable>

class Semaforo {
private:
    int contador;
    std::mutex mtx;
    std::condition_variable cv;

public:
    Semaforo(int valorInicial = 0) : contador(valorInicial) {}

    void wait() {
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [&]{ return contador > 0; });
        contador--;
    }

    void signal() {
        std::lock_guard<std::mutex> lk(mtx);
        contador++;
        cv.notify_one();
    }
};

#endif // SEMAFORO_H_INCLUDED
