#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h> 


typedef struct{
    int frequency;
    // No hay palabras con mas de 100 characteres.
    // 49 -> se asignan 52 bytes para el array de chars
    // + 4 bytes de el int frequency.
    char word[49];
    
}WordHash;
