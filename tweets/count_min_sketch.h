#include <stdio.h>
#include<string.h>
#include<stdlib.h>


//structura de count_min_sketch
typedef struct count_min sketch_t;


sketch_t* crear_sketch();

void sumar_a_count(sketch_t* count_sketch , char*  palabra);
unsigned int obtener_minima_aparicion(sketch_t* sketch,char* clave );
void destruir_sketch(sketch_t* sketch);
