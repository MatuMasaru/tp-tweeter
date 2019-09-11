#include "lista.h"
#include "testing.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

//la lista esta planteada como una lista de nodos punteros
typedef struct nodo{
	void* dato;
	struct nodo* siguiente;

}nodo_t;

struct lista{
	nodo_t* primero;
	nodo_t* ultimo;
	size_t largo;
};

//crea un nodo con sus re NULL.
nodo_t* crear_nodo(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if(!nodo) 
		return NULL;
	
	nodo->dato = valor;
	nodo->siguiente = NULL;
	return nodo;
}

// crea una lista vacia.
lista_t *lista_crear(void){

	lista_t* lista = malloc(sizeof(lista_t));
	if(!lista) 
		return NULL;
	lista->primero = NULL;
	lista->ultimo = NULL;
	lista->largo = 0;

	return lista;
}

bool lista_esta_vacia(const lista_t *lista){

	return !lista->largo;

}

bool lista_insertar_primero(lista_t *lista, void *dato){
	nodo_t* nodo = crear_nodo(dato);
	if (!nodo) 
		return false;
	if (lista_esta_vacia(lista))
		lista->ultimo = nodo;
	else 
		nodo->siguiente = lista->primero;

	lista->primero = nodo;
	lista->largo++;
	return true;
}
//inserta a lo ultimo de la lista al nuevo dato
bool lista_insertar_ultimo(lista_t *lista, void *dato){

	nodo_t* nodo = crear_nodo(dato);
	if (!nodo) return false;

	if (lista_esta_vacia(lista)){

		lista->primero = nodo;
	}
	else {

		lista->ultimo->siguiente = nodo;
	}

	lista->ultimo = nodo;
	lista->largo++;
	return true;
}
//devuelve el contenido borrado de la primer posicion de la lista.
void *lista_borrar_primero(lista_t *lista){

	if(lista_esta_vacia(lista)) 
		return NULL;

	nodo_t* auxiliar = lista->primero;
	void* dato = auxiliar->dato; 
	lista->primero = auxiliar->siguiente;

	if(auxiliar == lista->ultimo)
		lista->ultimo = NULL;

	free(auxiliar);
	lista->largo--;
	return dato;
}
//devuelve el valor del primer elemento de la lista.
void *lista_ver_primero(const lista_t *lista){

	if (lista_esta_vacia(lista)) 
		return NULL;

	return lista->primero->dato;
}
//devuelve el valor del ultimo elemento de la lista.
void *lista_ver_ultimo(const lista_t* lista){
	if (lista_esta_vacia(lista)) 
		return NULL;

	return lista->ultimo->dato;
}
// devuelve la cantidad de elementos que tiene la lista(el tope).
size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void *)){
	while(!lista_esta_vacia(lista)){
		void* dato = lista_borrar_primero(lista);
		if(destruir_dato)
			destruir_dato(dato);
	}
	free(lista);
}


struct lista_iter{
	nodo_t* anterior;
	nodo_t* actual;
	lista_t* lista;
};


//
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){

	nodo_t* actual = lista->primero;
	bool continuar = true;
	
	while(actual && continuar){
		if (visitar){
			continuar = visitar(actual->dato, extra);
		}
		actual = actual->siguiente;
	}	
}



lista_iter_t *lista_iter_crear(lista_t *lista){
	lista_iter_t* iter = malloc(sizeof(lista_iter_t));
	if(!iter) 
		return NULL;
	iter->actual = NULL;
	if(!lista_esta_vacia(lista)) 
		iter->actual = lista->primero;

	iter->lista = lista;
	iter->anterior = NULL;
	iter->actual = lista->primero;

	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){

	if(lista_iter_al_final(iter)) 
		return false;

	iter->anterior = iter->actual;
	iter->actual = iter->actual->siguiente;
	return true;
}

void *lista_iter_ver_actual(const lista_iter_t *iter){
	if(!iter->actual)
		return NULL;
	return iter->actual->dato;
}

bool lista_iter_al_final(const lista_iter_t *iter){
	return !iter->actual;
}

void lista_iter_destruir(lista_iter_t *iter){
	free(iter);
}

bool lista_iter_insertar(lista_iter_t *iter, void *dato){

	nodo_t* nodo = crear_nodo(dato);
	if (!nodo) 
		return false;

	if(!iter->anterior){
		nodo->siguiente = iter->lista->primero;
		iter->lista->primero= nodo;	
		if(!nodo->siguiente)
			iter->lista->ultimo =nodo;
	}else{
		nodo->siguiente= iter->actual;
		iter->anterior->siguiente = nodo;
		if(!iter->actual)
			iter->lista->ultimo=nodo;
	}
	iter->actual = nodo;
	(iter->lista->largo)++;
	return true;		
}

void *lista_iter_borrar(lista_iter_t *iter){

	if (lista_iter_al_final(iter) || lista_esta_vacia(iter->lista)) 
		return NULL;

	nodo_t* auxiliar= iter->actual;
	void* dato= auxiliar->dato;

	if (iter->actual == iter->lista->primero){
		iter->lista->primero = auxiliar->siguiente;
		iter->actual = iter->lista->primero;
	}else{
		if (iter->actual == iter->lista->ultimo)
			iter->lista->ultimo = iter->anterior;

		iter->anterior->siguiente = auxiliar->siguiente;
		iter->actual= iter->anterior->siguiente;
	}
	iter->lista->largo--;
	free(auxiliar);
	return dato;
}