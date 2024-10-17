/* 
 * Nombre del equipo: S.O. AGREVAL
 * Fecha: 16/10/2024
 * Versión: 1.1.2
 * Descripción:
 * Este programa implementa la gestión de memoria mediante el algoritmo **LRU (Least Recently Used)**.
 * LRU reemplaza la página menos recientemente usada cuando la memoria física está llena, para optimizar
 * el rendimiento del sistema. El programa utiliza una lista doblemente enlazada para facilitar el 
 * seguimiento del uso de las páginas. Cada vez que una página se accede, se mueve al frente de la lista 
 * para marcarla como la más recientemente usada. Si la lista está llena, se elimina el frame menos 
 * recientemente utilizado (el último en la lista). 
 * 
 * El estado actual de la memoria se imprime en cada paso para depuración.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUM_FRAMES 4   // Número de frames disponibles en memoria física
#define NUM_PAGES 10   // Número total de páginas virtuales

// Estructura para un frame en memoria física
typedef struct Frame {
    int page;           // Número de la página almacenada (-1 si está vacío)
    bool valid;         // Indica si el frame está ocupado (true) o vacío (false)
    struct Frame *prev; // Puntero al frame anterior (para lista doblemente enlazada)
    struct Frame *next; // Puntero al frame siguiente (para lista doblemente enlazada)
} Frame;

// Estructura para administrar la lista de frames en memoria física
typedef struct FrameList {
    int numFrames;      // Número de frames ocupados actualmente
    Frame *head;        // Puntero al primer frame (más recientemente usado)
    Frame *tail;        // Puntero al último frame (menos recientemente usado)
} FrameList;

/*
 * Función: createFrame
 * Descripción: Crea e inicializa un nuevo frame vacío.
 * Retorna: Puntero al frame creado.
 */
Frame* createFrame() {
    Frame *frame = (Frame *)malloc(sizeof(Frame));
    if (frame != NULL) {
        frame->page = -1;
        frame->valid = false;
        frame->prev = NULL;
        frame->next = NULL;
    }
    return frame;
}

/*
 * Función: createFrameList
 * Descripción: Inicializa una lista vacía de frames.
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
 * Descripción: Inserta un frame al frente de la lista.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - frame: Puntero al frame que se va a insertar.
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
 * Función: moveToHead
 * Descripción: Mueve un frame al frente de la lista, marcándolo como el más recientemente usado.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - frame: Puntero al frame que se va a mover.
 */
void moveToHead(FrameList *frameList, Frame *frame) {
    if (frameList->head == frame) {
        return; // Ya está al frente
    }

    // Desconectar el frame de su posición actual
    if (frame->prev != NULL) {
        frame->prev->next = frame->next;
    }
    if (frame->next != NULL) {
        frame->next->prev = frame->prev;
    }
    if (frameList->tail == frame) {
        frameList->tail = frame->prev;
    }

    // Mover el frame al frente de la lista
    frame->next = frameList->head;
    frame->prev = NULL;
    if (frameList->head != NULL) {
        frameList->head->prev = frame;
    }
    frameList->head = frame;
    if (frameList->tail == NULL) {
        frameList->tail = frame;
    }
}

/*
 * Función: removeFrame
 * Descripción: Elimina el frame menos recientemente usado (tail) de la lista.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - frame: Puntero al frame que se va a eliminar.
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
    free(frame); // Liberar la memoria del frame eliminado
}

/*
 * Función: findFrame
 * Descripción: Busca un frame en la lista por su número de página.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - page: Número de la página que se busca.
 * Retorna: Puntero al frame encontrado, o NULL si no está en la lista.
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
 * Descripción: Carga una página en memoria utilizando el algoritmo LRU.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - page: Número de la página a cargar.
 */
void loadPage(FrameList *frameList, int page) {
    Frame *frame = findFrame(frameList, page);
    if (frame != NULL) {
        moveToHead(frameList, frame);  // Mover al frente si ya está en memoria
    } else {
        frame = createFrame();
        frame->page = page;
        frame->valid = true;

        // Eliminar el frame menos recientemente usado si la lista está llena
        if (frameList->numFrames == NUM_FRAMES) {
            Frame *lruFrame = frameList->tail;
            removeFrame(frameList, lruFrame);
        }
        insertFrame(frameList, frame);  // Insertar el nuevo frame al frente
    }
}

/*
 * Función: printFrameList
 * Descripción: Imprime el estado actual de la lista de frames para depuración.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 */
void printFrameList(FrameList *frameList) {
    printf("Estado actual de los frames:\n");
    Frame *current = frameList->head;
    while (current != NULL) {
        printf("Página: %d, Estado: %s\n", current->page,
               current->valid ? "Ocupado" : "Vacío");
        current = current->next;
    }
    printf("\n");
}

/*
 * Función: main
 * Descripción: Función principal que simula la carga de páginas en memoria usando LRU.
 */
int main() {
    FrameList *frameList = createFrameList();

    // Simular la carga de páginas en memoria
    loadPage(frameList, 1);
    loadPage(frameList, 2);
    loadPage(frameList, 3);
    loadPage(frameList, 4);
    printFrameList(frameList);  // Imprimir el estado actual de los frames

    // Intentar cargar otra página con todos los frames ocupados
    loadPage(frameList, 5);
    printFrameList(frameList);  // Imprimir el estado después de la sustitución

    // Liberar la memoria utilizada por la lista de frames
    free(frameList);

    return 0;
}
