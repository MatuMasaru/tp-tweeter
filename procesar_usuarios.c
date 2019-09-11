#define _POSIX_C_SOURCE 200809L
#include "abb.h"
#include "hash.h"
#include "cola.h"
#include "strutil.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#define TAM 10
#define CANT_ARGUMENTOS 2
#define FIN_STRING '\0'

ssize_t getline(char **lineptr, size_t *n, FILE *stream);

/*
 * Devuelve la True si la cantidad de argumentos es valido de lo contrario False.
*/

bool cantidad_argumentos_valido(int numero_argumentos){
    bool condicion = (numero_argumentos == CANT_ARGUMENTOS);
    if (!condicion) fprintf(stderr,"%s","Error: Cantidad erronea de parametros\n");
    return condicion;
}

/*
 * Devuelve la True si el archivo se abre sin problemas de lo contrario False.
 * Pre:
 */

bool archivo_fue_abierto(char* argv ,FILE** archivo){
    *archivo = fopen(argv,"r");

    if(!*archivo) fprintf(stderr,"%s","Error: archivo fuente inaccesible\n");

    return *archivo;
}

/* Verifica si hubo algun error en cantidad de argumentos 
 * de entrada o al abrir el archivo.
 * Devuelve true si no hubo ningun error de caso contrario false.
*/
bool verificacion_errores(char* argv,int argc ,FILE** archivo){
    bool verificacion_argumentos = cantidad_argumentos_valido(argc);
    bool archivo_abierto = !verificacion_argumentos;
    if(verificacion_argumentos)
        archivo_abierto = archivo_fue_abierto(argv,archivo);

    return (archivo_abierto == verificacion_argumentos);
}

/******************* FUNCIONES VISITAR DE ABB **********************/

/* Imprime las claves y los valores  que estan en el  ABB.
 * Devuelve true.
*/
bool imprimir(const char* numero_tweest , void* perfil ,void* sin_usar){
    printf("%s: ",numero_tweest);
    while(!cola_esta_vacia((cola_t*)perfil)){
        char* usuario = cola_desencolar((cola_t*)perfil);
        printf("%s",usuario);
        if(!cola_esta_vacia((cola_t*)perfil)) 
            printf(", ");
        free(usuario);
    }
    printf("\n");
    return true;
}

/* Obtiene las claves y los valores y hace una copia en un nuevo abb.
 * return true o false si hubo algun error.  
*/
bool obtener_informacion(const char* perfil_usuario ,void* tweets , void* arbol){
    char clave[TAM];
    size_t cantidad_tweets = hash_cantidad(tweets);

    sprintf(clave ,"%zu" ,cantidad_tweets);

    bool pertenece = abb_pertenece(arbol,clave);

    cola_t* usuarios = (pertenece)? abb_obtener(arbol ,clave):cola_crear();
    
    if(!usuarios)
        return false;

    char* copia_perfil_usuario = strdup(perfil_usuario);

    cola_encolar(usuarios ,copia_perfil_usuario);

    if(!pertenece)
        abb_guardar(arbol ,clave ,usuarios);

    return true;
}

/*******************************************************************/


/******************* WRAPPERS **********************/
void destruir_tweets(void* tweets){
    hash_destruir((hash_t*)tweets);
}

void destruir_usuarios(void* usuarios){
    cola_destruir((cola_t*)usuarios ,free);
}

/***************************************************/

/*
 * Guarda en un abb nombre de usuario y sus tweets(los tweets del usuario van a estar en un hash)
 * Obtenidas por el texto.
 * Pre: abb usuarios tiene que estar creado.
*/
void guardar_informacion(abb_t* usuarios,char* texto){
    char** informacion_usuario = split(texto,',');
    bool pertenece = abb_pertenece(usuarios ,informacion_usuario[0]);
    hash_t* tweets_usuario = (pertenece)? abb_obtener(usuarios ,informacion_usuario[0]):hash_crear(free);

    for (size_t i = 1;informacion_usuario[i] != NULL; i++)
        hash_guardar(tweets_usuario,informacion_usuario[i],NULL);
    
    if(!pertenece)
        abb_guardar(usuarios ,informacion_usuario[0] ,tweets_usuario);

    free_strv(informacion_usuario);
}

/* Lee el archivo linea a linea y guarda la informacion en un ABB.
 * Pre: El archivo tiene que estar abierto y abb de TWEETS tiene 
 * que estar creado. 
*/
void lectura_archivo(FILE ** archivo ,abb_t* tweets){
    size_t cantidad  = 0;
    char* texto = NULL;
    ssize_t largo_texto = getline(&texto ,&cantidad ,*archivo);
    
    while(!feof(*archivo)){
        texto[largo_texto -1] = FIN_STRING;
        guardar_informacion(tweets ,texto);
        largo_texto = getline(&texto ,&cantidad ,*archivo);
    }

    free(texto);
}



int main(int argc, char *argv[]){
    FILE* archivo;

    if(!verificacion_errores(argv[1],argc,&archivo)) 
        return -1;

    abb_t* tweets = abb_crear(strcmp ,destruir_tweets);

    if(!tweets)
        return -1;
    
    lectura_archivo(&archivo,tweets);

    abb_t* usuarios = abb_crear(strcmp ,destruir_usuarios);
    
    abb_in_order(tweets ,obtener_informacion ,usuarios);

    abb_destruir(tweets);

    abb_in_order(usuarios ,imprimir ,NULL);

    abb_destruir(usuarios);

    fclose(archivo);
    return 0;
}
