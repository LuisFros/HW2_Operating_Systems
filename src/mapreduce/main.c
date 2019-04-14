// Importe de librerías
#include "words.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <math.h>


#define MAXCHAR 50

typedef struct MapArgs {
    WordArray *array;
    char *line;
    // WordHash **accumulated_result;
}MapArgs;


struct reduce_struct{
    WordArray *boss1;
    WordArray *boss2;
}ReduceArgs;

// struct reduce_struct Reduce;

int current_line=0;
int current_reduce=0;

int n_lines=0;
double height=0;

void print_array(WordHash **array,int size){
    for(int i=0;i<size;i++){
        printf("WORD: %s | FREQ: %d \n",array[i]->word,array[i]->frequency);
    }
}
void insert_sort(WordHash **arr,int n){
    // https://www.geeksforgeeks.org/insertion-sort/
    int i, j; 
    for (i = 1; i < n; i++) { 
        WordHash *key = arr[i]; 
        j = i - 1; 
  
        /* Move elements of arr[0..i-1], that are 
          greater than key, to one position ahead 
          of their current position */
        while (j >= 0 && arr[j]->frequency > key->frequency) { 
            arr[j + 1] = arr[j]; 
            j = j - 1; 
        } 
        arr[j + 1] = key; 
    } 
}
char * get_next_line(FILE * input_file){
        bool ignore_next;
        int c, index = 0;
        int size=0;
        char *temp=NULL;
        char *line=NULL;
        while ( (c = fgetc (input_file)) != EOF ){
            if ( c == '\n'){
                // Agregar ===' ' para una palabra
                // Se termina una linea por el salto de linea.
                // Crear un thread aca
                ignore_next=true;
                break;
            }
            if(size<=index){
                size+=8;
                // alocar memoria de manera dinamica 
                temp=realloc(line,size);
                if(!temp){
                    free(line);
                    line=NULL;
                    break;
                }
                line=temp;
            }
            // for ( ; *p; ++p) *p = tolower(*p);
                // printf("%c",tolower(c));
            if (!ignore_next){
                // Para no agregar el salto de linea
                line[index++]=c;
            }
            else{
                ignore_next=false;
            }
            
        }  // EO While
    return line;
    } // EO Function

int compare_function(const void *a,const void *b) {
    WordHash *x = (WordHash *) a;
    WordHash *y = (WordHash *) b;
    if ( x->frequency < y->frequency )
    {
        return -1;
    }
    else if ( y->frequency < x->frequency )
    {
        return 1;
    }
    else
    {
        if ( x->frequency < y-> frequency )
        {
            return 1;
        }
        else if ( y->frequency < x->frequency )
        {
            return -1; 
        }
        else
        {
            return 0;
        }
    }
   
}
// Convierte dirty_string a lowercase
char * strlow(char *dirty_string){
    for(int i=0;dirty_string[i];i++){
        dirty_string[i]=tolower(dirty_string[i]);
    }
    return dirty_string;
}


void * reduce(void *arguments){
    printf("entra\n");
    struct reduce_struct *args = arguments;
    WordArray *boss1=(WordArray*)args->boss1;
    WordArray *boss2=(WordArray*)args->boss2;
    int size1=boss1->size;
    int size2=boss2->size;
    bool shouldInsert=false;
    int position_found=0;
    WordHash **static_array=boss1->elements;
    WordHash **iterated_array=boss2->elements;
    WordHash **temp=NULL;  
    int current_word=0;
    while(size2>0){
        char *word=iterated_array[current_word]->word;
        shouldInsert=true;
        // Iteramos sobre el array statico para ver si existe la palabra
        for(int w=0;w<size1;w++){
            // buscamso si la palabra ya exite
            // La funcion strcmp(s1,s2) retorna 0 si son distntos los strings s1 y s2
            if(strcmp(static_array[w]->word, word) == 0){
                shouldInsert=false;
                position_found=w;
            }
        }
        if(shouldInsert){
            // inseratmos la palabra en al array
            if (strlen(word)>0){
                temp=realloc(static_array,sizeof(WordHash)*(size1+1));
                if(!temp){
                    // NULL POINTER!, NOT ENOUGH MEMORY?
                    free(static_array);
                    static_array=NULL;
                    break;
                }
                static_array=temp;
                static_array[size1]=malloc(sizeof(WordHash));
                strcpy(static_array[size1]->word,word);
                static_array[size1]->frequency=1;
                // incrementams el tamaño del array statico
                size1++;
            }
        }
        else{
            // debemos aumentar la frecuencia.
            static_array[position_found]->frequency+=1;
        }
        word=iterated_array[current_word++]->word;
        size2--;
    }
    insert_sort(static_array,size1);
    args->boss1->elements=static_array;
    pthread_exit(args);
}


// Entrega a WordBoss un array arreglado de palabras de la linea *line

// void * wordcounter(void *wordboss_void,void *line_void){

void * wordcounter(void *arguments){
    MapArgs *args = (MapArgs*)arguments;

    WordArray *wordboss=(WordArray *)args->array;
    // char *line_not_void=(char *)args->line;
    // // char *line=strcpy();
    
    // int lineLength = strlen(line_not_void);
    // char *line = (char*) calloc(lineLength + 1, sizeof(char));
    // strncpy(line_not_void, line, lineLength);

    char delim[]=" ";
    char *context;
   
    // printf("Linea que entra %s \n",line);
    // printf("Linea que entra %p \n",wordboss[0]);

    // int inputLength = strlen(line);
    // char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    // strncpy(inputCopy, line, inputLength);
    // line[inputLength]='\0';

    // // char *word = strlow(strtok_r (line, delim, &context));
    char *word = strtok_r (args->line, delim, &context);

    bool shouldInsert=false;
    int position_found=0;


    WordHash **temp=NULL;
    WordHash **wordarray=malloc(sizeof(WordHash));
    int boss_size=wordboss->size;
    int size=0;

    if(boss_size>0){
        wordarray=realloc(wordarray,sizeof(wordboss->elements));
        wordarray=wordboss->elements;
        size=boss_size;
    }
    
    

    while(word!=NULL){
        shouldInsert=true;
        // printf("value is %i \n",size);

        for(int w=0;w<size;w++){
            // buscamso si la palabra ya exite
            // La funcion strcmp(s1,s2) retorna 0 si son distntos los strings s1 y s2
            if(strcmp(wordarray[w]->word, word) == 0){
                shouldInsert=false;
                position_found=w;
            }

        }
        if(shouldInsert){
            // inseratmos la palabra en al array
            if (strlen(word)>0){
                temp=realloc(wordarray,sizeof(WordHash)*(size+1));
                if(!temp){
                    // NULL POINTER!, NOT ENOUGH MEMORY?
                    free(wordarray);
                    wordarray=NULL;
                    break;
                }
                wordarray=temp;
                wordarray[size]=malloc(sizeof(WordHash));
                strcpy(wordarray[size]->word,word);
                wordarray[size]->frequency=1;
                size++;
            }
        }
        else{
            // debemos aumentar la frecuencia.
            wordarray[position_found]->frequency+=1;
        }

        word=strtok_r(NULL,delim,&context);
    }

    insert_sort(wordarray,size);
    // qsort(wordarray,size,sizeof(WordHash),compare_function);
    wordboss->elements=wordarray;
    wordboss->size=size;
    // args->accumulated_result=wordarray;
    args->array=wordboss;
    
    pthread_exit(args);
    // free(line);

    
}



// Función principal.
int main(int argc, char *argv[]) {   
    if(0){

        printf("No hay suficientes argumentos!\n");
		return 0;
    }
    // inicializamos el puntero al arreglo de procesos

    /* Abrimos el archivo input en modo de lectura */
	FILE *input_file = fopen("text1.txt","r");

	/* Abrimos el archivo output en modo de escritura */
	// FILE *output_file = fopen(argv[2], "w");




    if (!input_file)
	{
		printf("¡El archivo %s no existe!\n", argv[1]);
		return 2;
	}


    char x;
    // Primero obtenemos el numero de procesos que nos van a entregar
    for (x = getc(input_file); x != EOF; x = getc(input_file)) 
        if (x == '\n') // Increment count if this character is newline 
            n_lines++;
  
    // reseteamos el puntero al inicio del archivo..
    rewind(input_file);

    int n_nodes;
    int n_reduce;
    if (n_lines%2==0){
        n_reduce=(int)(n_lines/2);
        n_nodes=(2*n_lines)-1;
        // Es par
    }
    else{
        n_reduce=(int)((n_lines-1)/2)+1;
        n_nodes=(2*n_lines);
    }
    height=ceil(log2(n_nodes))-1;

    //  A lo mas una palabra tiene largo 43 char.
    // char *line=NULL;

    // WordHash **wordarray=init_array(n_words);
    

    // printf("value of wordhash %i \n",sizeof(WordHash));
//  -4 es por el int de frequency.
    // int size_chars=sizeof(WordHash)-4;


    pthread_t maps[n_lines];
    
    pthread_t reduces[n_reduce];

    WordArray *boss=malloc(sizeof(WordArray));
    boss->size=0;
    boss->elements=NULL;
    // args.arg1=boss;

	// pthread_attr_t attr;
	// pthread_attr_init(&attr);
    // wordcounter(boss);
    

    bool ignore_next;
    char *line=NULL;

    int m;
        // Creamos un arreglo de arreglos (Matriz) para guardar los map
    // WordArray **MapMatrix=calloc(sizeof(WordArray),n_lines);
    MapArgs **MapMatrix=calloc(sizeof(MapArgs),n_lines);
    // init pointers
    for(m=0;m<n_lines;m++){
        MapMatrix[m]=malloc(sizeof(MapArgs));
        line=get_next_line(input_file);
        MapMatrix[m]->line=calloc(sizeof(char),strlen(line));
        strcpy(MapMatrix[m]->line,line);
        // strcpy(MapMatrix[m]->line,line);
        MapMatrix[m]->array=malloc(sizeof(WordHash));
    }
    printf("Matrix initialized\n");
    n_lines=1;
    for(m=0;m<n_lines;m++){

        // Para cada linea, creamos un map
       
        if(pthread_create(&maps[m], NULL, (void *)wordcounter, (void*)MapMatrix[m])) {
            fprintf(stderr, "Error creating thread\n");
        return 1;
        }
    }

    for(m=0;m<n_lines;m++){
        // Para cada map, hacemos un join para esperarlos
        if(pthread_join(maps[m], NULL) ){
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }
    printf("Matrix value created %s \n",MapMatrix[0]->array->elements[7]->word);



    // while(0){
    //     if (height==0){
    //         printf("Se termino el arbol %i \n");
    //     }
    //     // SIGINT!!!
    //     // Funcion para obtener las lineas
        
        
    //     // 
        
    //     if (ferror (input_file) ){
    //         printf ("Read Error\n"), exit (EXIT_FAILURE);

    //     }
        


    //     args.arg2=line;
    //     if((current_line>0) && ((current_line)%2==0)){
    //         if( (pthread_join(maps[current_line-2], NULL)) && (pthread_join(maps[current_line-1], NULL)) ){
    //             fprintf(stderr, "Error joining thread\n");
    //             return 2;
    //         };

    //         pthread_create(&reduces[current_reduce], NULL, (void *)reduce, (void*)&Reduce);
    //         current_reduce++;
    //         // SE CREA UN THREAD DE REDUCE
    //         // Aca se deberia crear el 
    //     }
    //     // caso donde el numero de MAPS es impar
    //     else if((current_line>0) && (current_reduce==n_reduce-1)){
    //          if((pthread_join(maps[current_line-1], NULL)) ){
    //             fprintf(stderr, "Error joining thread\n");
    //             return 2;
    //         };
    //         printf("Aca se crea un Reduce\n");
    //         pthread_create(&reduces[current_reduce], NULL, (void *)reduce, (void*)&args);
    //         current_reduce++;
    //     }
    //     // SE CREA UN THREAD THE MAP
        
    //     printf("CREATED THREAD N_%i\n",current_line);
    //     // Update values for loops
    //     current_line++;
    //     // BORRAR ===================
    //     if(current_line==3){
    //         break;
    //     }
    //     // END ======================
    //     line=NULL;
    //     size=0;
    //     index=0;
    //     temp=NULL;

    // }
    return 0;

}