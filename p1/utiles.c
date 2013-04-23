#include "utiles.h"

int mi_atoi(const char* p)
{
    int numero_leido;
    int i = 0; 
    int numero = 0;
    
    while (1) {
      numero_leido = p[i];
      
      // FIXME : no funciona la comprobacion
      /*
      if ( (numero_leido < 48) || (numero_leido > 57) ) {
        return -1;
      }
      */
        
      if (numero_leido == 10) {
        return numero;
      } 
      numero_leido = numero_leido - 48;
      if (i == 0) {
        numero = numero_leido;
      } else {
        numero = numero * 10 + numero_leido;
      }
      i++;
    }
}

