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

char * copyStr(char s[])
{
  size_t len = strlen2(s); //find length of s
  char * copy; 
  copy = (char *)malloc(len + 1); //dynamically allocate memory
                           /* One more char must be allocated for the null char */

  size_t i; 
  for(i=0;i<len;i++) 
   {
     copy[i]=s[i];  //copy characters               
   }
   copy[i] = '\0'; // HERE
   return copy; //return address
}

typedef struct ReduceArgs{
    WordArray *result;
    WordArray *array2;
    WordArray *array1;
}ReduceArgs;

// struct reduce_struct Reduce;

int current_line=0;
int current_reduce=0;
MapArgs **MapMatrix;
int n_lines=0;
double height=0;

int strlen2(char *s)
{
    int x=0;
    while (*s++)
        x++;
    return(x);
}


int * get_asigned_maps(int n_maps,int n_reduce,int current_reduce){
    int *asigned=malloc(sizeof(int)*2);
    if( ((n_maps%2)!=0) && ( (n_reduce-1)==current_reduce) ){
        asigned[0]=current_reduce*2;
        // -1 es para avisa que no hay elemento
        asigned[1]=-1;
        }
    else{
        // asigned=malloc(sizeof(int)*2);
        asigned[0]=current_reduce*2;
        asigned[1]=(current_reduce*2)+1;
        }
    return asigned;
}
int * get_asigned_reduce(int n_reduce_prev,int n_reduce,int current_reduce){
    int *asigned=malloc(sizeof(int)*2);
    // Si es el nodo raiz;
    int left_child;
    int right_child;
    if (n_reduce==1){
        asigned[0]=current_reduce-2;
        asigned[1]=current_reduce-1;
        return asigned;
    }
    else{
        left_child=2*(current_reduce-n_reduce_prev);
        right_child=left_child+1;

    }
    asigned[0]=left_child;
    int theorethical_child=current_reduce-n_reduce+1;
    if ((n_reduce_prev%2!=0)&&(theorethical_child==right_child)){
        asigned[1]=-1;
    }
    else{
        asigned[1]=right_child;
    }
    return asigned;
}
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
        int c, index = 0;
        int size=0;
        char *temp=NULL;
        char *tempfinal=NULL;
        char *line=NULL;
        while ( (c = fgetc (input_file)) != EOF ){
            if ( c == '\n'){
                // Agregar ===' ' para una palabra
                // Se termina una linea por el salto de linea.
                // Crear un thread aca
                break;
            }
            if(size<=index){
                size+=1;
                // alocar memoria de manera dinamica 
                temp=realloc(line,size+1);
                if(!temp){
                    free(line);
                    line=NULL;
                    break;
                }
                line=temp;
            }
            // for ( ; *p; ++p) *p = tolower(*p);
                // printf("%c",tolower(c));
            // if (!ignore_next){
            //     // Para no agregar el salto de linea
            line[index++]=c;
            // }
            // else{
                
            //     ignore_next=false;
            // }
        }  // EO While

    tempfinal=malloc(size+1);
    tempfinal=line;
    return tempfinal;
    } // EO Function


// Convierte dirty_string a lowercase
char * strlow(char *dirty_string){
    for(int i=0;dirty_string[i];i++){
        dirty_string[i]=tolower(dirty_string[i]);
    }
    return dirty_string;
}


void * reduce(void *arguments){

  
    printf("entra\n");
    ReduceArgs *args =(ReduceArgs*) arguments;

    // WordArray *result=args->result;
    WordArray *boss1=(WordArray*)args->array1;
    WordArray *boss2=(WordArray*)args->array2;
    
    int size1=(int)boss1->size;
    int size2=(int)boss2->size;
    bool shouldInsert=false;
    int position_found=0;
    WordHash **static_array=boss1->elements;
    WordHash **iterated_array=boss2->elements;
    WordHash **temp=NULL;  
    int current_word=0;
    while(true){
        if(boss2->size==0){
            break;
        }
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
            if (strlen2(word)>0){
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
        boss2->size--;
        if(size2==0){
            printf("brea\n");
        }
    }
   
    insert_sort(static_array,size1);
    args->result->elements=malloc(sizeof(static_array));
    args->result->elements=static_array;
    pthread_exit(args);
}


// Entrega a WordBoss un array arreglado de palabras de la linea *line

// void * wordcounter(void *wordboss_void,void *line_void){

void * wordcounter(void *arguments){
    MapArgs *args = (MapArgs*)arguments;

    WordArray *wordboss=(WordArray *)args->array;
    wordboss=malloc(sizeof(WordArray));

    // char *line_not_void=(char *)args->line;
    // // char *line=strcpy();
    
    // int lineLength = strlen2(line_not_void);
    // char *line = (char*) calloc(lineLength + 1, sizeof(char));
    // strncpy(line_not_void, line, lineLength);

    char delim[]=" ";
    char *context;
   
    // printf("Linea que entra %s \n",line);
    // printf("Linea que entra %p \n",wordboss[0]);

    // int inputLength = strlen2(line);
    // char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    // strncpy(inputCopy, line, inputLength);
    // line[inputLength]='\0';

    // // char *word = strlow(strtok_r (line, delim, &context));
    char *word = strtok_r (args->line, delim, &context);

    bool shouldInsert=false;
    int position_found=0;


    WordHash **temp=NULL;
    WordHash **wordarray=NULL;
    int size=0;

    // if(boss_size>0){
    //     wordarray=realloc(wordarray,sizeof(wordboss->elements));
    //     wordarray=wordboss->elements;
    //     size=boss_size;
    // }
    
    

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
            if (strlen2(word)>0){
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
    wordboss->elements=malloc(sizeof(WordHash*)*size);
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
    

    int chars_on_current_line=0;
    for (x = getc(input_file); x != EOF; x = getc(input_file)) 
        if (x == '\n'){// Increment count if this character is newline 
            n_lines++;
            chars_on_current_line=0;
        }
        else{
            chars_on_current_line++;
            }
    if(chars_on_current_line>0) n_lines++;
    
    // printf("Curent %i\n",n_lines);
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

    // n_reduce=n_nodes-n_lines;


    // boss->size=0;
    // boss->elements=NULL;
    // args.arg1=boss;

	// pthread_attr_t attr;
	// pthread_attr_init(&attr);
    // wordcounter(boss);
    pthread_t maps[n_lines];
    pthread_t reduces[n_nodes-n_reduce];

    
    bool ignore_next;
    char *line=NULL;
    int m;
        // Creamos un arreglo de arreglos (Matriz) para guardar los map
    // WordArray **MapMatrix=calloc(sizeof(WordArray),n_lines);
    MapMatrix=calloc(sizeof(MapArgs),n_lines);
    // init pointers
    for(m=0;m<n_lines;m++){
        MapMatrix[m]=malloc(sizeof(MapArgs));
        line=get_next_line(input_file);
        printf("value is %i M:%i\n",strlen2(line),m);
        MapMatrix[m]->line=copyStr(line);
        // MapMatrix[m]->line=malloc(sizeof(char)*(strlen2(line)+1));
        // strcpy(MapMatrix[m]->line,line);
        // strcpy(MapMatrix[m]->line,line);
        MapMatrix[m]->array=NULL;
        line=NULL;
    }
    printf("Matrix initialized\n");
    // n_lines=2;
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
    fclose(input_file);

    // print_array(MapMatrix[0]->array->elements,MapMatrix[0]->array->size);
    // printf("---------------\n");
    // print_array(MapMatrix[1]->array->elements,MapMatrix[1]->array->size);
    

    int *assigned=NULL;

    ReduceArgs **ReduceMatrix=calloc(sizeof(ReduceArgs),n_nodes-n_reduce);
    int r;

    for(r=0;r<n_reduce;r++){
        ReduceMatrix[r]=malloc(sizeof(ReduceArgs));
        assigned=get_asigned_maps(n_lines,n_reduce,r);
        ReduceMatrix[r]->array1=malloc(sizeof(MapMatrix[assigned[0]]->array));
        ReduceMatrix[r]->array1=MapMatrix[assigned[0]]->array;
        if(assigned[1]!=-1){
            ReduceMatrix[r]->array2=malloc(sizeof(MapMatrix[assigned[1]]->array));
            ReduceMatrix[r]->array2=MapMatrix[assigned[1]]->array;
            ReduceMatrix[r]->result=malloc(sizeof(WordArray));
            pthread_create(&reduces[r],NULL,(void *)reduce,(void*)ReduceMatrix[r]);
        }
        else{
            ReduceMatrix[r]->array2=NULL;
            // No es necesario hacer un thread
            ReduceMatrix[r]->result=malloc(sizeof(MapMatrix[assigned[0]]->array));
            ReduceMatrix[r]->result=MapMatrix[assigned[0]]->array;
        }
        // strcpy(ReduceMatrix[m]->line,line);
        
        // current_reduce++;
    }
        // Join threads
    for(m=0;m<n_reduce;m++){
        // Para cada map, hacemos un join para esperarlos
        assigned=get_asigned_maps(n_lines,n_reduce,m);
        if(assigned[1]!=-1){
            if(pthread_join(reduces[m], NULL) ){
                fprintf(stderr, "Error joining thread\n");
                return 2;
            }
        }
    }
    int base=n_reduce;

    n_lines=n_reduce;
    if (n_reduce%2==0){
        n_reduce=(int)(n_reduce/2);
    }
    else{
        n_reduce=(int)((n_reduce-1)/2)+1;
    }
    // base+=n_reduce;
    height--;
        
    while(height>0){
        // maps=realloc(maps,sizeof(pthread_t)*n_lines);
        // reduces=realloc(reduces,sizeof(pthread_t)*n_reduce);
        // Create threads
        for(r=base;r<n_reduce+base;r++){
            ReduceMatrix[r]=malloc(sizeof(ReduceArgs));
            assigned=get_asigned_reduce(n_lines,n_reduce,r);
            ReduceMatrix[r]->array1=malloc(sizeof(ReduceMatrix[assigned[0]]->result));
            ReduceMatrix[r]->array1=ReduceMatrix[assigned[0]]->result;
            if(assigned[1]!=-1){
                ReduceMatrix[r]->array2=malloc(sizeof(ReduceMatrix[assigned[1]]->result));
                ReduceMatrix[r]->array2=ReduceMatrix[assigned[1]]->result;
                ReduceMatrix[r]->result=malloc(sizeof(WordArray));
                pthread_create(&reduces[r],NULL,(void *)reduce,(void*)ReduceMatrix[r]);
            }
            else{
                ReduceMatrix[r]->array2=NULL;
                // No es necesario hacer un thread
                ReduceMatrix[r]->result=malloc(sizeof(ReduceMatrix[assigned[0]]->result));
                ReduceMatrix[r]->result=ReduceMatrix[assigned[0]]->result;
            }
            // strcpy(ReduceMatrix[m]->line,line);
            
            // current_reduce++;
        }
        // Join threads
        for(m=base;m<n_reduce+base;m++){
            // Para cada map, hacemos un join para esperarlos
            assigned=get_asigned_reduce(n_lines,n_reduce,m);
            if(assigned[1]!=-1){
                if(pthread_join(reduces[m], NULL) ){
                    fprintf(stderr, "Error joining thread\n");
                    return 2;
                }
            }
        }
        n_lines=n_reduce;
        base+=n_reduce;

        if (n_reduce%2==0){
            n_reduce=(int)(n_reduce/2);
        }
        else{
            n_reduce=(int)((n_reduce-1)/2)+1;
        }
        height--;
    }
    // printf(ReduceMatrix[n_nodes-n_reduce]);
    printf("DONE\n");
    // printf("Matrix value created %s \n",MapMatrix[0]->array->elements[7]->word);



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
   pthread_exit(NULL); 

}