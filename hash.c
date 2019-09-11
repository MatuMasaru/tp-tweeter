#define _POSIX_C_SOURCE 200809L
#include "hash.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define TAM_INICIAL 97
#define VACIO 1
#define OCUPADO 0
#define BORRADO -1
#define FACTOR_CARGA_MAX 0.60
#define FACTOR_CARGA_MIN 0.10
#define DUPLICAR 2
#define MITAD 2

    // ESTRUCTURAS  


typedef struct hash_campo {
    char *clave;
    void *valor;
    int estado; 
} hash_campo_t;

struct hash {
    size_t cantidad; //cantidad de elementos ocupados          
    size_t largo; //tamaño de la tabla de hash                     
    float carga;  //cantidad/largo (densidad del hash)         
    hash_campo_t *tabla; 
    hash_destruir_dato_t destruir_dato; 
};

struct hash_iter{
	const hash_t* hash_i;
	size_t contador;
	size_t pos_iter;
};

size_t funcion_hash(const char* s,size_t tam){ 
    size_t hash_val; 
	for (hash_val = 0; *s != '\0'; s++){
		hash_val = *s + 131 * hash_val;
	}
	return hash_val % tam;
}

                // AUXILIARES 
//modificar es O(N)

int buscar (const hash_t* hash ,const char* clave){  
    size_t posicion = funcion_hash(clave, hash->largo);
    for (size_t i = 1 ;i < hash->largo ;i++) {
        if(hash->tabla[posicion].estado == VACIO) break;   
        if( hash->tabla[posicion].estado == OCUPADO){
            if (!strcmp(hash->tabla[posicion].clave , clave)) 
                return (int)posicion;
        }
        posicion++;
        if(posicion == hash->largo) posicion = 0;
    }
    return -1;
}



hash_campo_t *inicializar_campo(size_t largo){
    hash_campo_t* tabla = malloc(largo * sizeof(hash_campo_t));
    if(!tabla) 
        return NULL;
    for (size_t i = 0; i < largo; i++){
        tabla[i].estado = VACIO;        
        tabla[i].clave = NULL;
        tabla[i].valor = NULL;
    }   
    return tabla;
}
bool hash_redimencionar(hash_t* hash,size_t nuevo_largo){
    hash_campo_t* tabla = inicializar_campo(nuevo_largo);
    if(!tabla)
        return false;
    for (size_t i = 0; i < hash->largo; i++){
        if(hash->tabla[i].estado == OCUPADO){
            size_t pos = funcion_hash(hash->tabla[i].clave , nuevo_largo);
            while(tabla[pos].estado == OCUPADO){
                pos++;
                if(pos == nuevo_largo)
                    pos = 0;
            
            }
            tabla[pos] = hash->tabla[i];
        }
    }
    free(hash->tabla);
    hash->tabla = tabla;
    hash->largo = nuevo_largo;
    hash->carga = (float)hash->cantidad / (float)hash->largo; 
    return true;
}


//PRIMITIVAS

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){         // ES TOTALMENTE LINEAL O(N)
    hash_t* hash = malloc(sizeof(hash_t));
    if (!hash) 
        return NULL;
    hash->tabla = inicializar_campo(TAM_INICIAL);               // O(N)
    if(!hash->tabla){
        free(hash);                                             //O(1)
        return NULL;
    }
    hash->destruir_dato = destruir_dato;
    hash->cantidad = 0;
    hash->largo = TAM_INICIAL;
    hash->carga = 0;
    return hash;
}
 
 /*Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
 // 1 vacio => -1 => borrado 0 => ocupado
bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    if(FACTOR_CARGA_MAX < hash->carga){
        if(!hash_redimencionar(hash ,(hash->largo * DUPLICAR) + 1)) 
            return false;
    }
    int pos = buscar(hash ,clave);
    if(pos > -1){
        if(hash->destruir_dato)
            hash->destruir_dato(hash->tabla[pos].valor);
        hash->tabla[pos].valor = dato;
        return true;
    }
    size_t posicion = funcion_hash(clave,hash->largo);
    while(hash->tabla[posicion].estado != VACIO){
        posicion++;       
        if((size_t)posicion == hash->largo) posicion = 0;
    }
    hash->tabla[posicion].clave = strdup(clave);
    hash->tabla[posicion].valor = dato;
    hash->tabla[posicion].estado = OCUPADO;
    hash->cantidad++;
    hash->carga = (float)hash->cantidad / (float)hash->largo;
    return true;
}
/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve    
 * NULL si el datio no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave){
    if(!hash_cantidad(hash))
        return NULL;
    if( (hash->carga  <= FACTOR_CARGA_MIN) && (TAM_INICIAL < hash->largo) )
        hash_redimencionar(hash ,hash->largo / MITAD);
    int posicion = buscar(hash,clave);
    if (posicion != -1){
        hash->tabla[posicion].estado = BORRADO;
        free((hash->tabla[posicion].clave));
        hash->cantidad--;
        return hash->tabla[posicion].valor;
    }
    return NULL;

}

 /* Obtiene el valor de un elemento del hash, si la clave no se encuentra
 * devuelve NULL.
 * Pre: La estructura hash fue inicializada
 */
void *hash_obtener(const hash_t *hash, const char *clave){
    if(!hash_cantidad(hash))
        return NULL;
    int posicion = buscar(hash,clave);
    return (posicion != -1)? hash->tabla[posicion].valor : NULL;
}
  
 /* Determina si clave pertenece o no al hash.
 * Pre: La estructura hash fue inicializada
 */
bool hash_pertenece(const hash_t *hash, const char *clave){
    if(!hash_cantidad(hash)) return false;
    return (buscar(hash,clave) != -1)?  true : false;
}

/* Devuelve la cantidad de elementos del hash.
 * Pre: La estructura hash fue inicializada
*/
size_t hash_cantidad(const hash_t *hash){
    return hash->cantidad;
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * hash_t *hash_crear(hash_destruir_dato_t destruir_dato); * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
    for (size_t i = 0; i < hash->largo ; i++){
        if(hash->tabla[i].estado == OCUPADO){
            if(hash->destruir_dato) 
                hash->destruir_dato(hash->tabla[i].valor);
            free((char*)hash->tabla[i].clave);
        }
    }
    free(hash->tabla);
    free(hash);
}

hash_iter_t *hash_iter_crear(const hash_t *hash){
	hash_iter_t* iter = malloc(sizeof(hash_iter_t));        // O(N)
    if(!iter)
		return NULL;                                          //O(1)

	iter-> hash_i = hash;                                   //O(1)
	size_t i = 0;
	if ( hash_cantidad(hash) ){                              // CANTIDAD ES 0(N)
		for (i = 0 ; hash->tabla[i].estado != OCUPADO ; ++i);
    }else{
        i = hash->largo;
    }
	iter-> pos_iter = i;
	return iter;
}

bool hash_iter_al_final(const hash_iter_t *iter){       //este esta bien O(1)
	return (iter->pos_iter == iter->hash_i->largo);
}

bool hash_iter_avanzar(hash_iter_t *iter){
	if(hash_iter_al_final(iter) )                  // O(1)
		return false;
    size_t i= iter->pos_iter + 1;
    for (  ; i < iter->hash_i->largo  ; i++){
        if(iter->hash_i->tabla[i].estado == OCUPADO){       //O(N)
	        iter->pos_iter = i;
            return true;
        }
    }
    iter->pos_iter = i;
	return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){      //O(1)
	return (hash_iter_al_final(iter))? NULL : iter->hash_i->tabla[iter->pos_iter].clave; 
}

void hash_iter_destruir(hash_iter_t* iter){                 
    free(iter);
}





