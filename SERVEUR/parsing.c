#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "header.h"
void traite_message(char *message, struct pollfd poll, Parametre parametres, char ***matrice){
	int ecrit;
	char messageEnvoi[LG_MESSAGE];
	
	char *parametre[] = {"", ""};

	char *split_string = strtok(message, " "); 
		
	if( strcmp(split_string, "/setPixel" ) == 0){
	
		/*RECUPERATION DES PARAMETRES*/
		char *position = strtok(NULL, " ");
		char *color = strtok(NULL, " ");
		if(position == NULL || color == NULL){
			sprintf(messageEnvoi, "Pas assez de parametres\n");
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
		}
		else{
			parametre[0] = strtok(position, "x");
			parametre[1] = strtok(NULL, "x");
			
			//mise à jour de la matrice
			if( (atoi(parametre[0]) >= parametres.dimensions[0]) || (atoi(parametre[0]) < 0) ){
				sprintf(messageEnvoi, "Out of Bound\n");
				ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
			}
			else if( (atoi(parametre[1]) >= parametres.dimensions[1]) || (atoi(parametre[1]) < 0) ){
				sprintf(messageEnvoi, "Out of Bound\n");
				ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
			}
			else{
				//regarder si la couleur est ok 
				sprintf(messageEnvoi, "ok\n");
				ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
				//traiter la commande	
				printf("%s\n", color);
				matrice[ atoi(parametre[0])][atoi(parametre[1])] = (char*) malloc(LG_MESSAGE);
				strcpy(matrice[ atoi(parametre[0])][atoi(parametre[1])] ,  color);
				
				for(int i=0; i < parametres.dimensions[0]; i++){
					for(int j=0; j < parametres.dimensions[1]; j++){
						printf("%s", matrice[i][j]);
					}
				}
			}
		}
		/*---------------------------*/
	}
	
	//utilisation de strncmp et non strcmp pour eviter les probleme avec les acsii allant de 0 à 32
	else if( strncmp(split_string, "/getMatrix", 10) == 0){
		
		char matrice_b64[parametres.dimensions[0]*parametres.dimensions[1]*LG_MESSAGE];
		for(int i =0; i < parametres.dimensions[0]; i++){
			for(int j =0; j < parametres.dimensions[1]; j++){		
				strcat(matrice_b64, matrice[i][j]);
			}
		}
		ecrit = write(poll.fd, matrice_b64, strlen(matrice_b64));
	}
	
	else if(strncmp(split_string, "/getSize", 8) == 0){
		/*on met les dimensions en char* pour initialiser messageEnvoie avec celles-ci*/
		char dim1[10], dim2[10];
		sprintf(dim1, "%d",parametres.dimensions[0]);
		sprintf(dim2, "%d",parametres.dimensions[1]);
		strcat(dim1,"x");
		strcat(dim1, dim2);
		strcat(dim1, "\n");
		sprintf(messageEnvoi,dim1);
		/*----------------------------------------------------------------------------*/
		
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else if(strncmp(split_string, "/getVersion", 11) == 0){
		sprintf(messageEnvoi, "ok\n");
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else if(strncmp(split_string, "/getLimits", 10) == 0){
		sprintf(messageEnvoi, "%d", parametres.limite);
		strcat(messageEnvoi, " pixel/min\n");
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else if(strncmp(split_string, "/getWaitTime", 12) == 0){
		sprintf(messageEnvoi, "ok\n");		
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else{
		sprintf(messageEnvoi, "Unknown Command\n");		
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	
}
