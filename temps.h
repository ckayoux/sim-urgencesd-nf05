#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
//les headers sont imbriqués : temps.h->aleatoire.h->patient.h->queue.h->salles.h->main.c.

/**
*Le temps sera représenté par une structure comprenant année, mois, jour, heure et minutes. Des fonctions de saisie, d'affichage, de test et de calcul adaptées à ce type doivent être conçues.
*/
typedef struct {
    unsigned short int annee;
    unsigned char mois;
    unsigned char jour;
    unsigned char h;
    unsigned char m;
} temps;
//fonctions d'interaction utilisateur
temps demander_duree();
temps demander_heure();
temps demander_date_heure();
temps demander_date();
temps demander_annee_mois();
temps demander_jour_heure(unsigned short int annee,unsigned char mois);

//procédures d'affichage
void afficher_heure(temps t);
void afficher_date(temps t);
void afficher_date_heure(temps t);
char* h_to_string(temps t);

//fonctions intermédiaires
char digit_en_int(char *chaine,unsigned char indice);
bool estBissextile(unsigned short int annee);
bool jmSontCorrects(unsigned char jour, unsigned char mois, unsigned short int annee);

bool estDernierJourMois(temps t);
bool estDernierMois(temps t);
bool estDerniereHeure(temps t);
bool estDerniereMinute(temps t);
void incrementerTemps(temps *t);
temps decrementerTemps(temps t);
bool estNul(temps t);
void ajouterTemps(temps *t1,temps t2);
char comparerTemps(temps temps1,temps temps2);
temps diffTemps(temps t1,temps t2);
unsigned int diffTemps_minutes(temps t1,temps t2);

double convertir_temps_minutes(temps t);
void dureeMoyenne(temps t,unsigned short int diviseur);
double ecart_type(temps *tab_temps,unsigned short int nb);
void trierDurees(temps *tab_temps,int nb);
