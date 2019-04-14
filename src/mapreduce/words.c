#include "words.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> 


WordHash ** init_array(int n_words){
    // Inicalizamos un array de tamaño 10 
    WordHash **wordarray=malloc(sizeof(WordHash)*n_words);
    for(int i=0;i<n_words;i++){
        wordarray[i]=malloc(sizeof(WordHash));
    }
    
    return wordarray;
}
