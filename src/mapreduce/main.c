// Importe de librerías
#include "ArrayOfWords.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAXCHAR 50

void wordcounter(WordHash **wordarray,char *line){
    int size=0;
    char delim[]=" ";
    char *context;

    int inputLength = strlen(line);
    char *inputCopy = (char*) calloc(inputLength + 1, sizeof(char));
    strncpy(inputCopy, line, inputLength);

    char *word = strtok_r (inputCopy, delim, &context);
    bool shouldInsert=false;
    int position_found=0;
    while(word!=NULL){
        shouldInsert=true;
        printf("value is %i \n",size);

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
            strcpy(wordarray[size]->word,word);
            wordarray[size]->frequency=1;
            size++;
        }
        else{
            // debemos aumentar la frecuencia.
            wordarray[position_found]->frequency+=1;
        }

        word=strtok_r(NULL,delim,&context);
    }
    printf("word array: %s \n",*wordarray);

}

char *getln()
{
    char *line = NULL, *tmp = NULL;
    size_t size = 0, index = 0;
    int ch = EOF;

    while (ch) {
        ch = getc(stdin);

        /* Check if we need to stop. */
        if (ch == EOF || ch == '\n')
            ch = 0;

        /* Check if we need to expand. */
        if (size <= index) {
            size += 32;
            tmp = realloc(line, size);
            if (!tmp) {
                free(line);
                line = NULL;
                break;
            }
            line = tmp;
        }

        /* Actually store the thing. */
        line[index++] = ch;
    }

    return line;
}
// strcmp(read[i], "if") == 0

// Función principal.
int main(int argc, char *argv[]) {   
    if(argc<3){
        printf("No hay suficientes argumentos!\n");
		return 0;
    }
    // inicializamos el puntero al arreglo de procesos

    /* Abrimos el archivo input en modo de lectura */
	FILE *input_file = fopen(argv[1], "r");

	/* Abrimos el archivo output en modo de escritura */
	FILE *output_file = fopen(argv[2], "w");




    if (!input_file)
	{
		printf("¡El archivo %s no existe!\n", argv[1]);
		return 2;
	}

    // char input[1000];
    // while (fgets(input, sizeof(input), input_file) != NULL){
    //     // Cada linea deberiamso entregarsela a un map

    //     printf("%s\n", input);
    //     break;
    // }

    int c, index = 0;
    int size=0;

    //  A lo mas una palabra tiene largo 43 char.
    // char *line=NULL;

    char *temp=NULL;
    char *line=NULL;
    int n_words=10;
    WordHash **wordarray=malloc(sizeof(WordHash)*n_words);
    printf("value of wordhash %i \n",sizeof(WordHash));
//  -4 es por el int de frequency.
    int size_chars=sizeof(WordHash)-4;
    


    while ( (c = fgetc (input_file)) != EOF ){
        if ( c == '\n'){
            // Agregar ===' ' para una palabra
            // Se termina una linea por el salto de linea.

            // fgets(line,counter,input_file);
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
        line[index++]=c;
    }
    // 
// void wordcounter(WordHash **wordarray,char *line,int size){
    printf("%s \n",line);

    wordcounter(wordarray,line);
    if (ferror (input_file) ){
        printf ("Read Error\n"), exit (EXIT_FAILURE);

    }


 
    

    return 0;

}