# Twitter y una implementación del TT

## Introducción

Twitter es una red social basada en la publicación mensajes cortos (_tweets_). Una de las características de esta red es la publicación de los temas más comentados del momento, llamados _trending topics_ (_TT_). Los usuarios pueden contribuir a la popularidad de ciertos temas mencionándolos usando _hashtags_ que se crean agregando un caracter numeral a una palabra, por ejemplo `#Algoritmos2`.

El objetivo de este trabajo práctico es modelar un sistema que calcule los _TT_’s de una serie de tweets.
## Consigna

Implementar dos programas en C:

-   `procesar_tweets`: este programa se utilizará para contar los trending topics en un conjunto de tweets, leyendo cada línea proveniente de la _entrada estándar_ (`stdin`). Como se sabe que la cantidad de mensajes puede ser muy grande, o bien infinitos, para usarlo se requieren dos parámetros enteros, llamados _n_ y _k_.
    
    El programa deberá imprimir por _salida estándar_ (`stdout`) el **histórico** de los _k_ TTs aproximados cada _n_ lineas, ordenados por ocurrencias (en caso que dos TT tengan la misma cantidad de ocurrencias, se deberá mostrar por orden alfabético). Pueden suponer que _n_ y _k_ serán valores tales que se pueda almacenar en memoria esa cantidad de cadenas o estructuras auxiliares.
    
    El tiempo de procesamiento de cada tag debe ser _O(1)_, mientras que la impresión de los _k_ TT debe realizarse en _O(n log k)_.
    
    Ejemplo de invocación: `~$ cat tweets.txt | ./procesar_tweets 300000 20`
    
    Recomendamos crear un TDA CountMinSketch, para lograr que el código quede modularizado. La cantidad de funciones de hashing a utilizar (y por ende, contadores), así como el largo de éstos serán dejados a criterio del grupo.
    
    Asimismo, se pide que se imprima la frecuencia estimada para poder realizar durante la corrección una comparación con la frecuencia exacta. La salida debe ser como se indica a continuación, con separadores numerados entre cada grupo de _TT_; por ejemplo, para una entrada de 9 líneas, con _n=4_ y _k=2_ la salida sería del estilo:
    
    ```
      --- 1
      7 pesquisa
      2 zombietalk
      --- 2
      14 niley
      3 squarespace
      --- 3
      1 charlotte
    
    ```
    
-   `procesar_usuarios`: este programa tendrá como objetivo contar la cantidad de hashtags que usa cada usuario, leyendo cada línea del archivo pasado por parámetro. Como se sabe que la cantidad de usuarios es mucho menor a la cantidad de TTs, y que dicho archivo termina, consideramos que se puede almacenar en memoria todo lo necesario para procesar la entrada.
    
    El programa deberá procesar la entrada y luego deberá escribir por _salida estándar_ los usuarios y la cantidad de hashtags que utilizaron en tiempo lineal: _O(u + t)_ siendo _u_ la cantidad de usuarios encontrados y _t_ la cantidad de hashtags diferentes, ordenados según ocurrencias. Los usuarios que tienen la misma cantidad de hashtags tienen que ser impresos por orden alfabético (tener en cuenta que para ordenar alfabéticamente a los usuarios, los nombres de los mismos no superan más de 15 caracteres).
    
    Ejemplo de invocación: `~$ ./procesar_usuarios tweets.txt`
    
    Ejemplo de salida: El usuario Mile tiene 5 tags, Martín 3, Jorge 3, Javier 5, Nacho 8, Cami 2, Ceci 5, Ezequiel 3, Matías 2, Dato 6, Anita 1, Gian 1 Se debe escribir por salida estándar:
    
    ```
      1: Anita, Gian
      2: Cami, Matias
      3: Ezequiel, Jorge, Martín
      5: Ceci, Javier, Mile
      6: Dato
      8: Nacho
    
    ```
    

En ambos casos, la entrada debe tener el siguiente formato:

```
	Usuario1,tag1,tag2,...
	Usuario2,tag3,tag4,...
	Usuario1,tag5,tag1,...

```

Donde cada línea corresponde a un tweet realizado por el usuario mencionado, continuado por una los tags en dicho tweet. Por eso, un mismo usuario puede aparecer más de una vez. No hay líneas que no contengan al menos un tag.

Para esto, contarán con un [archivo de prueba provisto por el curso](https://drive.google.com/file/d/0B_oxuLrlET2hU1ZYbDJ2LXY1Z2s/view?usp=sharing). Dicho archivo fue procesado de sus originales [aquí](https://snap.stanford.edu/data/twitter7.html). También les brindamos una [versión con los primeros cien tweets](https://drive.google.com/file/d/0B0x0VPz_v-f_RlZydHY4LVNsSFk/view?usp=sharing) para usar durante la etapa de desarrollo.

Pueden crear otros archivos de pruebas que a ustedes les sirva para probar el programa.

_Atención_: tener en cuenta que el archivo provisto por el curso, descomprimido, 460 Mb, y cuenta con más 19 millones de líneas. Se recomienda utilizar el comando `less` de Unix para leerlo, ya que no lo carga en memoria: `less tweets.txt`.

## Criterios de aprobación

El código entregado debe ser claro y legible y ajustarse a las especificaciones de la consigna. Debe compilar sin advertencias y correr sin errores de memoria. La fecha de entrega del TP es el **lunes 07/06**.

La entrega incluye, obligatoriamente, los siguientes archivos de código:

-   Código fuente de `procesar_tweets`.
-   Código fuente de `procesar_usuarios`.
-   El código de los TDAs programados en la cursada que se requieran.
-   Un archivo `deps.mk` con las dependencias del proyecto en formato make. Este deberá contener sólamente una línea por programa que indique qué _objetos_ necesita para compilar el ejecutable de cada uno de los archivos, por ejemplo:

```
procesar_tweets: procesar_tweets.o count_min_sketch.o
procesar_usuarios: procesar_usuarios.o hash.o

```

El corrector automático va a interpretar ese archivo de dependencias y va a compilar todos los `.o` especificados a partir de los `.h` y `.c` que deberán enviar, con los siguientes flags de `GCC`:

```
-g -std=c99 -Wall -Wtype-limits -pedantic -Wconversion -Wno-sign-conversion

```
