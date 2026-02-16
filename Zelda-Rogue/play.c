#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAXVIDA 127

static char nombres[3][4][20] = {
    {"Agua", "Tierra", "Fuego", "Aire"},//aldeas
    {"Manhattan", "Hell's Kitchen", "Brooklyn", "Queens"},//mazmorras
    {"Espada de plastico", "Sable de luz", "Adamantium", "Mjolnir"},//items
};

static char nombresParalelos[3][4][20] = {
    {"Sangre", "Metal", "Rayo", "Lodo"},//aldeas
    {"Metropolis", "Gotham", "Atlantis", "Themyscira"},//mazmorras
    {"Espada de fuego", "Sable Oscuro", "Vibranium", "Stormbreaker"},//items
};

//arreglo con las posibles combinaciones entre 4 elementos
static int arr[60] = {12,13,14,23,24,34,43,42,41,32,31,21,//12
                    123,124,134,143,142,132,213,214,234,243,241,231,//24
                    312,314,324,342,341,321,412,413,423,432,431,421,//36
                    1234,1243,1324,1342,1423,1432,2134,2143,2314,2341,2413,2431,//48
                    3124,3142,3214,3241,3412,3421,4123,4132,4213,4231,4312,4321//60
};

//Funcion que mezcla los nombres
char* mezcla_nombres(char nombres[4][20], int arr){
    char *resultado = (char *)malloc(sizeof(char)*80);
    if(arr > 9){
        int izq = arr / 10, der = arr % 10;
        strcpy(resultado, mezcla_nombres(nombres,izq));
        strcat(strcat(resultado,"-"), mezcla_nombres(nombres,der));
    }else
        strcpy(resultado, nombres[arr-1]);
    return resultado;
}

struct item {
    int id;
    int encontrado;
    char *nombre;
    struct mazmorra *atac;
    struct item *sig;
    // El item solo puede estar ubicado en una aldea o una mazmorra
    // nunca en dos lugares a la vez
    struct mazmorra *ubicMazmorra;
    struct aldea *ubicAldea;
};

struct mazmorra {
    int id;
    bool derrotada;
    bool buscado;
    char *nombre;
    struct aldea *ald;
    struct aldea *sig;
    // Hay un apuntador para el item que está ubicado (si aplica, puede ser NULL)
    // y otro apuntador para el item que la derrota (no debe ser NULL)
    struct item *itemPresente;
    struct item *itemDerrota;
};

struct aldea {
    int id;
    char *nombre;
    bool buscado;
    struct mazmorra *maz;
    struct aldea *trans;
    struct aldea *sig;
    struct aldea *ant;
    struct item *itemPresente;
};

//inicializa aldea
struct aldea *crearAldea(int i, char *nombre, struct mazmorra *maz){
    struct aldea *nueAld = (struct aldea *)malloc(sizeof(struct aldea));
    nueAld->id = i;  nueAld->nombre = nombre;  nueAld->maz = maz;  
    nueAld->trans = NULL;  nueAld->sig = NULL;  nueAld->ant = NULL;
    nueAld->itemPresente = NULL;    nueAld->buscado = false;
    maz->ald = nueAld;
    return nueAld;
}

//inicializa mazmorra
struct mazmorra *crearMazmorra(int i, char *nombre){
    struct mazmorra *nueMazm = (struct mazmorra *)malloc(sizeof(struct mazmorra));
    nueMazm->id = i;  nueMazm->nombre = nombre;  
    nueMazm->ald = NULL;  nueMazm->sig = NULL;
    nueMazm->itemPresente = NULL; nueMazm->itemDerrota = NULL;
    nueMazm->derrotada = false; nueMazm->buscado = false;
    return nueMazm;
}

//inicializa item
struct item *crearItem(int i, char *nombre){
    struct item *nueItem = (struct item *)malloc(sizeof(struct item));
    nueItem->id = i;  nueItem->nombre = nombre;  nueItem->encontrado = 0;  
    nueItem->atac = NULL;  nueItem->sig = NULL; 
    nueItem->ubicAldea = NULL; nueItem->ubicMazmorra = NULL;
    return nueItem;
}

//cabeza de lista de aldea
struct headA {
    struct aldea *primero;
};

/* Crea la lista y retorna un puntero a la cabeza */
struct headA *crearListaA(struct aldea *primer) {
    struct headA *x = (struct headA *)malloc(sizeof(struct headA));
    if(x != NULL)
        x->primero = primer;
    return x;
}

/* Agrega un elemento al principio de la lista de aldeas*/
struct headA *agregarAldea(struct aldea *nuevo, struct headA *actual) {
    if(actual->primero != NULL){
        actual->primero->ant = nuevo;
    }
    nuevo->sig = actual->primero;
    nuevo->maz->sig = actual->primero;
    actual->primero = nuevo;
    return actual;
}

//cabeza de lista de item
struct headI {
    struct item *primero;
};

/* Crea la lista y retorna un puntero a la cabeza */
struct headI *crearListaI(struct item *primer) {
    struct headI *x = (struct headI *)malloc(sizeof(struct headI));
    if(x != NULL)
        x->primero = primer;
    return x;
}

/* Agrega un elemento al principio de la lista de items*/
struct headI *agregarItem(struct item *nuevo, struct headI *actual) {
    nuevo->sig = actual->primero;
    actual->primero = nuevo;
    return actual;
}

//Genera una pausa para que el usuario lea el mensaje
void enter(){
    printf("\nPresiona enter para continuar"); while(getchar() != '\n');
}

//imprime la lista Aldea
void imprimirListaA(struct aldea *posicion, struct headA *cabeza, struct headA *cabezaP, int unlock, int esAldea) {
    struct aldea *actual = cabeza->primero;
    struct aldea *actualP = cabezaP->primero;
    while (actual != NULL && actualP != NULL) {
        if(actual == posicion){
            if(esAldea){
                if(unlock)  printf("Mazmorra: %s\tAldea: %s\t|\t",actualP->maz->nombre, actualP->nombre);
                printf("Aldea: %s*\tMazmorra: %s\n", actual->nombre, actual->maz->nombre);
            }else{
                if(unlock)  printf("Mazmorra: %s\tAldea: %s\t|\t",actualP->maz->nombre, actualP->nombre);
                printf("Aldea: %s\tMazmorra: %s*\n", actual->nombre, actual->maz->nombre);
            }
        }else if(actualP == posicion){
            if(esAldea){
                if(unlock)  printf("Mazmorra: %s\tAldea: %s*\t|\t",actualP->maz->nombre, actualP->nombre);
                printf("Aldea: %s\tMazmorra: %s\n", actual->nombre, actual->maz->nombre);
            }else{
                if(unlock)  printf("Mazmorra: %s*\tAldea: %s\t|\t",actualP->maz->nombre, actualP->nombre);
                printf("Aldea: %s\tMazmorra: %s\n", actual->nombre, actual->maz->nombre);
            }
        }else{
            if(unlock)  printf("Mazmorra: %s\tAldea: %s\t|\t",actualP->maz->nombre, actualP->nombre);
            printf("Aldea: %s\tMazmorra: %s\n", actual->nombre, actual->maz->nombre);
        }
        actual = actual->sig;   actualP = actualP->sig;
    }
    printf("\n");
}

//Imprime los items que posee el jugador
void imprimirItems(struct headI *item, struct headI *itemP, int unlock){
    struct item *itemActual = item->primero;
    printf("Items: ");
    while(itemActual != NULL){
        if(itemActual->encontrado == 1){
            printf("%s\t",itemActual->nombre);
        }
        itemActual = itemActual->sig;
    }
    printf("\n\n");
    if(unlock){
        struct item *itemActualP = itemP->primero;
        printf("Items: ");
        while(itemActualP != NULL){
            if(itemActualP->encontrado == 1){
                printf("%s,",itemActualP->nombre);
            }
            itemActualP = itemActualP->sig;
        }
        printf("\n");
    }
}

//Muestra los comandos posibles cuando estas en una aldea
void comandosA(int unlock){
    printf("'busq' es el comando para buscar el ítem oculto en la aldea.\n");
    printf("'maz' es el comando para trasladarse a la mazmorra asociada.\n");
    printf("'compr' es el comando para comprar ítems de esa aldea.\n");
    if(unlock)  printf("'trans' es el comando para trasladarse a la aldea del mundo paralelo.\n");
    printf("'ant' es el comando para trasladarse a la aldea anterior\n");
    printf("'sig' es el comando para trasladarse a la aldea siguiente\n");
    printf("'sal' es el comando para salir del juego\n");
    enter();
}

//Muestra los comandos posibles cuando estas en una mazmorra
void comandosM(){
    printf("'busq' es el comando para buscar el ítem oculto en la mazmorra.\n");
    printf("'atac' es el comando para intentar derrotar la mazmorra.\n");
    printf("'ant' es el comando para trasladarse a la aldea asociada\n");
    printf("'sig' es el comando para trasladarse a la aldea siguiente\n");
    printf("'sal' es el comando para salir del juego\n");
    enter();
}

//Funcion de la tienda del juego
void comprar(int *vidas, int *dinero, int *vidaMax, struct headA *aldea){
    printf("\nBienvenido a la tienda! Donde el dinero sí compra la felicidad :)\n");
    int op, adentro = 1;
    while(adentro){
        printf("Dinero actual: %d$.\n Estas son tus opciones:\n1) Recuperar vida <3 - 5$\n2) Ítem de la primera mazmorra - 25$\n3) Vida extra *.* - 100$\n4) Salir\n", *dinero);
        while(scanf("%d", &op) != 1 || op <= 0 || op > 4) {
            printf("Por favor, ingrese una opción válido (número entre 1 y 4). ");
            while(getchar() != '\n'); // Limpia el búfer de entrada
        }
        while(getchar() != '\n');
        switch(op){
            case 1:
                if(*vidas < MAXVIDA && *vidas < *vidaMax)
                    if(*dinero >= 5){
                        *vidas += 1;    *dinero -= 5;
                        printf("Recuperaste una vida perdida!! <3.\n");
                    }else
                        printf("No tienes suficiente dinero :(\nTrasladate entre aldeas para ganar dinero y luego regresa!!\n");
                else
                    printf("Ya recuperaste el máximo de vidas permitido, me encantaría aceptar tu dinero, pero por razones éticas no lo aceptaré,\npierde vidas y luego vuelve por favor!\n");
                break;
            case 2:
                if(aldea->primero->maz->itemDerrota->encontrado == 0)
                    if(*dinero >= 25){
                        *dinero -= 25;   aldea->primero->maz->itemDerrota->encontrado = 1;
                        printf("Obtuviste el ítem que te ayudará a vencer a la primera mazmorra!!\n"); 
                    }else
                        printf("No tienes suficiente dinero :(\nTrasladate entre aldeas para ganar dinero y luego regresa!!\n");
                else
                    printf("Ya posees este ítem, debes buscar los demás, no los puedes comprar\nPero puedes comprar otras cosas!!\n");
                break;
            case 3:
                if(*vidas < MAXVIDA)
                    if(*dinero >= 100){
                        *vidas += 1;    *dinero -= 100;
                        if(*vidaMax < MAXVIDA) *vidaMax += 1;
                        printf("Obtuviste una vida extra!! *.*\n");
                    }else
                        printf("No tienes suficiente dinero :(\nTrasladate entre aldeas para ganar dinero y luego regresa!!\n");
                else
                    printf("Ya alcanzaste el máximo de vidas, me encantaría aceptar tu dinero, pero por razones éticas no lo aceptaré,\npierde vidas y luego vuelve por favor!\n");
                break;
            case 4:
                adentro = 0;
                printf("Vuelva pronto! Recuerde que solo se vive una vez, pero se puede comprar muchas veces!\n");
                break;
            default:
                printf("Opción no válida.\n");
                break;
        }
    }
    enter();
}

struct item *itemDisponible(struct headI *listaItem) {
    struct item *itemDevuelto = listaItem->primero;
    while (itemDevuelto != NULL) {
        if (itemDevuelto->ubicAldea == NULL && itemDevuelto->ubicMazmorra == NULL) {
            break;
        }
        itemDevuelto = itemDevuelto->sig;
    }
    // if (itemDevuelto != NULL)// verificacion
    //     printf("Voy a devolver: %s\n", itemDevuelto->nombre); 
    // else
    //     printf("No devuelvo nada.\n");
    return itemDevuelto;
}

struct item *itemAtacDisponible(struct headI *listaItem) {
    struct item *itemDevuelto = listaItem->primero;
    while (itemDevuelto != NULL) {
        // printf("Item devuelto: %s\n", itemDevuelto->nombre); // verificacion
        if (itemDevuelto->atac == NULL) {
            break;
        }
        itemDevuelto = itemDevuelto->sig;
    }
    // if (itemDevuelto != NULL)// verificacion
    //     printf("Voy a devolver: %s\n", itemDevuelto->nombre); 
    // else
    //     printf("No devuelvo nada.\n");
    return itemDevuelto;
}

struct aldea *mazSinAtac(struct headA *listaAldea) {
    struct aldea *aldeaAsociada = listaAldea->primero;
    while (aldeaAsociada != NULL) {
        if (aldeaAsociada->maz->itemDerrota == NULL) {
            break;
        }
        aldeaAsociada = aldeaAsociada->sig;     
    }
    return aldeaAsociada;
}

struct aldea *mazSinItemUbic(struct headA *listaAldea) {
    struct aldea *aldeaAsociada = listaAldea->primero;
    while (aldeaAsociada != NULL) {
        if (aldeaAsociada->maz->itemPresente == NULL) {
            break;
        }
        aldeaAsociada = aldeaAsociada->sig;     
    }
    return aldeaAsociada;
}

void conectarMundosParalelos(struct headA *listaAldea, struct headA *listaAldeaAlt) {
    struct aldea *aldea = listaAldea->primero;
    struct aldea *aldeaAlt = listaAldeaAlt->primero;
    while (aldea != NULL) {
        aldea->trans = aldeaAlt;
        aldeaAlt->trans = aldea;
        aldea = aldea->sig;
        aldeaAlt = aldeaAlt->sig;
    }
}

void pierdeVida(int randN, int *vidas, int t){
    randN = rand();
    if(randN % 10 == 0){
        *vidas -= 1;
        if(t)   printf("Por razones del destino perdiste una vida al realizar esa acción. </3\n");
        else    printf("Por razones del destino perdiste una vida al trasladarte. </3\n");
        enter();
    }
}

void mensajeparalelo(){
    printf("Felicidades! Desbloqueaste el Mundo Paralelo!!\n");
    printf("Bienvenido al mundo paralelo! Un nuevo set de aldeas y mazmorras que debes derrotar\n");
    printf("Para volver al mundo normal debes derrotar la primera mazmorra del mundo paralelo.\nBuena suerte y que siga la aventura!\n\n");
    printf("\nPresiona enter para continuar"); while(getchar() != '\n');
}

void enlazarItemAldea(struct headA *listaAldea, struct headI *listaItem) {
    struct aldea *aldAsig = listaAldea->primero;
    struct item *itemAsig = NULL;
    int randN;
    while (aldAsig != NULL) {
        randN = rand();
        if (randN % 2 == 0) {
            itemAsig = itemDisponible(listaItem);
            if (itemAsig == NULL) {
                break;
            }
            // Enlaza el item con la aldea
            aldAsig->itemPresente = itemAsig;
            itemAsig->ubicAldea = aldAsig;
        }
        aldAsig = aldAsig->sig;
    }
}

void enlazarItemMazmorra(struct headA *listaAldea, struct headI *listaItem) {
    struct aldea *aldAsig = listaAldea->primero;
    struct item *itemAsig = NULL;
    int randN;
    while (aldAsig != NULL) {
        randN = rand();
        if (randN % 2 == 0) {
            itemAsig = itemDisponible(listaItem);
            if (itemAsig == NULL) {
                printf("No quedan items!\n");
                break;
            }
            // Enlaza el item con la mazmorra
            aldAsig->maz->itemPresente = itemAsig;
            itemAsig->ubicMazmorra = aldAsig->maz;
        }
        aldAsig = aldAsig->sig;
    }
}

/* Una vez creadas las aldeas y mazmorras, si hay items que faltan, se reparte entre mazmorras*/
void verificarItemSobrante (struct headA *listaAldea, struct headI *listaItem) {
    struct aldea *aldAsig;
    struct item *itemAsig;
    int randN;
    // Verifica si hay items sin ubicación y los asigna entre las mazmorras
    itemAsig = itemDisponible(listaItem);
    if (itemAsig != NULL) {
        aldAsig = listaAldea->primero;
        // Mientras haya elementos por asignar, sigue en el bucle (OPTIMIZAR)
        while (itemDisponible(listaItem) != NULL) {
            // Siempre buscaremos una aldea disponible
            if (aldAsig->maz->itemPresente != NULL) {
                aldAsig = mazSinItemUbic(listaAldea);
            }
            randN = rand();
            if (randN % 2 == 0) {
                // Ubica el item
                aldAsig->maz->itemPresente = itemAsig;
                itemAsig->ubicMazmorra = aldAsig->maz;
            }
            itemAsig = itemDisponible(listaItem);
            if (itemAsig == NULL) {
                itemAsig = listaItem->primero; // sigue atravesando!
            }
        }
    }
}

void asignaItemMazmorra(struct headA *listaAldea, struct headI *listaItem) {
    struct aldea *aldAsig = listaAldea->primero;
    struct item *itemAsig = listaItem->primero;
    int randN;
    // Mientras haya elementos por asignar, sigue en el bucle (OPTIMIZAR)
    while (mazSinAtac(listaAldea) != NULL) {
        // Siempre buscaremos una aldea disponible
        if (aldAsig->maz->itemDerrota != NULL) {
            aldAsig = mazSinAtac(listaAldea);
        }
        randN = rand();
        if (randN % 2 == 0) {
            // Enlaza el item que derrota a la mazmorra
            aldAsig->maz->itemDerrota = itemAsig;
            itemAsig->atac = aldAsig->maz;
        }
        itemAsig = itemAtacDisponible(listaItem);
        if (itemAsig == NULL) {
            itemAsig = listaItem->primero; // sigue atravesando!
        }
    }
}

int main() {
    int n;
    int randN = 0;
    srand(time(NULL)); // Inicializa la función random

    //Bienvenida
    printf("Bienvenido a ZELDA ROGUE, el juego favorito de los computistas :D\n");
    printf("Ingrese un numero de aldeas: ");
    
    //Numero de aldeas
    while(scanf("%d", &n) != 1 || n <= 1 || n > 64) {
        printf("Por favor, ingrese un numero de aldeas valido (entre 2 y 64). ");
        while(getchar() != '\n'); // Limpia el búfer de entrada
    }
    while(getchar() != '\n');
    
    //Creacion de listas
    int i;
    //aldea
    struct headA *listaAldea = (struct headA *)malloc(sizeof(struct headA));
    listaAldea->primero = NULL;
    //aldea del upside down
    struct headA *listaAedla = (struct headA *)malloc(sizeof(struct headA));
    listaAedla->primero = NULL;

    //item
    struct headI *listaItem = (struct headI *)malloc(sizeof(struct headI));
    listaItem->primero = NULL;
    //item del upside down
    struct headI *listaMeti = (struct headI *)malloc(sizeof(struct headI));
    listaItem->primero = NULL;

    for(i = 1; i <= n ; i++){
        if(n - i >= 4){ //caso que supera la cantidad de nombres que tenemos
            //aldeas e items normales
            struct mazmorra *nuevaMaz = crearMazmorra(i,mezcla_nombres(nombres[1],arr[n-i-4]));
            struct aldea *nuevaAldea = crearAldea(i,mezcla_nombres(nombres[0],arr[n-i-4]),nuevaMaz);
            struct item *nuevoItem = crearItem(i,mezcla_nombres(nombres[2],arr[n-i-4]));
            listaItem = agregarItem(nuevoItem, listaItem);
            listaAldea = agregarAldea(nuevaAldea, listaAldea);
            //aldeas e items del mundo paralelo
            struct mazmorra *nuevaZam = crearMazmorra(i,mezcla_nombres(nombresParalelos[1],arr[n-i-4]));
            struct aldea *nuevaAedla = crearAldea(i,mezcla_nombres(nombresParalelos[0],arr[n-i-4]),nuevaZam);
            struct item *nuevoMeti = crearItem(i,mezcla_nombres(nombresParalelos[2],arr[n-i-4]));
            listaMeti = agregarItem(nuevoMeti, listaMeti);
            listaAedla = agregarAldea(nuevaAedla, listaAedla);
        } else{
            //aldeas e items normales
            struct mazmorra *nuevaMaz = crearMazmorra(i,nombres[1][n-i]);
            struct aldea *nuevaAldea = crearAldea(i,nombres[0][n-i],nuevaMaz);
            struct item *nuevoItem = crearItem(i,nombres[2][n-i]);
            listaItem = agregarItem(nuevoItem, listaItem);
            listaAldea = agregarAldea(nuevaAldea, listaAldea);
            //aldeas e items del mundo paralelo
            struct mazmorra *nuevaZam = crearMazmorra(i,nombresParalelos[1][n-i]);
            struct aldea *nuevaAedla = crearAldea(i,nombresParalelos[0][n-i],nuevaZam);
            struct item *nuevoMeti = crearItem(i,nombresParalelos[2][n-i]);
            listaMeti = agregarItem(nuevoMeti, listaMeti);
            listaAedla = agregarAldea(nuevaAedla, listaAedla);
        }
    }

    // Enlazamiento 
    
    // Enlazar ubicación de items
    enlazarItemAldea(listaAldea, listaItem);
    enlazarItemMazmorra(listaAldea, listaItem); 
    if (itemDisponible(listaItem))
        verificarItemSobrante(listaAldea, listaItem);
    asignaItemMazmorra(listaAldea, listaItem); // Item que derrota la mazmorra

    // Ahora lo mismo para el mundo alternativo
    enlazarItemAldea(listaAedla, listaMeti);
    enlazarItemMazmorra(listaAedla, listaMeti);
    if (itemDisponible(listaMeti))
        verificarItemSobrante(listaAedla, listaMeti);
    asignaItemMazmorra(listaAedla, listaMeti);

    // Ahora enlaza los mundos paralelos
    conectarMundosParalelos(listaAldea, listaAedla);

    // Solo para verificar
    // struct aldea *ald = listaAldea->primero;
    // while (ald != NULL) {
    //     printf("Aldea: %s.",ald->nombre);
    //     if (ald->itemPresente == NULL)
    //         printf(" Sin objetos.");
    //     else
    //         printf(" Objeto: %s. ", ald->itemPresente->nombre);

    //     printf("Mazmorra: %s.",ald->maz->nombre);
    //     if (ald->maz->itemPresente == NULL)
    //         printf(" Sin objetos.\n");
    //     else
    //         printf(" Objeto: %s\n", ald->maz->itemPresente->nombre);
        
    //     ald = ald->sig;
    // }

    // Solo para verificar las mazmorras con item
    // struct aldea *ald1 = listaAldea->primero;
    // while (ald1 != NULL) {
    //     if (ald1->maz->itemDerrota == NULL)
    //         printf("Mazmorra: %s. Nada la derrota =(\n", ald1->maz->nombre);
    //     else
    //         printf("Mazmorra: %s. Objeto que la derrota: %s\n", ald1->maz->nombre, ald1->maz->itemDerrota->nombre);
    //     ald1 = ald1->sig;
    // }

    //Inicia el juego

    int vidas = 3, dinero = 0, vidaMax = 3, desbloqueo = n*3/4, mazmorrasDerrotadas = 0;
    int unlock = 0;//me dice si ya desbloqueo el mundo paralelo
    int actual = 1;//me dice si estoy en una aldea o mazmorra
    char comando[7];
    struct aldea *aldeaActual = listaAldea->primero;
    struct aldea *aldeaParalela = listaAedla->primero;
    struct mazmorra *mazmorraActual = aldeaActual->maz;

    //mensaje de bienvenida
    printf("\nYa estamos listos para la aventura!\n\nEn este momento te encuentras en una de %d aldeas llenas de vida y rincones por explorar...\n",n);
    printf("de las cuales cada una tiene su mazmorra, lugares tenebrosos llenos de bestias que hay que derrotar...\n");
    printf("Para derrotar a las mazmorras debes encontrar objetos que estan esparcidos por el mundo...\n");
    printf("Una vez derrotadas todas las mazmorras, habrás liberado al mundo de todos los males que lo someten ");
    printf("y serás el rey de la montaña!\nNo esperes más, el mundo te necesita!");
    printf("\n\nPresiona enter para comenzar"); while(getchar() != '\n');

    //inicio
    while(1){
        printf("\n\n-------------------------------------------------------------------------------------------\n\n");
        if(vidas > 0){
            if(actual){
                printf("Aldea actual: %s\tVidas: %d/%d \tDinero: %d$\n\n",aldeaActual->nombre, vidas, vidaMax, dinero);
                imprimirItems(listaItem,listaMeti,unlock);
                printf("\t\tM  A  P  A");    if(unlock)  printf("\tC  O  M  P  L  E  T  O");  printf(":\n");
                imprimirListaA(aldeaActual,listaAldea,listaAedla,unlock,actual);
                printf("Ingrese un comando (o help): ");
                fgets(comando, 7, stdin);

                if(strcmp(comando,"busq\n") == 0){
                    if(aldeaActual->itemPresente != NULL){
                        if(aldeaActual->itemPresente->encontrado == 0){
                            printf("Encontraste un ítem!!\n");
                            aldeaActual->itemPresente->encontrado = 1;
                        }else{
                            printf("Ya habías encontrado este ítem!!\n");
                            if(aldeaActual->maz->itemDerrota->ubicAldea != NULL)
                            printf("El item se encontraba en %s\n",aldeaActual->maz->itemDerrota->ubicAldea->nombre);
                            else if(aldeaActual->maz->itemDerrota->ubicMazmorra != NULL)
                            printf("El item se encontraba en %s\n",aldeaActual->maz->itemDerrota->ubicMazmorra->nombre);
                        }
                    }else{
                        if(aldeaActual->maz->itemDerrota->ubicAldea != NULL)
                            printf("En esta aldea no hay item :(\nEl item se encuentra en %s\n",aldeaActual->maz->itemDerrota->ubicAldea->nombre);
                        else if(aldeaActual->maz->itemDerrota->ubicMazmorra != NULL)
                            printf("En esta aldea no hay item :(\nEl item se encuentra en %s\n",aldeaActual->maz->itemDerrota->ubicMazmorra->nombre);
                    }
                    aldeaActual->buscado = true;
                    enter();
                    printf("Elegiste la opción busq\n");

                }else if(strcmp(comando,"maz\n") == 0){
                    actual = 0;     dinero += 10;
                    mazmorraActual = aldeaActual->maz;
                    pierdeVida(randN,&vidas,0);//por trasladarse
                    printf("Elegiste la opción maz\n");

                }else if(strcmp(comando,"compr\n") == 0){
                    comprar(&vidas, &dinero, &vidaMax, listaAldea);
                    printf("Elegiste la opción compr\n");

                }else if(strcmp(comando,"trans\n") == 0){
                    if(unlock){
                        aldeaActual = aldeaActual->trans;
                    }else{
                        printf("Opción BLOQUEADA, vuelve más adelante *.*\n");
                        enter();
                    } 
                    printf("Elegiste la opción trans\n");

                }else if(strcmp(comando,"ant\n") == 0){
                    if(aldeaActual->ant == NULL){
                        printf("Te encuentras en la primera aldea, ir mas atrás causaría una ramificación de la línea de tiempo :o\n");
                        enter();
                    }else{
                        dinero += 10;
                        aldeaActual = aldeaActual->ant;
                        pierdeVida(randN,&vidas,0);//por trasladarse
                    } 
                    printf("Elegiste la opción ant\n");

                }else if(strcmp(comando,"sig\n") == 0){
                    if(aldeaActual->sig == NULL){
                        printf("Te encuentras en la última aldea, ir mas adelante abriría un agujero de gusano que te llevaría a una eternidad en el vacío del espacio... :(\n");
                        enter();
                    }else{
                        dinero += 10;
                        aldeaActual = aldeaActual->sig;
                        pierdeVida(randN,&vidas,0);//por trasladarse
                    }
                    printf("Elegiste la opción sig\n");

                }else if(strcmp(comando,"help\n") == 0){
                    comandosA(unlock);
                    printf("Elegiste la opción help\n");

                }else if(strcmp(comando,"sal\n") == 0){
                    printf("Te fuiste sin derrotar el juego :(\n");
                    printf("\tG A M E  O V E R\nLa fuerza no te ha acompañado en esta ocasión :(\n");
                    break;
                }else{
                    printf("Opción no válida. Inténtalo de nuevo.\n");
                }
            }else{
                printf("Mazmorra actual: %s\tVidas: %d\tDinero: %d\n\n",mazmorraActual->nombre, vidas, dinero);
                imprimirItems(listaItem,listaMeti,unlock);
                printf("\t\tM  A  P  A");    if(unlock)  printf("\tC  O  M  P  L  E  T  O");  printf(":\n");
                imprimirListaA(aldeaActual,listaAldea,listaAedla,unlock,actual);
                printf("Ingrese un comando (o help): ");
                fgets(comando, 6, stdin);
                if(strcmp(comando,"busq\n") == 0){
                    if(mazmorraActual->buscado){
                        printf("Ya buscaste en esta mazmorra -.-\nPierdes una vida por eso!!\n");
                        vidas--;
                    }else{
                        if(mazmorraActual->itemPresente != NULL){
                            if(mazmorraActual->itemPresente->encontrado == 0){
                                printf("Encontraste un ítem!!\n");
                                mazmorraActual->itemPresente->encontrado = 1;
                            }else{
                                printf("Ya habías encontrado este ítem!!\n");
                            }
                        }else{
                            printf("En esta mazmorra no habia item :(\nSigue buscando\n");
                            printf("Aquí tienes 10$ por las molestias\n");
                            dinero += 10;
                        }
                        mazmorraActual->buscado = true;
                    }
                    enter();
                    printf("Elegiste la opción busq\n");

                }else if(strcmp(comando,"atac\n") == 0){
                    if(mazmorraActual->itemDerrota->encontrado == 1){
                        if(mazmorraActual->derrotada == true){
                            printf("Ya derrotaste esta mazmorra, ve a otra -.-\n");
                        }else{
                            printf("Has derrotado a esta mazmorra con éxito!\n");
                            mazmorrasDerrotadas++;  mazmorraActual->derrotada = true;
                        }
                    }else{
                        printf("Aún no tienes el ítem que derrota esta mazmorra, cosíguelo e inténtalo de nuevo.\n");
                        printf("Por este intento fallido, pierdes una vida </3\n");
                        vidas--;
                    }
                    enter();
                    if(!unlock){
                        if(mazmorrasDerrotadas == desbloqueo){
                            mensajeparalelo();
                            unlock = 1; actual = 1;
                            aldeaActual = aldeaParalela;
                        }
                    }
                    if(mazmorrasDerrotadas == 2*n) {
                        //ganaste
                        printf("¡Felicidades! Tus esfuerzos no han sido en vano: derroaste a todas las mazmorras y salvaste el mundo. \n");
                        break;
                    }
                    printf("Elegiste la opción atac\n");

                }else if(strcmp(comando,"ant\n") == 0){
                    actual = 1;     dinero += 10;
                    pierdeVida(randN,&vidas,0);//por trasladarse
                    pierdeVida(randN,&vidas,1);//por realizar la accion
                    printf("Elegiste la opción ant\n");

                }else if(strcmp(comando,"sig\n") == 0){
                    if(mazmorraActual->sig == NULL){
                        printf("Te encuentras en la mazmorra de la última aldea, ir mas adelante causaría un ruptura en el espacio-tiempo :s\n");
                        enter();
                    }else{
                        dinero += 10;     actual = 1;   aldeaActual = aldeaActual->sig;
                        pierdeVida(randN,&vidas,0);//por trasladarse
                    }
                    pierdeVida(randN,&vidas,1);//por realizar la accion
                    printf("Elegiste la opción sig\n");

                }else if(strcmp(comando,"help\n") == 0){
                    comandosM();
                    printf("Elegiste la opción help\n");
                
                }else if(strcmp(comando,"sal\n") == 0){
                    printf("Te fuiste sin derrotar el juego :(\n");
                    printf("\tG A M E  O V E R\nLa fuerza no te ha acompañado en esta ocasión :(\n");
                    break;
                }else{
                    printf("Opción no válida. Inténtalo de nuevo.\n");
                }
            }
        }else{
            printf("Perdiste todas las vidas </3\n\n");
            printf("\tG A M E  O V E R\nLa fuerza no te ha acompañado en esta ocasión :(\n");
            break;
        }
    }
}
