#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "header.h"
void traite_message(char *message, struct pollfd poll, char ***matrice, Parametre parametres){
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
			
			//printf("%s, %s, %s\n", parametre[0],parametre[1], color);
		}
		/*---------------------------*/
		
		if(atoi(parametre[0]) > parametres.dimensions[0] || atoi(parametre[0]) < 0){
			sprintf(messageEnvoi, "Out of Bound\n");
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
		}
		else if(atoi(parametre[1]) > parametres.dimensions[1] || atoi(parametre[1]) < 0){
			sprintf(messageEnvoi, "Out of Bound\n");
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
		}
		else{
			//regarder si la couleur est ok 
			sprintf(messageEnvoi, "ok\n");
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
			//traiter la commande	
		}
		//printf("%s %s", parametre[0], parametre[1]);
	}
	
	//utilisation de strncmp et non strcmp pour eviter les probleme avec les acsii allant de 0 à 32
	else if( strncmp(split_string, "/getMatrix", 10) == 0){
		sprintf(messageEnvoi, "ok\n");
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
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
		sprintf(messageEnvoi, "ok\n");
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
