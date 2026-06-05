#ifndef DESPACHADOR_H_INCLUDED
#define DESPACHADOR_H_INCLUDED


#include "WaitingQueue.h"
#include "ProcessingQueue.h"

void despachador(WaitingQueue& waiting_queue, ProcessingQueue& processing_queue);

#endif // DESPACHADOR_H_INCLUDED
