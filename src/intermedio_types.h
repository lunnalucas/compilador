#ifndef INTERMEDIO_TYPES_H
#define INTERMEDIO_TYPES_H

typedef struct {
    int tipo;
    int posicion;
} Expresion;

typedef struct {
    char nombre[64];
    int tipo;
    int posicion;
    int bf;
    char decremento[64];
} Control;

typedef struct {
    char nombres[16][64];
    int cantidad;
} ListaIds;

typedef struct {
    int bf;
    int bi;
} SaltosSeleccion;

#endif
