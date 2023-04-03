#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h> /* pour close et sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#include <poll.h>
#define PORT IPPORT_USERRESERVED // = 5000
#define LG_MESSAGE 10000

#define parametre_PORT "-p"
#define parametre_IP "-i"

typedef struct Parametre{
	int PORT;
	char* IP;
}Parametre;

char* binaire_8bits(int);
int binaire_toRGB(char*);

int* ascii_to_rgb(char*);
void rgb_to_string(int*, char*);
