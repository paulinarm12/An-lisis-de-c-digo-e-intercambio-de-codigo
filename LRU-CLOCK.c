/* 
 * Nombre del equipo: S.O. AGREVAL
 * Fecha: 16/10/2024
 * Versión: 1.2.1 
 * Descripción:
 * Este programa implementa la gestión de páginas en memoria física mediante el **algoritmo de reemplazo Clock**.
 * El objetivo del algoritmo Clock es reemplazar una página cuando todas las páginas disponibles en la memoria 
 * están ocupadas. Se usa un bit de referencia para cada frame que indica si la página ha sido recientemente utilizada.
 * El puntero del reloj (clock hand) recorre los frames y decide cuál página reemplazar según los bits de referencia.
 * Este algoritmo es una optimización del algoritmo FIFO, proporcionando un reemplazo más eficiente.
 * 
 * Cada vez que se accede a una página, su bit de referencia se actualiza. Si es necesario reemplazar una página, 
 * el algoritmo busca un frame con bit de referencia en 0 para realizar la sustitución.
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
    bool reference;     // Bit de referencia para el algoritmo Clock
} Frame;

// Estructura para administrar la lista de frames en memoria física
typedef struct FrameList {
    int numFrames;      // Número de frames actualmente ocupados
    Frame frames[NUM_FRAMES]; // Array que almacena los frames
    int clockHand;      // Puntero del reloj (clock hand)
} FrameList;

/*
 * Función: createFrameList
 * Descripción: Inicializa la lista de frames en memoria física.
 * Retorna: Puntero a la lista creada.
 */
FrameList* createFrameList() {
    FrameList *frameList = (FrameList *)malloc(sizeof(FrameList));
    if (frameList != NULL) {
        frameList->numFrames = 0;
        frameList->clockHand = 0;  // Inicializar el puntero del reloj en 0
        for (int i = 0; i < NUM_FRAMES; i++) {
            frameList->frames[i].page = -1;
            frameList->frames[i].valid = false;
            frameList->frames[i].reference = false;
        }
    }
    return frameList;
}

/*
 * Función: findFrame
 * Descripción: Busca un frame en la lista por su número de página.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - page: Número de la página que se busca.
 * Retorna: Índice del frame encontrado o -1 si no está en la lista.
 */
int findFrame(FrameList *frameList, int page) {
    for (int i = 0; i < NUM_FRAMES; i++) {
        if (frameList->frames[i].page == page) {
            return i;
        }
    }
    return -1;
}

/*
 * Función: loadPage
 * Descripción: Carga una página en memoria utilizando el algoritmo Clock.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 *  - page: Número de la página a cargar.
 */
void loadPage(FrameList *frameList, int page) {
    int frameIndex = findFrame(frameList, page);

    if (frameIndex != -1) {
        // La página ya está en memoria, actualizar el bit de referencia
        frameList->frames[frameIndex].reference = true;
    } else {
        // La página no está en memoria, buscar un frame para reemplazar
        while (true) {
            if (!frameList->frames[frameList->clockHand].valid || 
                !frameList->frames[frameList->clockHand].reference) {
                // Encontrar un frame vacío o con bit de referencia en 0
                frameList->frames[frameList->clockHand].page = page;
                frameList->frames[frameList->clockHand].valid = true;
                frameList->frames[frameList->clockHand].reference = true;
                frameList->clockHand = (frameList->clockHand + 1) % NUM_FRAMES;
                break;
            } else {
                // Poner el bit de referencia en 0 y avanzar el puntero del reloj
                frameList->frames[frameList->clockHand].reference = false;
                frameList->clockHand = (frameList->clockHand + 1) % NUM_FRAMES;
            }
        }
    }
}

/*
 * Función: printFrameList
 * Descripción: Imprime el estado actual de los frames en memoria.
 * Parámetros:
 *  - frameList: Puntero a la lista de frames.
 */
void printFrameList(FrameList *frameList) {
    printf("Estado actual de los frames:\n");
    for (int i = 0; i < NUM_FRAMES; i++) {
        printf("Frame %d - Página: %d, Estado: %s, Referencia: %d\n", 
               i, frameList->frames[i].page,
               frameList->frames[i].valid ? "Ocupado" : "Vacío", 
               frameList->frames[i].reference);
    }
    printf("\n");
}

/*
 * Función: main
 * Descripción: Función principal que simula la carga de páginas en memoria utilizando el algoritmo Clock.
 */
int main() {
    FrameList *frameList = createFrameList();  // Inicializar la lista de frames

    // Cargar páginas en memoria
    loadPage(frameList, 1);
    loadPage(frameList, 2);
    loadPage(frameList, 3);
    loadPage(frameList, 4);
    printFrameList(frameList);  // Imprimir estado actual de los frames

    // Intentar cargar otra página con todos los frames ocupados
    loadPage(frameList, 5);
    printFrameList(frameList);  // Imprimir estado tras la sustitución

    // Liberar la memoria utilizada
    free(frameList);

    return 0;
}
