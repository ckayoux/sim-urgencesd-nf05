#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "temps.h"

#define PI acos(-1.0) //pi = arccos(-1)
temps genererDureeTache(unsigned int esperance, unsigned int ecart);
/* on va determiner aleatoirement la duree d'une tache a l'aide de la loi normale.*/
double densite_LN(unsigned int esperance, unsigned int ecart,double x);
double integrale_LN(unsigned int esperance,unsigned int ecart,double erreur,double bornesup);

double factorielle(unsigned long int n);
