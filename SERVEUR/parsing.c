#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "header.h"
int traite_message(char *message, struct pollfd poll, Parametre parametres, char ***matrice, User *user, int ecrit){

	char messageEnvoi[LG_MESSAGE];
	memset(messageEnvoi, '\0', LG_MESSAGE);
	char *parametre[] = {"", ""};

	char *split_string = strtok(message, " "); 
		
	if( strcmp(split_string, "/setPixel" ) == 0){
	
		/*RECUPERATION DES PARAMETRES*/
		char *position = strtok(NULL, " ");
		char *color = strtok(NULL, " ");
		color = strtok(color, "\n");
		if(position == NULL || color == NULL){
			sprintf(messageEnvoi, "10 : Bad Command\n");
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
		}
		else{
			parametre[0] = strtok(position, "x");
			parametre[1] = strtok(NULL, "x");
			
			//mise à jour de la matrice
			if( (atoi(parametre[0]) >= parametres.dimensions[0]) || (atoi(parametre[0]) < 0) ){
				sprintf(messageEnvoi, "11 : Out of Bound\n");
				ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
			}
			else if( (atoi(parametre[1]) >= parametres.dimensions[1]) || (atoi(parametre[1]) < 0) ){
				sprintf(messageEnvoi, "11 : Out of Bound\n");
				ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
			}
			else{
				//regarder si la couleur est ok 
				//traiter la commande	
				int check = check_limite(user, parametres, matrice, parametre, color, ecrit);
				
				for(int i=0; i < parametres.dimensions[0]; i++){
					for(int j=0; j < parametres.dimensions[1]; j++){
						printf("%s", matrice[i][j]);
					}
				}
				printf("\n\n");
				
				if(check == 0){
					sprintf(messageEnvoi, "00 : pixel placé\n");		
					ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
				}
				else{
					sprintf(messageEnvoi, "20 : Out of Quota\n");		
					ecrit = write(user->socketClient, messageEnvoi, strlen(messageEnvoi));
				}
			}
		}
		/*---------------------------*/
	}
	
	//utilisation de strncmp et non strcmp pour eviter les probleme avec les acsii allant de 0 à 32
	else if( strncmp(split_string, "/getMatrix", 10) == 0){
		
		char *matrice_b64 = (char*) malloc(parametres.dimensions[0]*parametres.dimensions[1]*4*sizeof(char));
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
		
		sprintf(dim2, "%d",parametres.dimensions[1]-1);
		char precision[10];	
		sprintf(precision, "%s", " de 0 à ");
		strcat(precision, dim2);
		
		strcat(dim1, precision);
		strcat(dim1, "\n");
		sprintf(messageEnvoi, "%s",dim1);
		/*----------------------------------------------------------------------------*/
		
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else if(strncmp(split_string, "/getVersion", 11) == 0){
		sprintf(messageEnvoi, "version : 1\n");
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else if(strncmp(split_string, "/getLimits", 10) == 0){
		sprintf(messageEnvoi, "%d", parametres.limite);
		strcat(messageEnvoi, " pixel/min\n");
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	else if(strncmp(split_string, "/getWaitTime", 12) == 0){
		if(user->chrono != 0){
			int temp_restant = (int) difftime( time(NULL), user->chrono);
			temp_restant = 60 - temp_restant;
			if(temp_restant > 0) sprintf(messageEnvoi, "%d",temp_restant);
			else sprintf(messageEnvoi, "%d", 0);
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
		} 
		else{
			sprintf(messageEnvoi, "%s", "0 secondes\n");		
			ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
		}
		
	}
	else{
		sprintf(messageEnvoi, "99 : Unknown Command\n");		
		ecrit = write(poll.fd, messageEnvoi, strlen(messageEnvoi));
	}
	return ecrit;
}


int check_limite(User *user, Parametre parametres, char ***matrice, char **position, char *color, int ecrit){
	if( (user->chrono ==0) || (difftime( time(NULL), user->chrono) > 60)){
		//set pixel
		matrice[ atoi(position[0])][atoi(position[1])] = (char*) malloc(LG_MESSAGE);
		strcpy(matrice[ atoi(position[0])][atoi(position[1])] ,  color);
		//enclenche timer 
		time(&user->chrono);
		//incrémente le compteur
		user->cpt_pixel = 1;
		return 0;
	}
	else if(difftime( time(NULL), user->chrono) < 60){
		if(user->cpt_pixel < parametres.limite){
			//set pixel
			matrice[ atoi(position[0])][atoi(position[1])] = (char*) malloc(LG_MESSAGE);
			strcpy(matrice[ atoi(position[0])][atoi(position[1])] ,  color);
			//enclenche timer 
			time(&user->chrono);
			//incrémente le compteur
			user->cpt_pixel += 1;
			return 0;
		}
		else{
			return 1;
		}
	}
	return ecrit;
}
