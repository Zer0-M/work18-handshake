#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  int s2cp=mkfifo("s2c",0644);
  if(s2cp==-1){
    printf("%s \n",strerror(errno));
  }
  printf("Well-Known FIFO s2c created\n");
  int upstream=open("s2c",O_RDONLY);
  if(upstream==-1){
    printf("%s \n",strerror(errno));
  }
  char buff[HANDSHAKE_BUFFER_SIZE];
  read(upstream,buff,HANDSHAKE_BUFFER_SIZE);
  char * wrfile=buff;
  printf("Name of Client's Private Pipe Received: %s\n",wrfile);
  *to_client=open(wrfile,O_WRONLY);
  remove("s2c");
  printf("Removed WKP\n");
  if(*to_client==-1){
    printf("%s \n",strerror(errno));
  }
  write(*to_client,ACK,HANDSHAKE_BUFFER_SIZE);
  char response[HANDSHAKE_BUFFER_SIZE];
  read(upstream,response,HANDSHAKE_BUFFER_SIZE);
  printf("Response received from client\n");
  printf("Handshake Complete\n");
  return upstream;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int pid=getpid();
  int c2sp=mkfifo(privatepipe,0644);
  if(c2sp){
    printf("%s \n",strerror(errno));
  }
  int downstream=open("s2c",O_WRONLY);
  if(downstream==-1){
    printf("%s \n",strerror(errno));
  }
  write(downstream,privatepipe,HANDSHAKE_BUFFER_SIZE);
  printf("Sent FIFO name: %d\n",privatepipe);
  *to_server=open(privatepipe,O_RDONLY);
  if(*to_server==-1){
    printf("%s \n",strerror(errno));
  }
  char * buff=malloc(sizeof(char));
  read(*to_server,buff,HANDSHAKE_BUFFER_SIZE);
  printf("Message Received From Server\n");
  remove(privatepipe);
  printf("Private FIFO removed\n");
  write(wfd,ACK,HANDSHAKE_BUFFER_SIZE);
  printf("Response sent to server\n");  
  return downstream;
}
