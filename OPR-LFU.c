/* 
 * Nombre del equipo: S.O. AGREVAL
 * Fecha: 16/10/2024
 * Versión: 1.1.2
 * Descripción:
 * Este programa implementa la gestión de páginas en memoria física utilizando el **algoritmo LFU (Least Frequently Used)**.
 * El algoritmo LFU reemplaza la página que ha sido utilizada con menor frecuencia cuando la memoria física está llena.
 * Para lograr esto, cada frame (marco) tiene un contador de frecuencia que se incrementa cada vez que la página es utilizada.
 * Si es necesario reemplazar una página, se selecciona el frame con la frecuencia más baja.
 * 
 * La implementación utiliza una lista doblemente enlazada para mantener los frames en memoria. Cada vez que una página es 
 * accedida o reemplazada, se imprime el estado de la memoria para fines de depuración.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_FRAMES 4   // Número de frames disponibles en memoria física
#define NUM_PAGES 10   // Número total de páginas virtuales

// Estructura que representa un frame en memoria física
typedef struct Frame {
    int page;           // Número de la página almacenada (-1 si está vacío)
    bool valid;         // Indica si el frame está ocupado (true) o vacío (false)
    int frequency;      // Contador de frecuencia de uso
    struct Frame *prev; // Puntero al frame anterior (lista doblemente enlazada)
    struct Frame *next; // Puntero al frame siguiente (lista doblemente enlazada)
} Frame;

// Estructura para administrar la lista de frames en memoria física
typedef struct FrameList {
    int numFrames;      // Número de frames ocupados actualmente
    Frame *head;        // Primer frame de la lista (más reciente)
    Frame *tail;        // Último frame de la lista (menos reciente)
} FrameList;

/*
 * Función: createFrame
 * Descripción: Crea e inicializa un frame vacío.
 * Retorna: Puntero al frame creado.
 */
Frame* createFrame() {
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    if (frame != NULL) {
        frame->page = -1;
        frame->valid = false;
        frame->frequency = 0;
        frame->prev = NULL;
        frame->next = NULL;
    }
    return frame;
}

/*
 * Función: createFrameList
 * Descripción: Inicializa una lista vacía de frames en memoria física.
 * Retorna: Puntero a la lista creada.
 */
FrameList* createFrameList() {
    FrameList *frameList = (FrameList *)malloc(sizeof(FrameList));
    if (frameList != NULL) {
        frameList->numFrames = 0;
        frameList->head = NULL;
        frameList->tail = NULL;
    }
    return frameList;
}

/*
 * Función: insertFrame
 * Descripción: Inserta un frame al frente de la lista de frames.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - frame: Puntero al frame a insertar.
 */
void insertFrame(FrameList *frameList, Frame *frame) {
    if (frameList->head == NULL) {
        frameList->head = frame;
        frameList->tail = frame;
    } else {
        frame->next = frameList->head;
        frameList->head->prev = frame;
        frameList->head = frame;
    }
    frameList->numFrames++;
}

/*
 * Función: removeFrame
 * Descripción: Elimina un frame específico de la lista.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - frame: Puntero al frame a eliminar.
 */
void removeFrame(FrameList *frameList, Frame *frame) {
    if (frame->prev != NULL) {
        frame->prev->next = frame->next;
    } else {
        frameList->head = frame->next;
    }
    if (frame->next != NULL) {
        frame->next->prev = frame->prev;
    } else {
        frameList->tail = frame->prev;
    }
    frameList->numFrames--;
    free(frame);  // Liberar la memoria del frame eliminado
}

/*
 * Función: findFrame
 * Descripción: Busca un frame por número de página.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - page: Número de la página a buscar.
 * Retorna: Puntero al frame encontrado o NULL si no está.
 */
Frame* findFrame(FrameList *frameList, int page) {
    Frame *current = frameList->head;
    while (current != NULL) {
        if (current->page == page) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/*
 * Función: loadPage
 * Descripción: Carga una página en memoria física utilizando el algoritmo LFU.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - page: Número de la página a cargar.
 */
void loadPage(FrameList *frameList, int page) {
    Frame *frame = findFrame(frameList, page);
    if (frame != NULL) {
        frame->frequency++;  // Incrementar la frecuencia si la página ya está en memoria
    } else {
        frame = createFrame();
        frame->page = page;
        frame->valid = true;
        frame->frequency = 1;

        if (frameList->numFrames == NUM_FRAMES) {
            // Encontrar el frame con menor frecuencia de uso
            Frame *lfuFrame = frameList->head;
            Frame *current = frameList->head;
            while (current != NULL) {
                if (current->frequency < lfuFrame->frequency) {
                    lfuFrame = current;
                }
                current = current->next;
            }
            removeFrame(frameList, lfuFrame);  // Eliminar el frame LFU
        }
        insertFrame(frameList, frame);  // Insertar el nuevo frame al frente
    }
}

/*
 * Función: printFrameList
 * Descripción: Imprime el estado actual de los frames en memoria para depuración.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 */
void printFrameList(FrameList *frameList) {
    printf("Estado actual de la lista de frames:\n");
    Frame *current = frameList->head;
    while (current != NULL) {
        printf("Página: %d, Frecuencia: %d, Estado: %s\n", 
               current->page, current->frequency, 
               current->valid ? "Ocupado" : "Vacío");
        current = current->next;
    }
    printf("\n");
}

/*
 * Función: main
 * Descripción: Función principal que simula la carga de páginas utilizando LFU.
 */
int main() {
    FrameList *frameList = createFrameList();  // Inicializar la lista de frames

    int pageAccesses[NUM_PAGES] = {1, 2, 3, 4, 5, 1, 2, 1, 3, 4};  // Accesos simulados

    for (int i = 0; i < NUM_PAGES; ++i) {
        loadPage(frameList, pageAccesses[i]);  // Cargar páginas según los accesos
        printFrameList(frameList);  // Imprimir estado tras cada carga
    }

    free(frameList);  // Liberar la memoria utilizada

    return 0;
}
