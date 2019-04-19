#include "words.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> 


WordArray * init_array(int n_words){
    WordArray *new_array=malloc(sizeof(WordArray));
    // Inicalizamos un array de tamaño 10 
    WordHash **wordarray=malloc(sizeof(WordHash)*n_words);
    for(int i=0;i<n_words;i++){
        wordarray[i]=calloc(sizeof(WordHash),1);
        // *wordarray[i]->word=NULL;
    }
    new_array->elements=wordarray;
    new_array->size=n_words;
    return new_array;
}
