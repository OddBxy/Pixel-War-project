#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h> /* pour close et sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#include <poll.h>
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 256

#define parametre_PORT "-p"
#define parametre_SIZE "-s"
#define parametre_LIMIT "-l"

#define commande_matrix "/getMatrix"
#define commande_size "/getSize"
#define commande_limits "/getLimits"
#define commande_version "/getVersion"
#define commande_WaitTime "/getWaitTime"

typedef struct Parametre{
	int PORT;
	int limite;
	int *dimensions;
}Parametre;



typedef struct User User;
struct User{
	int socketClient;
	struct sockaddr_in *sockin;
	User *suivant;
};

Parametre parametrage(int, char *args[]);
void affiche_liste(User *liste);
User *ajout_Client(User *liste, int socketEcoute, socklen_t *address_len);
User* supprime_Client(User *liste, int socket_Client);
struct pollfd *supprime_poll(struct pollfd *liste, int index, int nbPoll);
void traite_message(char *message, struct pollfd, Parametre parametres, char ***matrice);
void reception_message(struct pollfd *poll_message, int *nbClient, User **liste, Parametre parametres, char ***matrice);



