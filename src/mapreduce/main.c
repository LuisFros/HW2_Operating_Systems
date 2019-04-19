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
// Arreglar problemas con UTF-8
#include <wchar.h>
#include <locale.h>

#define MAXCHAR 50

pthread_mutex_t lock1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock4=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock5=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock6=PTHREAD_MUTEX_INITIALIZER;

void clean(char *s){
    int x=0;
    while (*s++)
        if((*s==201) || (*s==187)){
            *s='\0';
        }
    return(x);
}

int valid_identifier_start(char ch)
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'));
}
typedef struct MapArgs {
    WordArray *array;
    char *line;
    // WordHash **accumulated_result;
}MapArgs;

char * copyStr(char *s)
{

  int len = strlen(s); //find length of s
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

int height=0;

// char  get_next_word(char *line,char *result){
//     int word_idx=0;
//     while(*line!=' '){
//         if(*line=='\n'){
//             break;
//         }
//         result[word_idx]=*line;
//         word_idx++;
//         line++;
//     }
//     return *result;
// }

int strlen2(char *s)
{
    int x=0;
    while (*s++)
        x++;
    return(x);
}

void delete_word(WordHash *word){
    free(word->word);
    free(word);
}

void delete_array(WordArray *array){
    // for(int k=0;k<array->size;k++){
    //     delete_word(array->elements[k]);
    // }
    free(array);
}

void delete_matrix_map(MapArgs **matrix,int n_maps){
    for(int row=0;row<n_maps;row++){
        delete_array(matrix[row]->array);
        free(matrix[row]->line);
        free(matrix[row]);
    }
    free(matrix);
}

int * get_asigned_maps(int n_maps,int n_reduce,int current_reduce,int *maps_assigned){
    int *asigned=malloc(sizeof(int)*2);
    int diff=n_reduce-current_reduce;
    if (diff>=2){
        // Adjust for idx , -1 
        asigned[0]=*maps_assigned;
        asigned[1]=*maps_assigned+1;
        *maps_assigned+=2;
    }
    else{
        asigned[0]=*maps_assigned;
        asigned[1]=-1;
        *maps_assigned+=1;
    }

    
    return asigned;
    // if( ((n_maps%2)!=0) && ( (n_reduce-1)==current_reduce) ){
    //     asigned[0]=current_reduce*2;
    //     // -1 es para avisa que no hay elemento
    //     asigned[1]=-1;
    //     }
    // else{
    //     // asigned=malloc(sizeof(int)*2);
    //     asigned[0]=current_reduce*2;
    //     asigned[1]=(current_reduce*2)+1;
    //     }
    // return asigned;
}
int * get_asigned_reduce(bool * av_array,int size_array){
    int * asigned=calloc(sizeof(int),2);
    int found=0;
    int pos1=-1;
    int pos2=-1;

    for (int b=0;b<size_array;b++){
        if (found==2){
            break;
        }
        if(av_array[b]==true){
            if(pos1!=-1){
                pos2=b;
            }
            else{
                pos1=b;
            }
            av_array[b]=false;
            found+=1;
        }
    }
    if(found==2){
        asigned[0]=pos1;
        asigned[1]=pos2;
    }
    else{
        asigned[0]=pos1;
        asigned[1]=-1;
    }
    return asigned;
}
void print_array(WordHash **array,int size){
    for(int i=0;i<size;i++){
        // printf("%i| WORD: %s | FREQ: %d \n",i,array[i]->word,array[i]->frequency);
        printf("%d,%s\n",array[i]->frequency,array[i]->word);

    }
}

void write_array(WordHash **array,int size, FILE *out){
    for(int i=0;i<size;i++){
        // printf("%i| WORD: %s | FREQ: %d \n",i,array[i]->word,array[i]->frequency);
        fprintf(out,"%d,%s\n",array[i]->frequency,array[i]->word);

    }
}
void print_array2(WordHash **array,int size){
    printf("%d,[%s",array[0]->frequency,array[0]->word);
    for(int i=1;i<size;i++){
        // printf("%i| WORD: %s | FREQ: %d \n",i,array[i]->word,array[i]->frequency);
        printf(";%s",array[i]->word);
    }
    printf("]\n");

}
void write_array2(WordHash **array,int size,FILE *out){
    fprintf(out,"%d,[%s",array[0]->frequency,array[0]->word);
    for(int i=1;i<size;i++){
        // printf("%i| WORD: %s | FREQ: %d \n",i,array[i]->word,array[i]->frequency);
        fprintf(out,";%s",array[i]->word);
    }
    fprintf(out,"]\n");

}


void *freqcounter(void * arguments){
    // printf("ENTRA A FREQ\n");
    MapArgs *args = (MapArgs*)arguments;

    WordArray *result=(WordArray *)args->array;

    WordHash **temp=NULL;
    int size=0;
    WordHash **new_array=calloc(sizeof(WordHash),1);
    // wordboss=malloc(sizeof(WordArray));

    int freq=(int)args->line;
    // printf("value is %i \n",freq);
    int starting_pos=0;
    bool found=false;
    while(true){
        // printf("starting_pos is %i \n",starting_pos);
        if(starting_pos==result->size-1){
            // printf("FREQUENCIA no existe\n");
            break;
        }
        if(result->elements[starting_pos]->frequency==freq){
            // printf("starting_pos is %i \n",starting_pos);
            found=true;
            break;
        }
        starting_pos++;
    }
    if(found){
        int pos;
        // printf("starting_pos is %i \n",starting_pos);
        int idx=starting_pos;
        for(;result->elements[idx]->frequency==freq;idx++){
            // printf("current idx is %i \n",idx);
            pos=idx-starting_pos;
            if(size<=pos){
                temp=realloc(new_array,sizeof(WordHash)*(size+1));
                    if(!temp){
                        // NULL POINTER!, NOT ENOUGH MEMORY?
                        free(new_array);
                        new_array=NULL;
                        break;
                    }
                    new_array=temp;
            }
            new_array[pos]=result->elements[idx];
            size++;
        }
        // print_array(new_array,size);
    }
    args->array->elements=new_array;
    args->array->size=size;
    // printf("ASDASDSAD\n");
    // // char *line=strcpy();
    return 0;
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
        char c;
        int index = 0;
        int size=0;
        char *temp=NULL;
        char *tempfinal=NULL;
        char *line=NULL;
        pthread_mutex_lock(&lock1);
        while ( (c = fgetc (input_file)) != EOF ){
            if ( c == '\n' && line){
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
            if(c!='\n' && c!='\r'){
                if (valid_identifier_start(c)||c==' '){
                    line[index++]=tolower(c); 
                }
                else{
                    // printf("value i %c \n",c);
                    continue;
                }

            }
            
            // }
            // else{
                
            //     ignore_next=false;
            // }
        }  // EO While

    // tempfinal[size]='\0';
    pthread_mutex_unlock(&lock1);
    line[size]='\0';
    return line;
    } // EO Function


// Convierte dirty_string a lowercase
char * strlow(char *dirty_string){
   
    for(int i=0;dirty_string[i] && isalpha(i) ;i++){
        dirty_string[i]=tolower(dirty_string[i]);
    }
    return dirty_string;
}


void * reduce(void *arguments){

  
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
            static_array[position_found]->frequency+=iterated_array[current_word]->frequency;
        }
        word=iterated_array[current_word++]->word;
        boss2->size--;
        if(size2==0){
            printf("brea\n");
        }
    }
   
    insert_sort(static_array,size1);
    // printf("-------------\n");
    // print_array(static_array,size1);

    // printf("-------------\n");
    
    args->result=init_array(size1);
    args->result->elements=static_array;
    // print_array(args->result->elements,size1);
    pthread_exit(args);
}


// Entrega a WordBoss un array arreglado de palabras de la linea *line

// void * wordcounter(void *wordboss_void,void *line_void){

void * wordcounter(void *arguments){
    MapArgs *args = (MapArgs*)arguments;

    WordArray *wordboss=(WordArray *)args->array;
    
    // wordboss=malloc(sizeof(WordArray));

    char *line=(char *)args->line;
    // // char *line=strcpy();
    
    // int lineLength = strlen2(line_not_void);
    // char *line = (char*) calloc(lineLength + 1, sizeof(char));
    // strncpy(line_not_void, line, lineLength);

    const char *delim=" ";
    char *context;
   
    // printf("Linea que entra %s \n",line);
    // printf("Linea que entra %p \n",wordboss[0]);

    // int inputLength = strlen2(line);
    // char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    // strncpy(inputCopy, line, inputLength);
    // line[inputLength]='\0';

    // // char *word = strlow(strtok_r (line, delim, &context));
    char *word=strtok_r(line,delim,&context);
    bool shouldInsert=false;
    int position_found=0;


    WordHash **temp=NULL;
    WordHash **wordarray=wordboss->elements;
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
            if (strlen(word)>0){
                pthread_mutex_lock(&lock3);
                temp=realloc(wordarray,sizeof(WordHash)*(size+1));
                if(!temp){
                    // NULL POINTER!, NOT ENOUGH MEMORY?
                    free(wordarray);
                    wordarray=NULL;
                    break;
                }
                wordarray=temp;
                wordarray[size]=malloc(sizeof(WordHash));
                // int i,j;
                // for(i = 0; word[i] != '\0'; ++i)
                // {
                //     while (!( (word[i] >= 'a' && word[i] <= 'z') || (word[i] >= 'A' && word[i] <= 'Z') || word[i] == '\0') )
                //     {
                //         for(int j = i; word[j] != '\0'; ++j)
                //         {
                //             word[j] = word[j+1];
                //         }
                //         word[j] = '\0';
                //     }
                // }

                strcpy(wordarray[size]->word,word);
                // memcpy(wordarray[size]->word,word);
                pthread_mutex_unlock(&lock3);
                wordarray[size]->frequency=1;
                size++;
            }
        }
        else{
            // debemos aumentar la frecuencia.
            wordarray[position_found]->frequency+=1;
        }
        pthread_mutex_lock(&lock4);
        word=strtok_r(NULL,delim,&context);
        pthread_mutex_unlock(&lock4);

    }

    insert_sort(wordarray,size);
    // qsort(wordarray,size,sizeof(WordHash),compare_function);
    // print_array(wordarray,size);
    // pthread_mutex_lock(&lock5);
    // temp=realloc(wordboss->elements,size);
    // if(!temp){
    //     free(wordarray);
    //     wordarray=NULL;
    //     printf("NULL PINTER \n");
    // }
    // pthread_mutex_unlock(&lock5);

    // wordboss->elements=temp;
    wordboss->elements=wordarray;
    wordboss->size=size;
    // args->accumulated_result=wordarray;
    args->array=wordboss;
    
    pthread_exit(args);
    // free(line);

    
}

int get_reduce(int n_lines){
    int n_reduce;
    if (n_lines%2==0){
            n_reduce=(int)(n_lines/2);
            // n_nodes=(2*n_lines)-1;
            // Es par
        }
        else{
            n_reduce=(int)((n_lines-1)/2)+1;
            // n_nodes=(2*n_lines);
        }
    return n_reduce;
}

int number_nodes(int n_lines){
    int n_reduce=get_reduce(n_lines);
    int nodes=n_lines;
    while(n_reduce!=1){
        n_lines=n_reduce;
        n_reduce=get_reduce(n_lines);
        nodes+=n_lines;
    }
    // agregamos la raiz
    return nodes+1;
}
// Función principal.
int main(int argc, char *argv[]) {   
    // /mapreduce input.txt output.csv threads 3
    char *locale;
    locale = setlocale(LC_ALL, "");

    if(argc<4){

        printf("No hay suficientes argumentos!\n");
		return 0;
    }
    // inicializamos el puntero al arreglo de procesos

    /* Abrimos el archivo input en modo de lectura */
	FILE *input_file = fopen(argv[1],"r");

	/* Abrimos el archivo output en modo de escritura */
	FILE *output_file = fopen(argv[2], "w");

    int option=atoi(argv[4]);

    if (!input_file)
	{
		printf("¡El archivo %s no existe!\n", argv[1]);
		return 2;
	}

    MapArgs **MapMatrix;
    ReduceArgs **ReduceMatrix;
    int n_lines=0;
    char x;
    // Primero obtenemos el numero de procesos que nos van a entregar

    char last=' ';
    int chars_on_current_line=0;
    for (x = getc(input_file); x != EOF; x = getc(input_file)){
        if (x == '\n' && last!='\n'){// Increment count if this character is newline 
            n_lines++;
            chars_on_current_line=0;
        }
        else{
            chars_on_current_line++;
            }
        last=x;
    }
    if(chars_on_current_line>0) n_lines++;
    // printf("Curent %i\n",n_lines);
    // reseteamos el puntero al inicio del archivo..
    rewind(input_file);
    int n_nodes;
    int n_reduce;
    // printf("NUMBER OF LINES: %i\n",n_lines);
    n_nodes=number_nodes(n_lines);
    n_reduce=get_reduce(n_lines);
    // printf("Number of nodes is %i \n",n_nodes);
    height=ceil(log2(n_nodes))-1;

    int total_threads=n_nodes-n_lines;

    
    pthread_t maps[n_lines];
    pthread_t reduces[total_threads];

    
    char *line=NULL;
    char *temp=NULL;

    int m;
        // Creamos un arreglo de arreglos (Matriz) para guardar los map
    // WordArray **MapMatrix=calloc(sizeof(WordArray),n_lines);
    MapMatrix=calloc(sizeof(MapArgs),n_lines);
    // init pointers
    for(m=0;m<n_lines;m++){
        // printf("--Se crea el map %i \n",m);
        temp=get_next_line(input_file);
        if (!temp){
            printf("EROR AL ASIGNAR MEMORIA");

        }
        line=temp;
        
        MapMatrix[m]=calloc(sizeof(MapArgs),1);
        // printf("value is %i M:%i\n",strlen2(line),m);
        MapMatrix[m]->line=line;
        // MapMatrix[m]->line=malloc(sizeof(char)*(strlen2(line)+1));
        // strcpy(MapMatrix[m]->line,line);
        // strcpy(MapMatrix[m]->line,line);
        MapMatrix[m]->array=init_array(1);
        line=NULL;
    }
    // printf("Matrix initialized\n");
    // n_lines=2;
    for(m=0;m<n_lines;m++){

        // Para cada linea, creamos un map
        // printf("Se crea el map %i \n",m);
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

        // printf("--Resultado  del map %i \n",m);
        // print_array(MapMatrix[m]->array->elements,MapMatrix[m]->array->size);

    }
    // fclose(input_file);

    // print_array(MapMatrix[0]->array->elements,MapMatrix[0]->array->size);
 

    int *assigned=NULL;

    ReduceMatrix=calloc(sizeof(ReduceArgs),total_threads);
    int r;
    
    int aux_r=0;
 
    int maps_assigned=0;
    int *asigned_maps=&maps_assigned;

    bool * availability_array=calloc(sizeof(bool),total_threads);

    for(r=0;r<n_reduce;r++){
        // printf("Se crea el Reduce %i \n",r);
       
        ReduceMatrix[r]=malloc(sizeof(ReduceArgs));
        assigned=get_asigned_maps(n_lines,n_reduce,r,asigned_maps);
        // ReduceMatrix[r]->array1=malloc(sizeof(MapMatrix[assigned[0]]->array));
        ReduceMatrix[r]->array1=init_array(MapMatrix[assigned[0]]->array->size);
        ReduceMatrix[r]->array1=MapMatrix[assigned[0]]->array;
        // printf("El Reduce %i Recibe array1\n",r);
        // print_array(ReduceMatrix[r]->array1->elements,ReduceMatrix[r]->array1->size);

        if(assigned[1]!=-1){
            ReduceMatrix[r]->array2=init_array(MapMatrix[assigned[1]]->array->size);
            ReduceMatrix[r]->array2=MapMatrix[assigned[1]]->array;
            pthread_create(&reduces[r],NULL,(void *)reduce,(void*)ReduceMatrix[r]);
            //  printf("El Reduce %i Recibe array2\n",r);
            // print_array(ReduceMatrix[r]->array2->elements,ReduceMatrix[r]->array2->size);
        }
        else{
            ReduceMatrix[r]->array2=NULL;
            // No es necesario hacer un thread
            ReduceMatrix[r]->result=init_array(MapMatrix[assigned[0]]->array->size);
            ReduceMatrix[r]->result=MapMatrix[assigned[0]]->array;
        }

        aux_r++;

        // strcpy(ReduceMatrix[m]->line,line);
        
        // current_reduce++;
    }
        // Join threads

    // int maps_assigned=0;
    *asigned_maps=0;
    aux_r=0;
    for(r=0;r<n_reduce;r++){
        // Para cada map, hacemos un join para esperarlos
        assigned=get_asigned_maps(n_lines,n_reduce,r,asigned_maps);
            availability_array[r]=true;

        if(assigned[1]!=-1){
            if(pthread_join(reduces[r], NULL) ){
                fprintf(stderr, "Error joining thread\n");
                return 2;
            }
            // printf("--START Resultado  del reduce %i \n",r);
            // print_array(ReduceMatrix[r]->result->elements,ReduceMatrix[r]->result->size);
            // printf("--END Resultado  del reduce %i \n",r);
        }
    }
    // delete_matrix_map(MapMatrix,n_lines);
    n_lines=n_reduce;
    n_reduce=get_reduce(n_lines);
    // base+=n_reduce;
    height-=1;
        
    int value=n_lines;
    aux_r=0;

    while(availability_array[total_threads-1]!=true){
        // maps=realloc(maps,sizeof(pthread_t)*n_lines);
        // reduces=realloc(reduces,sizeof(pthread_t)*n_reduce);
        for(r=value;r<n_reduce+value;r++){
            // printf("Se crea el Reduce %i \n",r);
            ReduceMatrix[r]=malloc(sizeof(ReduceArgs));
            assigned=get_asigned_reduce(availability_array,total_threads);
            // availability_array[assigned[0]]=true;
            // availability_array[assigned[1]]=true;
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
        
        for(m=value;m<n_reduce+value;m++){
            // Para cada map, hacemos un join para esperarlos
            // printf("value is %i \n",m);
            // assigned=get_asigned_reduce(availability_array,total_threads);
           
            availability_array[m]=true;

            if(ReduceMatrix[m]->array2!=NULL){
                if(pthread_join(reduces[m], NULL) ){
                    fprintf(stderr, "Error joining thread\n");
                    return 2;
                }

                //  printf("--START Resultado  del reduce %i \n",m);
                //  print_array(ReduceMatrix[m]->result->elements,ReduceMatrix[m]->result->size);
                //  printf("--END Resultado  del reduce %i \n",m);
            }
        }
        // remaining_threads-=n_reduce;
        value+=n_reduce;

        n_lines=n_reduce;
        n_reduce=get_reduce(n_lines);

    }

    WordArray *result1=malloc(sizeof(ReduceMatrix[total_threads-1]->result));
    result1->elements=calloc(sizeof(WordHash),ReduceMatrix[total_threads-1]->result->size);
    result1=ReduceMatrix[total_threads-1]->result;
    result1->elements=ReduceMatrix[total_threads-1]->result->elements;

    if(option==1){
        write_array(result1->elements,result1->size,output_file);
        pthread_exit(NULL); 
        
    }

    int max_freq=result1->elements[result1->size-1]->frequency;
    // ------------------------FUNCIONALITY 2

    // int n_nodes;
    // int n_reduce;
    ReduceArgs ** ReduceMatrix2;
    MapArgs ** MapMatrix2;
    // Tiramos un map por cada frequencia
    n_lines=max_freq-1;

    // printf("NUMBER OF LINES: %i\n",n_lines);
    n_nodes=number_nodes(n_lines);
    n_reduce=get_reduce(n_lines);
    // printf("Number of nodes is %i \n",n_nodes);

    total_threads=n_nodes-n_lines;

    
    pthread_t maps2[n_lines];
    pthread_t reduces2[total_threads];

    

        // Creamos un arreglo de arreglos (Matriz) para guardar los map
    // WordArray **MapMatrix2=calloc(sizeof(WordArray),n_lines);

    // n_lines=2;
    MapMatrix2=calloc(sizeof(MapArgs),n_lines);
    // init pointers
    for(m=1;m<n_lines;m++){
        MapMatrix2[m]=calloc(sizeof(MapArgs),1);
        MapMatrix2[m]->line=malloc(sizeof(m));
        MapMatrix2[m]->line=m;
        MapMatrix2[m]->array=init_array(result1->size);
        // Copiamos el array result
        for(int x=0;x<result1->size;x++){
            MapMatrix2[m]->array->elements[x]=result1->elements[x];
        }
        // MapMatrix2[m]->array=result1;
        line=NULL;
    }
    for(m=1;m<n_lines;m++){
        // printf("map created %i \n",m);
        if(pthread_create(&maps2[m], NULL, (void *)freqcounter, (void*)MapMatrix2[m])) {
            fprintf(stderr, "Error creating thread\n");
        return 1;
        }
    }

    for(m=1;m<n_lines;m++){
        // Para cada map, hacemos un join para esperarlos
        if(pthread_join(maps2[m], NULL) ){
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }

        if (MapMatrix2[m]->array->size>0){
            // printf("--Resultado  del map %i \n",m);
            write_array2(MapMatrix2[m]->array->elements,MapMatrix2[m]->array->size,output_file);
        }

    }

    // fclose(input_file);

    // print_array(MapMatrix2[0]->array->elements,MapMatrix2[0]->array->size);
 
    // free(assigned);
    // assigned=NULL;

    // ReduceMatrix2=calloc(sizeof(ReduceArgs),total_threads);
    
    // aux_r=0;
 
    // maps_assigned=0;
    // asigned_maps=&maps_assigned;

    // free(availability_array);
    // availability_array=calloc(sizeof(bool),total_threads);

    // for(r=0;r<n_reduce;r++){
    //     // printf("Se crea el Reduce %i \n",r);
       
    //     ReduceMatrix2[r]=malloc(sizeof(ReduceArgs));
    //     assigned=get_asigned_maps(n_lines,n_reduce,r,asigned_maps);
    //     // ReduceMatrix2[r]->array1=malloc(sizeof(MapMatrix2[assigned[0]]->array));
    //     ReduceMatrix2[r]->array1=init_array(MapMatrix2[assigned[0]]->array->size);
    //     ReduceMatrix2[r]->array1=MapMatrix2[assigned[0]]->array;
    //     // printf("El Reduce %i Recibe array1\n",r);
    //     // print_array(ReduceMatrix2[r]->array1->elements,ReduceMatrix2[r]->array1->size);

    //     if(assigned[1]!=-1){
    //         ReduceMatrix2[r]->array2=init_array(MapMatrix2[assigned[1]]->array->size);
    //         ReduceMatrix2[r]->array2=MapMatrix2[assigned[1]]->array;
    //         pthread_create(&reduces2[r],NULL,(void *)reduce,(void*)ReduceMatrix2[r]);
    //         //  printf("El Reduce %i Recibe array2\n",r);
    //         // print_array(ReduceMatrix2[r]->array2->elements,ReduceMatrix2[r]->array2->size);
    //     }
    //     else{
    //         ReduceMatrix2[r]->array2=NULL;
    //         // No es necesario hacer un thread
    //         ReduceMatrix2[r]->result=init_array(MapMatrix2[assigned[0]]->array->size);
    //         ReduceMatrix2[r]->result=MapMatrix2[assigned[0]]->array;
    //     }

    //     aux_r++;

    //     // strcpy(ReduceMatrix2[m]->line,line);
        
    //     // current_reduce++;
    // }
    //     // Join threads

    // // int maps_assigned=0;
    // *asigned_maps=0;
    // aux_r=0;
    // for(r=0;r<n_reduce;r++){
    //     // Para cada map, hacemos un join para esperarlos
    //     assigned=get_asigned_maps(n_lines,n_reduce,r,asigned_maps);
    //         availability_array[r]=true;

    //     if(assigned[1]!=-1){
    //         if(pthread_join(reduces2[r], NULL) ){
    //             fprintf(stderr, "Error joining thread\n");
    //             return 2;
    //         }
    //         // printf("--START Resultado  del reduce %i \n",r);
    //         // print_array(ReduceMatrix2[r]->result->elements,ReduceMatrix2[r]->result->size);
    //         // printf("--END Resultado  del reduce %i \n",r);
    //     }
    // }
    // // delete_matrix_map(MapMatrix,n_lines);

    // n_lines=n_reduce;
    // n_reduce=get_reduce(n_lines);
    // // base+=n_reduce;
    // height-=1;
        
    // value=n_lines;
    // aux_r=0;

    // while(availability_array[total_threads-1]!=true){
    //     // maps=realloc(maps,sizeof(pthread_t)*n_lines);
    //     // reduces2=realloc(reduces2,sizeof(pthread_t)*n_reduce);
    //     for(r=value;r<n_reduce+value;r++){
    //         // printf("Se crea el Reduce %i \n",r);
    //         ReduceMatrix2[r]=malloc(sizeof(ReduceArgs));
    //         assigned=get_asigned_reduce(availability_array,total_threads);
    //         // availability_array[assigned[0]]=true;
    //         // availability_array[assigned[1]]=true;
    //         ReduceMatrix2[r]->array1=malloc(sizeof(ReduceMatrix2[assigned[0]]->result));
    //         ReduceMatrix2[r]->array1=ReduceMatrix2[assigned[0]]->result;

    //         if(assigned[1]!=-1){
    //             ReduceMatrix2[r]->array2=malloc(sizeof(ReduceMatrix2[assigned[1]]->result));
    //             ReduceMatrix2[r]->array2=ReduceMatrix2[assigned[1]]->result;
    //             ReduceMatrix2[r]->result=malloc(sizeof(WordArray));

    //             pthread_create(&reduces2[r],NULL,(void *)reduce,(void*)ReduceMatrix2[r]);
                
    //         }
    //         else{
    //             ReduceMatrix2[r]->array2=NULL;

    //             // No es necesario hacer un thread
    //             ReduceMatrix2[r]->result=malloc(sizeof(ReduceMatrix2[assigned[0]]->result));
    //             ReduceMatrix2[r]->result=ReduceMatrix2[assigned[0]]->result;
    //         }

    //         // strcpy(ReduceMatrix2[m]->line,line);
            
    //         // current_reduce++;
    //     }
    //     // Join threads
        
    //     for(m=value;m<n_reduce+value;m++){
    //         // Para cada map, hacemos un join para esperarlos
    //         // printf("value is %i \n",m);
    //         // assigned=get_asigned_reduce(availability_array,total_threads);
           
    //         availability_array[m]=true;

    //         if(ReduceMatrix2[m]->array2!=NULL){
    //             if(pthread_join(reduces2[m], NULL) ){
    //                 fprintf(stderr, "Error joining thread\n");
    //                 return 2;
    //             }

    //             //  printf("--START Resultado  del reduce %i \n",m);
    //             //  print_array(ReduceMatrix2[m]->result->elements,ReduceMatrix[m]->result->size);
    //             //  printf("--END Resultado  del reduce %i \n",m);
    //         }
    //     }
    //     // remaining_threads-=n_reduce;
    //     value+=n_reduce;

    //     n_lines=n_reduce;
    //     n_reduce=get_reduce(n_lines);

    // }
   pthread_exit(NULL); 

}