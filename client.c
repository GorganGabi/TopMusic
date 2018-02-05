#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

extern int errno;
int port;

struct song
{
	char artist[30];
	char title[30];
	char type[10];
	char description[100];
	int count;
	char link[512];
}Song;

int main (int argc, char *argv[])
{
  int sd, ok=0, breakWhile=0;
  int type=0, back=0;
  struct sockaddr_in server;
  int choice, choice1, length, poz, lengthString, i;
  char username[20], msg[1000], type_music[20], s_title[100], comment[100], pwd[100];

  if (argc != 3)
    {
      printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
      return -1;
    }
 
  port = atoi (argv[2]);

  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("Socket() error.\n");
      return errno;
    }
  int enable = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    {
	perror("setsockopt(SO_REUSEADDR) failed");
	return errno;
    }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(argv[1]);
  server.sin_port = htons (port);
  
  if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
	{
		perror ("[client]Eroare la connect().\n");										       
		return errno;
	}  

	
  while(1)
  {
	if (ok) break;
	if (breakWhile) break;
	printf("***********************MENIU*********************\n");
 	printf("1. Sign in.\n");
 	printf("2. Log in.\n");
 	printf("3. Exit. \n");
 	printf("*************************************************\n");
 	printf("Enter your choice: ");
 	scanf(" %d", &choice);
	
  	switch (choice)
  	{
		case 1:
			printf("Enter the username: \n");
			scanf(" %s", username);
			printf("Enter the password: ");
			scanf(" %s", pwd);
			printf("--------------------\n");
			printf("Choose the type of user: \n");
			printf("1.Normal\n");
			printf("2.Admin\n");
			printf("--------------------\n");
			printf("Enter the type: ");
			scanf(" %d", &type);
			length=7;
			send(sd, &length, 4, 0);
			send(sd, "signIn", 7, 0);
			send(sd, &type, 4, 0);
			length=strlen(username)+1;
			send(sd, &length, 4, 0);
		   	send(sd, username, length, 0);
			length=strlen(pwd)+1;
			send(sd, &length, 4, 0);
			send(sd, pwd, length, 0);
			recv(sd, &length, 4 ,0);
			recv(sd, msg, length, 0);
			if (strcmp(msg, "Succes!")==0) printf("%s\n\n",msg);
			else printf("%s\n\n",msg);
			break;
		case 2:
			printf("Enter the username: ");
			scanf(" %s", username);
			printf("Enter the password: ");
			scanf(" %s", pwd);
			length=6;
			send(sd, &length, 4, 0);
			send(sd, "logIn", 6, 0);
			//printf("Trimitem la server %d si 'logIn' \n", length);
			length=strlen(username)+1;
			send(sd, &length, 4, 0);
			send(sd, username, length, 0);
			length=strlen(pwd)+1;
			send(sd, &length, 4, 0);
			send(sd, pwd, length, 0);
			//printf("Trimitem la server %s, %s\n", username, pwd);
		   	recv(sd, &length, 4 ,0);
			recv(sd, msg, length, 0);
			recv(sd, &type, 4, 0);
			if (strcmp(msg, "Succes!")==0)
			{ 
				printf("%s\n\n", msg);
				if (type==0)
				{ 
					back=0;
					while(1)
					{
						if (back) break;
						printf("---------------------\n");
						printf("-----Musical TOP----- \n");
						printf("1. Add a song\n");
						printf("2. Vote a song\n");
						printf("3. Show the general top\n");	
						printf("4. Show a specific top\n");
						printf("5. Add a comment\n");
						printf("6. Back\n");
						printf("---------------------\n");
						printf("Enter your choice: ");
						scanf(" %d", &choice);
						printf("You have chosen %d \n", choice);
						switch(choice)
						{	
							case 1: 
								length=4;
								getchar();
								printf(" Enter the name of the artist: ");
								fgets(Song.artist, sizeof(Song.artist), stdin);
								Song.artist[strlen(Song.artist)-1]='\0';
								printf(" Enter the title of the song: ");	
								fgets(Song.title, sizeof(Song.title), stdin);	
								Song.title[strlen(Song.title)-1]='\0';
								printf(" Enter the type of the song: ");	
								fgets(Song.type, sizeof(Song.type), stdin);
								Song.type[strlen(Song.type)-1]='\0';
								printf(" Enter the description of the song: ");	
								fgets(Song.description, sizeof(Song.description), stdin);
								Song.description[strlen(Song.description)-1]='\0';
								printf(" Enter the youtube's link to the song: ");	
								fgets(Song.link, sizeof(Song.link), stdin);
								Song.link[strlen(Song.link)-1]='\0';	
								send(sd, &length, 4, 0);
								send(sd, "add", 4, 0);
								send(sd, &Song, sizeof(Song), 0);
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("%s\n",msg);							
								break;
							case 2:		
								length=5;
								printf("1. Enter the name of the song\n");
								printf("2. Back \n");
								printf("Enter the choice: ");
								scanf(" %d", &choice1);
								if (choice1==1)
								{
									printf("Enter the title of the song: ");
									getchar();							
									fgets(s_title, sizeof(s_title), stdin);
									send(sd, &length, 4, 0); 
									send(sd, "vote", 5, 0);
									length=strlen(s_title)+1;
									s_title[strlen(s_title)-1]='\0';
									//printf("Am trimis %s si lungimea %d\n", s_title, length);
									send(sd, &length, 4, 0);
									send(sd, s_title, length, 0);	
									recv(sd, &length, 4, 0);
									recv(sd, msg, length, 0);
									printf("%s\n", msg); 						
									break;
								}
								else break;
							case 3: 
								length=5;
								send(sd, &length, 4, 0); 
								send(sd, "show", 5, 0);
								recv(sd, &length, 4, 0); //length is the number of rows
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("%s\n",msg);
								break;
							case 4: 
								length=9;
								printf("Enter the music type: ");
								scanf(" %s", type_music);
								send(sd, &length, 4, 0); 
								send(sd, "showType", 9, 0);
								int type_length=strlen(type_music)+1;
								send(sd, &type_length, 4, 0);
								send(sd, type_music, type_length, 0);
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("%s\n",msg);
								break;
							case 5:
								length=8;
								printf("1. Enter the name of the song\n");
								printf("2. Back \n");
								printf("Enter the choice: ");
								scanf(" %d", &choice1);
								if ( choice1==1)
								{
									send(sd, &length, 4, 0); 
									send(sd, "comment", length, 0);
									printf("Enter the title of the song: ");
									getchar();							
									fgets(s_title, sizeof(s_title), stdin);
									s_title[strlen(s_title)-1]='\0';
									length=strlen(s_title)+1;
									send(sd, &length, 4, 0);
									send(sd, s_title, length, 0);
									recv(sd, &length, 4, 0);
									if (length==-1)
									{
										recv(sd, &length, 4, 0);
										recv(sd, msg, length, 0);
										printf("%s\n",msg);  
										break; 
									} 
									else
									{
										printf("Enter the comment (100 caracters max): ");
										fgets(comment, sizeof(comment), stdin);
										comment[strlen(comment)-1]='\0';
										//printf("Am citit pentru melodia %s comentariul '%s' \n",s_title, comment);
										length=strlen(comment)+1;
										send(sd, &length, 4, 0);
										send(sd, comment, length, 0);
										recv(sd, &length, 4, 0);
										recv(sd, msg, length, 0);
										printf("%s\n",msg);		
										break;
									}
								}
								else break;
							case 6: 
								back=1;
								break;
							default: 
								printf("Wrong option! Enter again...\n");
						}
					}
				}
				else if (type==1)
				{
					back=0;
					while(1)
					{
						if(back) break;
						printf("---------------------\n");
						printf("-----Musical TOP----- \n");
						printf("1. Block a user\n");
						printf("2. Delete a song\n");
						printf("3. Unblock a user\n");
						printf("4. Show the users\n");
						printf("5. Back\n");
						printf("---------------------\n");
						printf("Enter your choice: ");
						scanf(" %d", &choice);	
						switch(choice)
						{
							case 1:
								length=6;
								send(sd, &length, 4, 0); 
								send(sd, "block", length, 0);
								printf("Enter the username of the user you want to block: ");
								scanf(" %s", msg);
								msg[strlen(msg)]='\0';
								length=strlen(msg)+1;
								send(sd, &length ,4 ,0);
								send(sd, msg, length, 0);
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("%s\n",msg);
								break;
							case 2: 
								length=7;
								send(sd, &length, 4, 0); 
								send(sd, "delete", length, 0);
								printf("Enter the title of the song you want to delete: ");
								getchar();							
								fgets(s_title, sizeof(s_title), stdin);
								s_title[strlen(s_title)-1]='\0';
								length=strlen(s_title)+1;
								//printf("melodia %s are lungimea %d \n", s_title, length);
								send(sd, &length, 4, 0);
								send(sd, s_title, length, 0);
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("\n%s\n",msg);
								break;
							case 3:
								length=8;
								send(sd, &length, 4, 0); 
								send(sd, "unblock", length, 0);
								printf("Enter the username of a blocked user: ");
								scanf(" %s", msg);
								msg[strlen(msg)]='\0';
								length=strlen(msg)+1;
								send(sd, &length ,4 ,0);
								send(sd, msg, length, 0);
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("%s\n",msg);
								break;
							case 4:
								length=10;
								send(sd, &length, 4, 0); 
								send(sd, "showUsers", length, 0);
								recv(sd, &length, 4, 0);
								recv(sd, msg, length, 0);
								printf("%s\n",msg);
								break;
							case 5:
								back=1;
								break;
							default: 
								printf("Wrong option! Enter again...\n");
						}
					}
				}
			}
			else
			{
				printf("%s\n\n",msg);
			}
	   		break;
		case 3: 
				length=5;
				send(sd, &length, 4, 0);
				send(sd, "exit", 6, 0);
				breakWhile=1;
  	}
  }
  
  close (sd);
  return 0;
}
