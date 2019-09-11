#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

struct lista;
typedef struct lista lista_t;

//Crea una lista.
//Post: devuelve una nueva lista vacia.
lista_t *lista_crear(void);

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Agrega un nuevo elemento al inicio de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento la lista, dato se encuentra al principio
// de la lista.
bool lista_insertar_primero(lista_t *lista, void *dato);

// Agrega un nuevo elemento al final de la lista. Devuelve falso en caso de error.
// Pre: la lista fue creada.
// Post: se agregó un nuevo elemento la lista, dato se encuentra al final
// de la lista.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

// Saca el primer elemento de la lista. Si la lista tiene elementos, se quita el
// primero de la lista, y se devuelve su valor, si está vacía, devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el valor del primer elemento anterior, la pila
// contiene un elemento menos, si la lista no estaba vacía.
void *lista_borrar_primero(lista_t *lista);

// Obtiene el valor del primer elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del primero, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el primer elemento de la lista, cuando no está vacía.
void *lista_ver_primero(const lista_t *lista);

// Obtiene el valor del ultimo elemento de la lista. Si la lista tiene
// elementos, se devuelve el valor del ultimo, si está vacía devuelve NULL.
// Pre: la lista fue creada.
// Post: se devolvió el ultimo elemento de la lista, cuando no está vacía.
void *lista_ver_ultimo(const lista_t* lista);

// Obtiene el largo de la lista.
// Pre: la lista fue creada.
// Post: se devolvió el largo de la lista.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la pila, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la pila.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

////Iterador Interno
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);

////Iterador Externo

typedef struct lista_iter lista_iter_t;

/*Crea un iterador de listas.
 *Post: devuelve un iterador de listas apuntando al primer valor.
*/
lista_iter_t *lista_iter_crear(lista_t *lista);

/*Avanza el iterador una posición sobre la lista.
 *Pre: el iterador debe haber sido creado.
 *Post: avanza una posición en la lista. Devuelve 'true' si se realiza con exito, 'false' si no es posible.
*/
bool lista_iter_avanzar(lista_iter_t *iter);

/*Muestra el valor actual del iterador.
 *Pre: el iterador debe haber sido creado.
 *Post: devuelve el valor actual del iterador, si no es posible devuelve 'NULL'.
*/
void *lista_iter_ver_actual(const lista_iter_t *iter);

/*Verifica si el iterador esta al final de la lista.
 *Pre: el iterador debe haber sido creado.
 *Post: devuelve 'true' si el iterador esta al final de la lista, de lo contrario devuelve 'false'.
*/
bool lista_iter_al_final(const lista_iter_t *iter);

//E limina el iterador de lista.
//Pre: el iterador debe haber sido creado.
//Post: elimina el iterador.
void lista_iter_destruir(lista_iter_t *iter);

// Inserta un dato en la lista
// Pre: el iterador debe haber sido creado.
// Post: inserta un dato en la lista, de efectuarse devuelve true,
// en cambio si no es posible devuelve false.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

// Borra el dato actual de una lista.
// Pre: el iterador debe haber sido creado.
// Post: borra un elemento de una lista y devuelve dicho valor;
void *lista_iter_borrar(lista_iter_t *iter);

////Pruebas Alumno

void pruebas_lista_alumno(void);

#endif