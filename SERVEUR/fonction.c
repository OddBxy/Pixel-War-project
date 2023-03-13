#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "header.h"
Parametre parametrage(int nbArgs, char *args[]){

	//si le parametre args est vide, le port et la matrice sont initialiser avec les valeurs suivantes :
	Parametre parametre;
	parametre.PORT = 5000;
	parametre.limite = 10;
	parametre.dimensions = (int*) calloc(2, sizeof(int));
	parametre.dimensions[0] = 80;
	parametre.dimensions[1] = 40;
	
	if(nbArgs > 5){
		printf("la ligne de commande contient trop de paramètres\n");
		exit(0);
	}
	for(int i=1; i < nbArgs; i++){
		//i commence a 1 car args[0] == ./serveur
		if( strcmp(args[i], parametre_PORT) == 0){
			i++;
			parametre.PORT = atoi(args[i]);
			//atoi permet de convertir des strings en integers
		}
		else if( strcmp(args[i], parametre_LIMIT) == 0){
			i++;
			parametre.limite = atoi(args[i]);
			//atoi permet de convertir des strings en integers
		}
		else if( strcmp(args[i], parametre_SIZE) == 0){
			char *dimensions_string[2] = {"", ""};
			i++;
			dimensions_string[0] = strtok(args[i], "x");
			dimensions_string[1] = strtok(NULL, "x");
			//strtok permet de separer une chaine de charactere selon un charactere donné
			for(int j=0; j<2; j++) parametre.dimensions[j] = atoi( dimensions_string[j] );
		}
	
	}
	
	return parametre;
}

void affiche_liste(User *liste){
	User *tmp = liste;
	if(tmp == NULL) printf("liste vide\n");
	while(tmp != NULL){
		printf(" socket : %d\n", tmp->socketClient);
		tmp = tmp->suivant;
	}
	printf("\n");
}

User *ajout_Client(User *liste, int socketEcoute, socklen_t *address_len){

	//affiche_liste(liste); //pour verifier 
	struct sockaddr_in pointDeRencontreDistant;

	User *client_suivant;
	client_suivant = malloc(sizeof(User));
	client_suivant->socketClient = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, address_len);
	client_suivant->sockin = &pointDeRencontreDistant;
	//client_suivant->suivant = NULL;
	
	
	if(liste == NULL){
		liste = client_suivant;
	}
	else if(liste != NULL){
		User *parcour = liste;
		while(parcour->suivant != NULL){
			parcour = parcour->suivant;
		}
		parcour->suivant = client_suivant;
		parcour->suivant->suivant = NULL;
	}
	//affiche_liste(liste); //pour verifier	
	return liste;
}

User* supprime_Client(User *liste, int socket_Client){
	User *tmp = liste;
	if(tmp->socketClient == socket_Client){
		tmp = tmp->suivant;
		liste = tmp;
		return liste;
	
	}
	else{
		while(tmp->suivant != NULL){
			if(tmp->suivant->socketClient == socket_Client){
				User *rest = tmp->suivant->suivant;
				free(tmp->suivant);
				tmp->suivant = rest; 
				liste = tmp;	
				return liste;		
			}
			else tmp = tmp->suivant;
		}
		
	}
	printf("error : socket non trouvé...\n\n");
	return liste;
}

struct pollfd *supprime_poll(struct pollfd *liste, int index, int nbPoll){
	for(int i = index; i <= nbPoll-1; i++){
		liste[i] = liste[i+1];
	}
	return liste;
}

void reception_message(struct pollfd *poll_message, int *nbClient, User **liste, char ***matrice, Parametre parametres){

	char messageEnvoi[LG_MESSAGE]; /* le message de la couche Application ! */
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */
	int ecrits, lus; /* nb d’octets ecrits et lus */

	memset(messageEnvoi, 0x00, LG_MESSAGE*sizeof(char));
	memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
	
	
	
	for(int i=1; i<=*nbClient; i++){
		if( poll(&poll_message[i], 1, 100) == 1 ){
			lus = read(poll_message[i].fd, messageRecu, LG_MESSAGE*sizeof(char)); // ici appel bloquant
			switch(lus)
			{
				case -1 : // une erreur ! 
					perror("read");
					close(poll_message[i].fd);
					exit(-5);
				case 0 : // la socket est fermée 
					fprintf(stderr, "La socket a été fermée par le client !\n\n");
					*liste = supprime_Client(*liste, poll_message[i].fd);
					poll_message = supprime_poll(poll_message, i, *nbClient);
					*nbClient -= 1;
					break;
					//return 0;
				default: 
					//if(strcmp(messageRecu[0], "/getMatrix") == 0) printf("test\n");
					//printf("Message reçu du client : %s (%d octets)\n\n", messageRecu, lus);
					traite_message(messageRecu, poll_message[i], matrice, parametres);
					break;
			}
			if(lus >0){
				// On envoie des données vers le client (cf. protocole)
				sprintf(messageEnvoi, "message reçu\n");
				ecrits = write(poll_message[i].fd, messageEnvoi, strlen(messageEnvoi));
				switch(ecrits)
				{
					case -1 : // une erreur ! 
						perror("write");
						close(poll_message[i].fd);
						
						exit(-6);
					case 0 : // la socket est fermée 
						fprintf(stderr, "La socket a été fermée par le client !\n\n");
						*liste = supprime_Client(*liste, poll_message[i].fd);
						poll_message = supprime_poll(poll_message, i, *nbClient);
						*nbClient -= 1;
						break;
						//return 0;
					default: // envoi de n octets 
						printf("Message envoyé au client : %s (%d octets)\n\n", messageEnvoi, ecrits);
				}			
			}
		}
	}
}
