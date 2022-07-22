#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "../funcs/message.h"

using namespace std;

#define MESSAGE_SIZE 16
#define REQUEST "1"
#define GRANT "2"
#define RELEASE "3"
#define SEPARATOR "|"


void escrevendo(int k){ 
  ofstream newFile;
  auto hora = chrono::system_clock::now();
  time_t horario = chrono::system_clock::to_time_t(hora);
  time (&horario);
  newFile.open("resultado.txt",ios_base::app);
  newFile << ctime(&horario) << " , " << to_string(getpid()) << endl;
  newFile.close();
  sleep(k); //aguarda k segundos
}


void conectandoCoordenador(int sockfd, int n, int k){
  char buffer[MESSAGE_SIZE]; 
  string request,release;
  int conexao;
  struct sockaddr_in serveraddr;
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(COORDINATOR_PORT);
  serveraddr.sin_addr.s_addr = INADDR_ANY; 
 // n repetiçoes
  while(n>0){
    //enviando request
    request = msg_to_string(REQUEST);
    conexao = sendto(sockfd, request.c_str(), strlen(request.c_str()), 0,(struct sockaddr *) &serveraddr, sizeof(serveraddr));
    conexao = recv(sockfd, buffer, MESSAGE_SIZE, 0); 

    message mensagem = msg_to_struct(buffer);
    if(mensagem.readable){
      if(strcmp(mensagem.ID_message, GRANT) == 0){ //entramos! 
        escrevendo(k);
        n--;
        release = msg_to_string(RELEASE);
        conexao = sendto(sockfd, release.c_str(), strlen(release.c_str()), 0,(struct sockaddr *) &serveraddr, sizeof(serveraddr));
      }
    }else{
      cout << "Não consigo ler mensagem" <<endl;
    }
  }
}

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  int portaProcesso = atoi(argv[2]);
  int k = atoi(argv[3]);
  struct sockaddr_in clientaddr;
  int sockfd;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  clientaddr.sin_family = AF_INET;
  clientaddr.sin_port = htons(portaProcesso);
  clientaddr.sin_addr.s_addr = INADDR_ANY;
  bind(sockfd, (struct sockaddr *) &clientaddr, sizeof(clientaddr));
  conectandoCoordenador(sockfd, n, k);
  close(sockfd);
}