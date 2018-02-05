#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sqlite3.h>

#define PORT 2024

extern int errno;
int poz=1, type, admin=-1;
char type_char[20], sql[100], type_music[10]="\0", song[1000], pwd[100];
int last_id=1, last_id1, client, id, status, user_id, password;

struct song
{
	char artist[30];
	char title[30];
	char type[10];
	char description[100];
	int count;
	char link[512];
}Song;

void init_type()
{
	if(type==2) strcpy(type_char,"admin");
	else strcpy(type_char,"normal");
	//printf("TYPE_CHAR ESTE %s \n", type_char);
}

int callback_name(void *NotUsed, int argc, char **argv, char **colName)
{
	for (int i = 0; i < argc; i++)
			user_id=atoi(argv[i]);
	//printf("USER_ID IN CALLBACK_NAME ESTE %d \n",user_id);
}

int callback_ID(void *NotUsed, int argc, char **argv, char **colName)
 {	
		for (int i = 0; i < argc; i++)
			id=atoi(argv[i]);
	//printf("ID IN CALLBACK_ID ESTE %d\n", id);
	return 0;
 }

int callback_status(void *NotUsed, int argc, char **argv, char **colName)
 {	
		for (int i = 0; i < argc; i++)
			status=atoi(argv[i]);
	//printf("STATUSUL IN CALLBACK_status ESTE %d\n", status);
	return 0;
}

int callback_lastID(void *NotUsed, int argc, char **argv, char **colName)
 {	
	//printf("AM INTRAT IN CALLBACK SIMPLU SI TYPE_MUSIC ESTE %s \n", type_music);
	if (type_music[0]=='\0') last_id++;
	else	
	{
		for (int i = 0; i < argc; i++)
			{
				if (strcmp(colName[i], "Type")==0)
					if(strcmp(argv[i],type_music)==0) last_id++;
				if (strcmp(colName[i], "Id")==0)
					last_id1=atoi(argv[i]);
			}	
	}
	//printf("last_id1 este %d si last_id %d\n", last_id1, last_id);
	return 0;
}

int callback_show(void *data, int argc, char **argv, char **colName)
{ 	
    if(poz==1)
    {
	  char Poz[4];
	  sprintf(Poz,"%d.", poz);
	  strcpy(song,Poz);
	  poz++;
    }
    else
    {
	 char Poz[4];
	 sprintf(Poz,"%d.", poz);
	 strcat(song,Poz);
	 poz++;
    }

    for (int i = 0; i < argc; i++)
	{
		if (strcmp(colName[i],"Artist") == 0) 
			strcat(song, argv[i]);
		if (strcmp(colName[i],"Title") == 0)
			{
				strcat(song, " - ");
				strcat(song, argv[i]);
				strcat(song,"\n");
			}	
	}
    return 0;
}

int callback_showUsers(void *data, int argc, char **argv, char **colName)
{ 	

   if(poz==1)
    {
	  char Poz[4];
	  sprintf(Poz,"%d.", poz);
	  strcpy(song,Poz);
	  poz++;
    }
    else
    {
	 char Poz[4];
	 sprintf(Poz,"%d.", poz);
	 strcat(song,Poz);
	 poz++;
    }

    for (int i = 0; i < argc; i++)
	{
		if (strcmp(colName[i],"Name") == 0) 
			strcat(song, argv[i]);
		if (strcmp(colName[i],"Vote_status") == 0)
			{
				strcat(song, " - ");
				strcat(song, argv[i]);
				strcat(song,"\n");
			}	
	}
    return 0;
}

int callback_logIn_name(void *data, int argc, char **argv, char **colName)
{ 	
    for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i],"admin")==0) admin=1; 
		else admin=0;
	}   
    return 0;
}

int callback_logIn_password(void *data, int argc, char **argv, char **colName)
{ 	
    for (int i = 0; i < argc; i++)
	{
		if (strcmp(argv[i],pwd)==0) password=1; 
		else password=0;
	}   
    return 0;
}

int logIn(sqlite3 *db, char username[20])
{	
	int rc;	
	char *err_msg = 0;
	char sql[100]="Select type from Users where Name='";
	strcat(sql,username);
	strcat(sql,"';");
	printf("SQL-ul de slectat este %s \n", sql);
	rc = sqlite3_exec(db, sql, callback_logIn_name, 0, &err_msg);
    
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "[logIn]Failed to select data\n");
   		fprintf(stderr, "[logIn]SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	//printf("ADMIN ESTE %d \n",admin);
	if (admin==-1) return 0; //admin == -1 means we didn't find the client in our database
	
	strcpy(sql,"Select Password from Users where Name='");
	strcat(sql,username);
	strcat(sql,"';");
	//printf("SQL-ul de slectat este %s \n", sql);
	rc = sqlite3_exec(db, sql, callback_logIn_password, 0, &err_msg);
	//printf("PASSWORD DUPA EXECUTIA INTEROGARII ESTE %d\n", password);
	if (password==0) return 0;
	return 1;
}

void songId(sqlite3 *db, char song[30])
{
	printf("AM INTRAT IN SONGID\n");
	char *err_msg = 0;
	char sql[100]="Select id from Top where Title='";
	strcat(sql, song);
	strcat(sql, "';");
	printf("[server - sonId]The statement is: %s\n",sql);
	id=-1;
	sqlite3_exec(db, sql, callback_ID, 0, &err_msg);
}

void userId(sqlite3 *db, char username[30])
{
	//printf("AM INTRAT IN userID\n");
	char *err_msg = 0;
	char sql[100]="Select id from Users where Name='";
	strcat(sql, username);
	strcat(sql, "';");
	printf("[server - userId]The statement is: %s\n",sql);
	id=-1;
	sqlite3_exec(db, sql, callback_ID, 0, &err_msg);
}

void lastId(sqlite3 *db, char table[20])
{
	char *err_msg = 0;
	char sql[100]="Select * from ";
	strcat(sql, table);
	last_id=1;
	if (type_music=='\0')
	{
		strcpy(sql, "Select count(*) from Top where type='");
		strcat(sql,type_music);
		strcat(sql,"'");
		strcat(sql, ";");
		printf("[server - lastId]The statement is: %s\n",sql);
		sqlite3_exec(db, sql, callback_lastID, 0, &err_msg);	
	}
	else
	{
		strcat(sql, ";");
		printf("[server - lastId]The statement is: %s\n",sql);
		sqlite3_exec(db, sql, callback_lastID, 0, &err_msg);
		//printf("last_id este %d !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n",last_id);
	}
	
}

int checkUsername(sqlite3 *db, char username[100])
{
	char *err_msg = 0;
	char sql[100]="Select id from Users where Name='";
	strcat(sql,username);
	strcat(sql,"';");
	userId(db,username);
	printf("[server-checkUsername] sql este %s\n",sql);
	sqlite3_exec(db, sql, callback_name, 0, &err_msg);
	printf("ID ESTE %d SI USER_ID ESTE %d\n", id, user_id);
	if(id==-1) return 0;
	if (user_id==id) return 1;
	return 0;
}

char* prepare_stmt(sqlite3 *db, char username[20])
{	
	char lastID[4];
	char *err_msg = 0;
	lastId(db, "Users");
	sprintf(lastID,"%d",last_id);
	//printf("LAST ID este %s si last_id calculat de fct este %d \n",lastID, last_id);
	
	strcpy(sql,"INSERT INTO Users VALUES(");
	strcat(sql,lastID);
	strcat(sql,", '");
	strcat(sql,username);
	strcat(sql,"', '");
	strcat(sql,pwd);
	strcat(sql,"', '");
	strcat(sql,type_char);
	strcat(sql,"', ");
	strcat(sql,"0, ");
	strcat(sql,"1);");
	return sql;
}

int signIn(sqlite3 *db, char username[20])
{
	int rc;
	char *err_msg;
	//printf("Am intrat in sigIN\n");
	if(checkUsername(db,username)) { printf("Returnam 0 ca exista numele \n"); return 0;}
	if (logIn(db,username)) { printf("Returnam 0 ca nu exista \n"); return 0;}

	char sql[100];
	strcpy(sql,prepare_stmt(db,username));
	//printf("USERNAME-UL ESTE %s \n", username);
	printf("[server - signIn]The statement is: %s\n",sql);
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
 
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "[signIn]Failed to select data\n");
   		fprintf(stderr, "[signIn]SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	return 1;
}

void createUser_table(sqlite3 *db)
{
	int rc;
	char *err_msg = 0;	
   	char *sql = "DROP TABLE IF EXISTS Users;"
	             "CREATE TABLE Users(Id INTEGER PRIMARY KEY, Name TEXT, Password TEXT, Type TEXT, Status INT, Vote_status INT);" 
	             "INSERT INTO Users VALUES(1, 'Gabi', 'gabi97', 'admin', 0, 1);" 
   		     "INSERT INTO Users VALUES(2, 'Dani', 'dani97', 'normal', 0, 1);" 
		     "INSERT INTO Users VALUES(3, 'Elena', 'elena97', 'normal', 0, 1);"
		     "INSERT INTO Users VALUES(4, 'Bianca', 'bianca96', 'normal', 0, 1);";

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
       	
   	if (rc != SQLITE_OK ) 
	{
        	fprintf(stderr, "[createUser]Failed to create table Users\n");
  		fprintf(stderr, "[createUser]SQL error: %s\n", err_msg);
    	 	sqlite3_free(err_msg); 		     
   	} 
	else
	{  
  		fprintf(stdout, "Table Users created successfully\n");
    	}
}

void showTop(int client, sqlite3 *db)
{	
	int rc;	
	char *err_msg = 0;
	char *sql="Select * from Top order by Count desc;";
	printf("[server - showTop]The statement is: %s\n",sql);
	rc = sqlite3_exec(db, sql, callback_show, 0, &err_msg);
    
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	int length=strlen(song)+1;
	printf("[server - showTop]We are sending back %d and %s\n",length, song);
	if(send(client, &length, 4, 0) == -1)
	{
 			perror("[server -showTop]Eroare la send() catre client.\n");
	}
	if(send(client, song, length, 0) == -1)
	{
 			perror("[server - showTop]Eroare la send() catre client.\n");
	}
	
}

void showTop_type(int client, sqlite3 *db)
{
	int rc;	
	char sql[100]="Select * from Top where Type=";
	char *err_msg = 0;

	strcat(sql,"'");
	strcat(sql,type_music);
	strcat(sql,"'");
	strcat(sql," order by Count desc;");
	printf("[server - showType]The statement is: %s\n",sql);
	//printf("TYPE_MUSIC ESTE %s\n", type_music);
	rc = sqlite3_exec(db, sql, callback_show, 0, &err_msg);
    
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	if(strlen(song)>0)
	{
		int length=strlen(song)+1;
		printf("[server - showTop_type]We are sending back %d and %s\n",length, song);
		if(send(client, &length, 4, 0) == -1)
		{
	 			perror("[server - showTop_type]Eroare la send() catre client.\n");
		}
		if(send(client, song, length, 0) == -1)
		{
	 			perror("[server - showtTop_type]Eroare la send() catre client.\n");
		}
	}
	else
	{
		char* msg1="The type doesn't exist!\n";
		int length=strlen(msg1)+1;
		printf("[server - showTop_type]We are sending back %d and %s\n",length, msg1);
		if(send(client, &length, 4, 0) == -1)
		{
  			perror("[server -showTop_type]Eroare la send() catre client.\n");
		}
		if(send(client, msg1, length, 0) == -1)
		{
  			perror("[server - showTop_type]Eroare la send() catre client.\n");
		}	
	}
}

void addSong(sqlite3 *db, struct song Song)
{
	strcpy(type_music, Song.type);
	int rc;
	char lastID[4], *err_msg;
	lastId(db, "Top");
	sprintf(lastID,"%d",last_id1+1);
	printf("ID este %s \n",lastID);
	char sql[1000]="INSERT INTO Top VALUES(";
	strcat(sql,lastID);
	strcat(sql,", '");
	strcat(sql,Song.artist);
	strcat(sql,"', '");
	strcat(sql,Song.title);
	strcat(sql,"', '");
	strcat(sql,Song.type);
	strcat(sql,"', 0, '");
	strcat(sql,Song.link);
	strcat(sql,"')");
	printf("[server - addSong]The statement is: %s\n",sql);
	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	else 
	{
		char* msg1="You have added the song to the playlist!\n";
		int length=strlen(msg1)+1;
		printf("[server - addSong]We are sending back %d and %s\n",length, msg1);
		if(send(client, &length, 4, 0) == -1)
		{
  			perror("[server -addSong]Eroare la send() catre client.\n");
		}
		if(send(client, msg1, length, 0) == -1)
		{
  			perror("[server - addSong]Eroare la send() catre client.\n");
		}
	}
}

void addComment(sqlite3 *db, char song[30], char comment[100])
{
	int rc;
	char lastID[4], *err_msg;
	songId(db, song);
	sprintf(lastID,"%d",id);
	//printf("ID din addComment este %s \n",lastID);
	char sql[100]="INSERT INTO Comments VALUES(";
	strcat(sql,lastID);
	strcat(sql,", '");
	strcat(sql, comment);
	strcat(sql, "');");
	printf("[server - addComment]The statement is: %s\n",sql);

	rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	else 
	{
		char* msg1="You succesfully added the comment!\n";
		int length=strlen(msg1)+1;
		printf("[server - addSong]We are sending back %d and %s\n",length, msg1);
		if(send(client, &length, 4, 0) == -1)
		{
  			perror("[server -addSong]Eroare la send() catre client.\n");
		}
		if(send(client, msg1, length, 0) == -1)
		{
  			perror("[server - addSong]Eroare la send() catre client.\n");
		}
	}
}

void getStatus(sqlite3 *db)
{
	int rc;
	char* err_msg, ID[4];	
	char sql[100]= "Select Vote_status from Users where id='";
	sprintf(ID,"%d",user_id);
	strcat(sql,ID);
	strcat(sql,"';");
	printf("[server - getStatus]The statement is: %s\n",sql);
	rc = sqlite3_exec(db, sql, callback_status, 0, &err_msg);
	  
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
}

void updateTop(sqlite3 *db, char msg[30])
{
	status=0;
	int rc;	
	char* err_msg;	

	getStatus(db); // get the status for the current user by id
	//printf("STATUSUL DE VOT ESTE %d !!!!!!!!!!!!!!!\n", status);
	if (status==1)
	{
		char sql[100]= "Update Top set count=count+1 where title='";
		songId(db,msg);
		strcat(sql, msg);
		strcat(sql,"';");		
		rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	  
		if (rc != SQLITE_OK )
		{   
			fprintf(stderr, "Failed to select data\n");
   			fprintf(stderr, "SQL error: %s\n", err_msg);
    			sqlite3_free(err_msg);
     			sqlite3_close(db);
    		} 
		char *msg1="You have voted!\n";
		int length = strlen(msg1)+1;
		send(client, &length, 4, 0);
		send(client, msg1, strlen(msg1)+1, 0);
		}
		else 
		{
			char *msg1="You can't vote!\n";
			int length = strlen(msg1)+1;
			printf("[server - updateTop]We are sending back %d and %s\n",length, msg1);
			if(send(client, &length, 4, 0) == -1)
			{
  				perror("[server - updateTop]Eroare la send() catre client.\n");
			}
			if(send(client, msg1, length, 0) == -1)
			{
  				perror("[server - updateTop]Eroare la send() catre client.\n");
			}
	}	
}

void blockUser(sqlite3 *db, char username[30])
{
	char *err_msg, ID[4], msg[100], length;
	char sql[100]="Update Users set Vote_status=Vote_status-1 where id='";
	sprintf(ID,"%d",id);
	strcat(sql, ID);
	strcat(sql, "';");
	printf("[server - blockUser]The statement is: %s\n",sql);
	int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	if (id == -1)
	{
		strcpy(msg, "Username doesn't exist!\n");
	}
	else
	{
		strcpy(msg, "You succesfully blocked the user!\n");
	}	
	length=strlen(msg)+1;
	printf("[server - blockUser]We are sending back %d and %s\n",length, msg);
	if (send(client, &length, 4, 0) == -1)
	{
  		perror("[server - blockUser]Eroare la send() catre client.\n");
	}
	if (send(client, msg, length, 0) == -1)
	{
  		perror("[server - blockUser]Eroare la send() catre client.\n");
	}	
}

void unblockUser(sqlite3 *db, char username[30])
{	
	int length;
	char *err_msg, ID[4], msg[100];
	char sql[100]="Update Users set Vote_status=Vote_status+1 where id='";
	sprintf(ID,"%d",id);
	strcat(sql, ID);
	strcat(sql, "';");
	printf("[server - unblockUser]The statement is: %s\n",sql);
	int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 
	if (id == -1)
	{
		strcpy(msg, "Username doesn't exist!\n");
	}
	else
	{
		strcpy(msg, "You succesfully unblocked the user!\n");
	}	
	length=strlen(msg)+1;
	printf("[server - unblockUser]We are sending back %d and %s\n",length, msg);
	if (send(client, &length, 4, 0) == -1)
	{
  		perror("[server -unblockUser]Eroare la send() catre client.\n");
	}
	if(send(client, msg, length, 0) == -1)
	{
  		perror("[server -unblockUser]Eroare la send() catre client.\n");
	}	
}

void deleteSong(sqlite3 *db, char username[30])
{
	int length;
	char *err_msg, ID[4], msg[100];
	sprintf(ID,"%d",id);
	char sql[200]=  "BEGIN;" 
			"DELETE FROM Comments WHERE id = ";
			strcat(sql,ID);
			strcat(sql,";");
  			strcat(sql,"DELETE FROM Top WHERE id = "); 
			strcat(sql,ID);
			strcat(sql,";");
			strcat(sql,"COMMIT; ");
	printf("[server - deleteSong]The statement is: %s\n",sql);
	//sprintf(ID,"%d",id);
	//strcat(sql, ID);
	//strcat(sql, "';");
	
	sqlite3_exec(db, sql, 0, 0, &err_msg);
	if (id == -1)
	{
		strcpy(msg, "Song doesn't exist!\n");
	}
	else
	{
		strcpy(msg, "You succesfully deleted the song!\n");
	}	
	length=strlen(msg)+1;
	printf("[server - deleteSong]We are sending back %d and %s\n",length, msg);
	if (send(client, &length, 4, 0) == -1)
	{
  		perror("[server - deleteSong]Eroare la send() catre client.\n");
	}		
	if (send(client, msg, length, 0) == -1)
	{
  		perror("[server - deleteSong]Eroare la send() catre client.\n");
	}	
}

void showUsers(sqlite3 *db)
{	
	poz=1;
	int rc;	
	//printf("SUNTEM IN showusers\n");
	char *err_msg = 0;
	char *sql="Select * from Users;";
	printf("[server - showUsers]The statement is: %s\n",sql);
	rc = sqlite3_exec(db, sql, callback_showUsers, 0, &err_msg);
    
	if (rc != SQLITE_OK )
	{   
		fprintf(stderr, "Failed to select data\n");
   		fprintf(stderr, "SQL error: %s\n", err_msg);
    		sqlite3_free(err_msg);
     		sqlite3_close(db);
    	} 

	int length=strlen(song)+1;
	printf("[server - addSong]We are sending back %d and %s\n",length, song);
	if(send(client, &length, 4, 0) == -1)
	{
 			perror("[server -addSong]Eroare la send() catre client.\n");
	}
	if(send(client, song, length, 0) == -1)
	{
 			perror("[server - addSong]Eroare la send() catre client.\n");
	}
}

int main ()
{
  struct sockaddr_in server;	
  struct sockaddr_in from;	        
  int sd, msg_length=0, pwd_length=0;	
  sqlite3 *db;
  int rc = sqlite3_open("database.db", &db);
    
  if (rc != SQLITE_OK) 
	{        
    	    fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
    	    sqlite3_close(db);
       	    return 1;
	}
  else 
	{
	    printf("Database is open! \n");
	}

  //createUser_table(db);	 
  
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror("[server]Error at socket().\n");
      return errno;
    }

  bzero(&server, sizeof(server));
  bzero(&from, sizeof(from));

  server.sin_family = AF_INET;	
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);
  
  if (bind(sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror("[server]Error at bind().\n");
      return errno;
    }

  if (listen(sd, 5) == -1)
    {
      perror("[server]Error at listen().\n");
      return errno;
    }
	
  while (1)
    {
      int length = sizeof(from);   

      printf("[server]We are waiting at the port %d...\n",PORT);
      fflush(stdout);

      client = accept(sd, (struct sockaddr *) &from, &length);
	  if (client < 0)
	{
	  perror("[server]Error at accept().\n");
	  continue;
	}

	  int pid;
	  if((pid=fork())==-1)
	  {
		  perror("[server]fork() error. \n");
		  close(client);
		  continue;
	  }
	  if (pid==0)
	 {
		char msg[30];
		char choice[10];
		int choice_length=0;
		while (1)
      		{
			if (recv(client, &choice_length, sizeof(choice_length), 0) == -1)
				{
		  			perror("[server]Error recv() from client.\n");
	  				close(client);	
	  				continue;
				}		
 			
			if (recv(client, choice, choice_length, 0) == -1)
				{
		  			perror("[server]Error recv() from client.\n");
	  				close(client);	
	  				continue;
				}	
			if (length==0) 
			{
				close(client);
				printf("[server] The client disconnected\n");
				break;
			}	
      			printf("\n[server]The choice is %s and has the length %d...\n", choice, choice_length);
			
			if (strcmp(choice,"logIn")==0)
				{
					admin=-1;
					if (recv(client, &msg_length, sizeof(msg_length), 0) == -1)
					{
		  				perror("[server]Error recv() from client.\n");
		  				close(client);	
		  				continue;
					}

					if (recv(client, &msg, msg_length, 0) == -1)
					{
		  				perror("[server]Error recv() from client.\n");
		  				close(client);	
		  				continue;
					}
					
					if (recv(client, &pwd_length, sizeof(pwd_length), 0) == -1)
					{
		  				perror("[server]Error recv() from client.\n");
		  				close(client);	
		  				continue;
					}

					if (recv(client, &pwd, pwd_length, 0) == -1)
					{
		  				perror("[server]Error recv() from client.\n");
		  				close(client);	
		  				continue;
					}
					printf("Am primit de la client username-ul %s cu lungimea %d si parola %s cu lungimea %d\n",msg, msg_length, pwd, pwd_length);

					if (logIn(db, msg)) 
					{
						userId(db, msg);
						user_id=id;
						strcpy(msg, "Succes!");
					}
					else strcpy(msg, "Wrong username or wrong password!");

					length=strlen(msg)+1;
					printf("[server - logIn]Sending back the message: %s\n", msg);
   					if (send(client,&length, 4, 0) <= 0)
						{
  							perror("[server - logIn]Error send() to client.\n");
  							continue;
						}		
	   					else
							printf("[server - logIn]Mesajul lungime_mesaj fost trasmis cu succes.\n");
	   				if (send(client,msg,length, 0) <= 0)
					{
  						perror("[server]Eroare la write() catre client.\n");
  						continue;
					}		
   					else printf("[server]Mesajul mesaj fost trasmis cu succes.\n");
					if (send(client,&admin, 4, 0) <= 0)
						{
  							perror("[server]Eroare la write() catre client.\n");
  							continue;
						}		
	   					else
							printf("[server]Mesajul admin fost trasmis cu succes.\n");
					//showUsers(db);	
					}  
			if (strcmp(choice,"signIn")==0)
				{
					admin=-1;
					if (recv(client, &type, 4, 0) == -1)
					{
		  				perror("[server]Eroare la read() de la client.\n");
	  					close(client);	
	  					continue;
					}
					init_type();
					if (recv(client, &msg_length, sizeof(msg_length), 0) == -1)
					{
		  				perror("[server]Eroare la read() de la client.\n");
		  				close(client);	
		  				continue;
					}
					if (recv(client, &msg, msg_length, 0) == -1)
					{
		  				perror("[server]Eroare la read() de la client.\n");
		  				close(client);	
		  				continue;
					}

					if (recv(client, &pwd_length, sizeof(pwd_length), 0) == -1)
					{
		  				perror("[server]Error recv() from client.\n");
		  				close(client);	
		  				continue;
					}

					if (recv(client, &pwd, pwd_length, 0) == -1)
					{
		  				perror("[server]Error recv() from client.\n");
		  				close(client);	
		  				continue;
					}
					printf("[server - signIn]We received from the client: %d, %s, %d, %s, %d \n", msg_length, msg, pwd_length, pwd, type);	
					
					if (signIn(db,msg))
					 {
						strcpy(msg, "Succes!");
						length=strlen(msg)+1;
						printf("[server]Trimitem mesajul inapoi: %s\n", msg);
						if (send(client,&length,4, 0) <= 0)
						{
  							perror("[server]Eroare la write() catre client.\n");
  							continue;
						}		
	   					else
							printf("[server]Mesajul a fost trasmis cu succes.\n");
	   					if (send(client,msg,length, 0) <= 0)
						{
  							perror("[server]Eroare la write() catre client.\n");
  							continue;
						}		
	   					else
							printf("[server]Mesajul a fost trasmis cu succes.\n");
					}
					else 
					{
						strcpy(msg, "Username already exists!");
						length=strlen(msg)+1;
						printf("[server]Trimitem mesajul inapoi: %s\n", msg);
						if (send(client,&length,4, 0) <= 0)
						{
  							perror("[server]Eroare la write() catre client.\n");
  							continue;
						}		
	   					else
							printf("[server]Mesajul a fost trasmis cu succes.\n");
	   					if (send(client,msg,length, 0) <= 0)
						{
  							perror("[server]Eroare la write() catre client.\n");
  							continue;
						}		
	   					else
							printf("[server]Mesajul a fost trasmis cu succes.\n");
					}
				}
				
			if (strcmp(choice, "add")==0)
				{
					last_id=1;
					lastId(db, "Top");
					if (recv(client, &Song, sizeof(Song), 0) == -1)
					{
		  				perror("[server]Eroare la read() de la client.\n");
		  				close(client);	
		  				continue;
					}
					printf("[server - add]The song is %s - %s, (%s, %s) \n", Song.artist, Song.title, Song.type, Song.description);
					addSong(db, Song);
				}		

			if (strcmp(choice, "show")==0)
				{
					poz=1;
					bzero(&type_music, sizeof(type_music));
					bzero(song, sizeof(song));
					//printf("LAST ID CARE IL TRIMITEM ESTE %d \n", last_id);
					if (send(client, &last_id,4, 0) <= 0)
						{
  							perror("[server]Eroare la write() catre client.\n");
  							continue;
						}		
	   					else
							printf("[server]Mesajul a fost trasmis cu succes.\n");
					showTop(client, db);
				}
			if (strcmp(choice, "showType")==0)
			{
					poz=1;
					int type_length;
					bzero(song, sizeof(song));
					if (recv(client, &type_length, sizeof(type_length), 0) == -1)
					{
		  				perror("[server]Eroare la read() de la client.\n");
		  				close(client);	
		  				continue;
					}		
 				
					if (recv(client, &type_music, type_length, 0) == -1)
					{
			  			perror("[server]Eroare la read() de la client.\n");
		  				close(client);	
		  				continue;
					}
					printf("[server - showType]We received from the client: %d, %s\n", type_length, type_music);		
				        showTop_type(client, db);
					bzero(&type_music, sizeof(type_music));
						
			}
			if (strcmp(choice, "vote")==0)
			{	
				if (recv(client, &msg_length, sizeof(msg_length), 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
	
				if (recv(client, &msg, msg_length, 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				printf("[server - vote]We received from the client: %d, %s\n", msg_length, msg);
				id=-1;
				songId(db, msg);
				if (id==-1)	
				{
					strcpy(msg, "The song doesn't exists!\n");
					length=strlen(msg)+1;
					if (send(client,&length,4, 0) <= 0)
					{
  						perror("[server]Eroare la send() catre client.\n");
  						continue;
					}		
	   				if (send(client,msg,length, 0) <= 0)
					{
  						perror("[server]Eroare la send() catre client.\n");
  						continue;
					}		
				}
				else updateTop(db,msg);
			}

			if (strcmp(choice,"comment")==0)
			{
				char song[100];
				int song_length;
				if (recv(client, &song_length, 4, 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
	
				if (recv(client, &song, song_length, 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				printf("[server - comment] We received the song %s with the length %d. \n",song, song_length);
				id=-1;
				songId(db, song);
				if (send(client,&id,4, 0) <= 0)
					{
  						perror("[server]Eroare la send() catre client.\n");
  						continue;
					}
				else printf("[server - comment] We send with succes: %d!\n",id);	
				
				if (id==-1)	
				{
					strcpy(msg, "The song doesn't exists!\n");
					length=strlen(msg)+1;
					if (send(client,&length,4, 0) <= 0)
					{
  						perror("[server]Eroare la send() catre client.\n");
  						continue;
					}		
	   				if (send(client,msg,length, 0) <= 0)
					{
  						perror("[server]Eroare la send() catre client.\n");
  						continue;
					}		
				}
				else 
				{

					if (recv(client, &msg_length, 4, 0) == -1)
					{
			  			perror("[server]Eroare la read() de la client.\n");
			  			close(client);	
			  			continue;
					}
					if (recv(client, &msg, msg_length, 0) == -1)
					{
			  			perror("[server]Eroare la read() de la client.\n");
			  			close(client);	
			  			continue;
					}
					printf("[server - comment]: We received from the client %d, %s, %d, %s \n",song_length, song, msg_length, msg);		
					addComment(db, song, msg);
				}
			}
			if (strcmp(choice,"exit")==0)
			{
				close(client);
				break;
			}
			if (strcmp(choice,"block")==0)
			{
				if (recv(client, &msg_length, sizeof(msg_length), 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				if (recv(client, &msg, msg_length, 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				printf("[server - block]We received from the client: %d, %s \n", msg_length, msg);	
				userId(db, msg);
				//printf("ID-UL ESTE %d \n", id);	
				blockUser(db, msg);	
			}

			if (strcmp(choice,"unblock")==0)
			{
				if (recv(client, &msg_length, sizeof(msg_length), 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				if (recv(client, &msg, msg_length, 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				printf("[server - unblock]We received from the client: %d, %s \n", msg_length, msg);	
				userId(db, msg);
				//printf("ID-UL ESTE %d \n", id);	
				unblockUser(db, msg);	
			}
			
			if (strcmp(choice,"delete")==0)
			{
				//printf("SUNTEM IN DELETE\n");
				if (recv(client, &msg_length, sizeof(msg_length), 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				if (recv(client, &msg, msg_length, 0) == -1)
				{
		  			perror("[server]Eroare la read() de la client.\n");
		  			close(client);	
		  			continue;
				}
				printf("[server - delete]We received from the client: %d, %s\n", msg_length, msg);	
				songId(db, msg);
				//printf("ID-UL ESTE %d \n", id);	
				deleteSong(db, msg);		
			}
			
			if (strcmp(choice,"showUsers")==0)
			{
				bzero(song, sizeof(song));
				printf("[server - showUsers]We received from the client: %d, %s\n", msg_length, msg);	
				showUsers(db);		
			}

			}//while
			exit(0);
	}//if
	else
	{

		int status;
		while(waitpid(-1,&status,WNOHANG));
		close(client);
	} 
   }				
}				
