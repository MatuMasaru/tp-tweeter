#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cola.h"
#include "heap.h"
#include "hash.h"
#include "pila.h"
#include "strutil.h"
#include "count_min_sketch.h"
#define ARGUMENTOS_VALIDOS 3

ssize_t getline(char **lineptr, size_t *n, FILE *stream);
/*estruras necesarias para la ejecucion de procesar tweets,
*bajo el tipo de dato TDA_procesar_t.
*/
typedef struct analizar{
    sketch_t* sketch;
    hash_t* hash_apariciones;
    cola_t* cola;
    pila_t* pila;
    heap_t* heap_min;
}TDA_procesar_t;
/*estructura para el tweet y sus apariciones
*/
typedef struct {
    char* palabra_tweet;
    unsigned int apariciones;
}tweets_t;

//valida la cantidad de argumentos necesarios
bool cantidad_argumentos_valido(int cantidad_necesaria){
    bool condicion = (cantidad_necesaria == ARGUMENTOS_VALIDOS)? true : false;
    if (!condicion) fprintf(stderr,"%s","Error: Cantidad erronea de parametros\n");
    return condicion;
}

//validad, si los argumento son solo numeros, en caso que no lo sean devuelven false.
bool validacion_argumentos(char const *argv[], size_t *nlineas,size_t *k_tweets ){
    char* ptrresto;
    *nlineas = strtol( argv[1] , &ptrresto, 0 );
    if(*ptrresto)   return false;
    *k_tweets = strtol(argv[2], &ptrresto, 0 );
    if(*ptrresto)   return false;
    return true;
}

/*de las estructura compara la cantidad de apariciones(si tweet_a < tweet_b devuelve -1),
* en caso de que las hash_apariciones
*sean iguales, va a comparar el tweet de ambos para darle priridad al nombre .
*/
int comparacion(const void* tweet_a,const void* tweet_b ){
    tweets_t* primero = (tweets_t*)tweet_a;
    tweets_t* segundo = (tweets_t*)tweet_b;
    if(primero->apariciones < segundo->apariciones)
        return -1;
    else if (primero->apariciones > segundo->apariciones)
        return 1;

    return (strcmp(primero->palabra_tweet,segundo->palabra_tweet)*-1);
}
/*es la funcion comparacion , pero invertida para que el heap se vuelva de minimos.
*/
int comparacion_inversa(const void* tweet_a,const void* tweet_b){
    return (comparacion(tweet_a,tweet_b)*-1);
}

/*crea todos los TDA necesarios ppara que procesar_tweets funcione.
*En caso que no se pueda devuelve NULL;
*/
TDA_procesar_t* crear_TDAS_procesar(){
    TDA_procesar_t* procesar =malloc(sizeof(TDA_procesar_t));
    if (!procesar)
        return NULL;
    procesar->sketch = crear_sketch();
    if(!procesar->sketch) {
        free(procesar);
        return NULL;
    }
    procesar->hash_apariciones = hash_crear(NULL);
    if (!procesar->hash_apariciones){
        destruir_sketch(procesar->sketch);
        free(procesar);
        return NULL;
    }
    procesar->cola = cola_crear();
    if(!procesar->cola){
        free(procesar->hash_apariciones);
        destruir_sketch(procesar->sketch);
        free(procesar);
        return NULL;
    }
    procesar->pila = pila_crear();
    if(!procesar->pila){
        free(procesar->cola);
        free(procesar->hash_apariciones);
        destruir_sketch(procesar->sketch);
        free(procesar);
        return NULL;
    }
    procesar->heap_min = heap_crear(comparacion_inversa);
    if(!procesar->heap_min){
        free(procesar->pila );      //se que aun no son necesarios
        free(procesar->cola);
        free(procesar->hash_apariciones);
        destruir_sketch(procesar->sketch);
        free(procesar);
        return NULL;
    }
    return procesar;
}
/*pre:todas las estructuras fueron creadas correctamente.
elimina todos los TDA que se usaron  procesar_tweets .
*/
void destruir_TDAS_procesar(TDA_procesar_t* estructura_procesar){
    heap_destruir(estructura_procesar->heap_min, NULL);
    pila_destruir(estructura_procesar->pila);
    cola_destruir(estructura_procesar->cola, NULL);
    hash_destruir(estructura_procesar->hash_apariciones);
    destruir_sketch(estructura_procesar->sketch);
    free(estructura_procesar);

}
/* pre:tdas_programa , los tdas que necesita el programa fueron creados correctamente.
*proceso cada tweet del texto, guardando en el hash y en la cola si es que no esta en el hash.
*/
void procesar_linea(char* texto,TDA_procesar_t* tdas_programa){
    char** tweets = split(texto, ',');
    for (int i = 1; tweets[i] ; i++) {
        sumar_a_count( tdas_programa->sketch , tweets[i]);
        if ( !hash_pertenece( tdas_programa->hash_apariciones, tweets[i] )) {
            hash_guardar(tdas_programa->hash_apariciones, tweets[i], NULL) ;
            cola_encolar(tdas_programa->cola , strdup(tweets[i]) );
        }
    }
    free_strv( tweets );
}

/*pre:el hash fue creado.
*post:se destruyo y se crea el hash nuevamente.
*cada nlineas sucede esta accion, el hash no se llena en lo posible y buscar sera O(1);
*/
void reiniciar_hash(TDA_procesar_t* tdas_programa ){
    hash_destruir(tdas_programa->hash_apariciones);
    tdas_programa->hash_apariciones = hash_crear(NULL);
    if( !tdas_programa->hash_apariciones)
        fprintf(stderr,"%s","falla en reinicio de hash\n");
}

/*
*post:la estructura de tweets_t enviada fue destruida.
*/
void destruir_tweet(tweets_t* max_a_imprimir ){
    free(max_a_imprimir->palabra_tweet);
    free(max_a_imprimir);
}

/*pre:tdas_programa , los tdas que necesita el programa fueron creados correctamente.
*post:se cargaron  en el heap de minimos los k_tweets con mas apariciones.
*/
void cargar_heap_min_kelem(TDA_procesar_t* tdas_programa, size_t k_tweets){
    size_t contador = 0;
    while (!cola_esta_vacia(tdas_programa->cola)) {
        tweets_t* tweet = malloc(sizeof(tweets_t));
        if(!tweet) {
            fprintf(stderr,"%s","hubo una falla al crear estructura tweet\n" );
            return ;
        }
        tweet-> palabra_tweet = (char*)cola_desencolar(tdas_programa->cola);
        tweet-> apariciones = obtener_minima_aparicion( tdas_programa->sketch ,tweet->palabra_tweet );
        if ( contador < k_tweets) {
            contador +=1 ;
            heap_encolar( tdas_programa->heap_min, tweet );
        }else if( comparacion( heap_ver_max(tdas_programa->heap_min), tweet) < 0 ){
            destruir_tweet((tweets_t*)heap_desencolar(tdas_programa->heap_min));
            heap_encolar(tdas_programa->heap_min, tweet);
        }else
            destruir_tweet(tweet);
    }
}

/*pre: tdas_programa , los tdas que necesita el programa fueron creados correctamente.
*post:se apilaron los elementos del heap de minimos en la pila
*/
void desencolar_heap_a_pila(TDA_procesar_t * tdas_programa){
    while(!heap_esta_vacio(tdas_programa->heap_min)){
        pila_apilar(tdas_programa->pila , heap_desencolar(tdas_programa->heap_min));
    }
}
/*pre:tdas_programa , los tdas que necesita el programa fueron creados correctamente.
*post: se imprimieron los k_tweets de mayor a menor apariciones.
*/
void imprimir_maximos(TDA_procesar_t* tdas_programa, size_t* impresiones_top){
    *impresiones_top +=1;
    if(!pila_esta_vacia(tdas_programa->pila))
        printf("--- %zu\n", *impresiones_top );
    while(!pila_esta_vacia(tdas_programa->pila) ){
        tweets_t* max_a_imprimir = (tweets_t*)pila_desapilar(tdas_programa->pila);
        printf("%u ",max_a_imprimir->apariciones );
        printf("%s\n",max_a_imprimir->palabra_tweet);
        destruir_tweet(max_a_imprimir);
    }
}
/*pre:los tdas que necesita el programa fueron creados correctamente.
*post:llama a los procedimientos(las acciones) y imprime los top k_tweets
*/
void acciones_de_impresion(TDA_procesar_t* tdas_programa,size_t k_tweets,size_t* contador_impresiones){
    cargar_heap_min_kelem(tdas_programa, k_tweets );
    desencolar_heap_a_pila(tdas_programa);
    imprimir_maximos(tdas_programa, contador_impresiones);
}
/*pre: los tdas que necesita el programa fueron creados correctamente, los argumentos se validaron.
*post:se procesaron todos los tweets del archivo o de la stdin y se imprimieron
*los k_tweets con mas hash_apariciones en el rango de nlinea
*/
void procesar_tweets(TDA_procesar_t* tdas_programa,size_t nlinea, size_t k_tweets){
    size_t cont_linea = 0,contador_impresiones =0;
    char* texto = NULL;
    size_t cantidad  ;
    ssize_t largo_texto = getline(&texto ,&cantidad , stdin);
    while(largo_texto != -1){
        cont_linea +=1;
        texto[largo_texto -1] = '\0';
        procesar_linea(texto, tdas_programa );
        if(cont_linea == nlinea){
            reiniciar_hash(tdas_programa);
            acciones_de_impresion( tdas_programa, k_tweets, &contador_impresiones);
            cont_linea = 0;
        }
        largo_texto = getline(&texto ,&cantidad ,stdin);
    }
    free(texto);    //debo eliminar el contenido de texto.
    acciones_de_impresion( tdas_programa, k_tweets, &contador_impresiones);
}
int main(int argc, char const *argv[]) {
    if(!cantidad_argumentos_valido(argc))   return -1;
    size_t nlineas = 10, k_tweets = 3;
    if(!validacion_argumentos(argv, &nlineas, &k_tweets)){
        fprintf(stderr,"alguno de los argumentos no son numeros\n");
        return -1;
    }
    TDA_procesar_t* estructura_procesar = crear_TDAS_procesar();
    if(!estructura_procesar)    return -1;
    procesar_tweets(estructura_procesar, nlineas, k_tweets);
    destruir_TDAS_procesar(estructura_procesar);
    return 0;
}
