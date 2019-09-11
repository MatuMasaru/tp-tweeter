#include <stdio.h>
#include"count_min_sketch.h"
#include<string.h>
#include<stdlib.h>
#define TAM_ARREGLO 9973
#define CANTIDAD_VECTORES 4
// donde saque las funciones de hashing.
// http://www2.elo.utfsm.cl/~lsb/elo320/clases/c7.pdf
struct count_min{
    unsigned int** vectores;
};


size_t funcion_hash_1( const char* s){
	size_t hash_val;
	for (hash_val = 0; *s != '\0'; s++){
		hash_val = *s + 31 * hash_val;
	}
    hash_val = hash_val% TAM_ARREGLO;
	return hash_val;
}

//es la funcion djb2
size_t funcion_hash_2(const char *str){
    size_t hash = 5381;
    int c;
    while ((c = *str++)){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    hash = hash%TAM_ARREGLO;
    return hash;
}

// funcion hash Robert  Sedgwicks( funcion RSHash) en  su  libro Algorithms  in  C.
size_t funcion_hash_3(const char* str){
    size_t b    = 378551;
    size_t a    = 63689;
    size_t hash = 0;
    size_t i    = 0;
    for(i = 0; *str; str++, i++){
        hash = hash * a + (*str);
        a    = a * b;
    }
    hash = hash %TAM_ARREGLO;
    return hash;
}

//funcion propuesta por Brian Kernighan y Dennis Ritchie(stringhash)
size_t funcion_hash_4(const char *s){
    size_t h=0;
    for( ; *s; s++ )
        h= 131*h+ *s;
    h = h%TAM_ARREGLO;
    return h;
}
sketch_t* crear_sketch(){
    sketch_t* sketch = malloc (sizeof(sketch_t));
    if(!sketch) return NULL;

    sketch->vectores = malloc(CANTIDAD_VECTORES * sizeof(unsigned int*));
    for (int i = 0; i < CANTIDAD_VECTORES; i++) {
        sketch->vectores[i] = calloc(TAM_ARREGLO, sizeof(unsigned int));
        if (!sketch->vectores[i]) {
            for (int j = 0;  j < i ; j++) {
                free(sketch->vectores[j]) ;
            }
            free(sketch->vectores);
            free(sketch);
            return NULL;
        }
    }
    return sketch;
}
void sumar_a_count(sketch_t* count_sketch , char*  palabra){

    size_t posicion = funcion_hash_1(palabra);
    count_sketch->vectores[0][posicion] += 1;

    posicion = funcion_hash_2(palabra);
    count_sketch->vectores[1][posicion] += 1;

    posicion = funcion_hash_3(palabra);
    count_sketch->vectores[2][posicion] += 1;

    posicion = funcion_hash_4(palabra);
    count_sketch->vectores[3][posicion] += 1;

}
unsigned int obtener_minima_aparicion(sketch_t* sketch,char* clave ){
    size_t posicion =funcion_hash_1(clave);
    //printf("posicion de funcion_hash_1 es :%zu para la palabra %s\n", posicion, clave );
    unsigned int min = sketch->vectores[0][posicion];
    //printf("minimo en el primer vector es %u\n", min );
    posicion = funcion_hash_2(clave);

    if(sketch->vectores[1][posicion]< min )
        min = sketch->vectores[1][posicion];
    //printf("posicion de funcion_hash_2 es :%zu para la palabra %s\n", posicion, clave );
    posicion = funcion_hash_3(clave);

    if (sketch->vectores[2][posicion]< min )
        min = sketch->vectores[2][posicion];
    //printf("posicion de funcion_hash_3 es :%zu para la palabra %s\n", posicion, clave );
    posicion = funcion_hash_4(clave);
    if (sketch->vectores[3][posicion]< min )
        min =sketch->vectores[3][posicion];
    //printf("posicion de funcion_hash_4 es :%zu para la palabra %s\n", posicion, clave );
    //printf("el minimo es %u\n", min );
    return min ;
}
void destruir_sketch(sketch_t* sketch){
    for (int i = 0 ; i < CANTIDAD_VECTORES ; ++i){
        free(sketch->vectores[i]);
    }
    free(sketch->vectores);
    free(sketch);
}
