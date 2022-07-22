#include <arpa/inet.h>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <mutex>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "../funcs/message.h"
#include "../funcs/process_request.h"
#include "../funcs/safe_queue.h"

using namespace std;

SafeQueue<ProcessRequest> request_queue; 
SafeQueue<string> logFila; 
int sockfd;
mutex mutex_;
condition_variable cv;
map<string, int> contadorProcesso; 

enum command{
  printFila,
  printHistorico,
  saindo
};

//escrever logs de forma segura
void escrevendoLog() {
  ofstream arquivoLog;
  while (true){         
    string msg = logFila.dequeue();
    arquivoLog.open("log.txt", ios_base::app);
    arquivoLog << msg << "\n";
    arquivoLog.close();
  }  
}

string editandoLog(string timeStamp, string ID_message, string process_PID){
  string log = timeStamp;
  log += SEPARATOR;
  log += ID_message;
  log += SEPARATOR;
  log += process_PID;
  return log;
}

//designa a acao para determinado tipo de mensagem
void mensagemCliente(message msg, struct sockaddr_in clientaddr){
  auto hora = chrono::system_clock::now(); 
  time_t horario = chrono::system_clock::to_time_t(hora); // passar p horario "humano dps"
        
  if (strcmp(msg.ID_message, REQUEST)==0){
    logFila.enqueue(editandoLog(to_string(horario),"REQUEST", msg.process_PID));
    ProcessRequest request = ProcessRequest(msg, clientaddr);
    request_queue.enqueue(request);
  }else if(strcmp(msg.ID_message, RELEASE) == 0){
    logFila.enqueue(editandoLog(to_string(horario),"RELEASE", msg.process_PID));
    cv.notify_one();
  }
}

//======================== Conexão UDP ==============================

void socketHandler(){
  cout << "Servidor UDP\n" << endl;
  int vopt;
  int n;
  socklen_t tamanhoCliente;
  char buffer[MESSAGE_SIZE];
  struct sockaddr_in serveraddr, clientaddr;
  struct hostent *hostp;
  char *hostaddrp;
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //Socket file descriptor

  vopt = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&vopt, sizeof(int));

  //configurações do endereço
  bzero((char *) &serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons((unsigned short)COORDINATOR_PORT);

  bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr)); 

  //loop principal esperando informações
  tamanhoCliente = sizeof(clientaddr);
  while (true){
    bzero(buffer, MESSAGE_SIZE);
    n = recvfrom(sockfd, buffer, MESSAGE_SIZE, 0, (struct sockaddr *) &clientaddr, &tamanhoCliente);

    message msg = msg_to_struct(buffer);
    if(msg.readable){
      mensagemCliente(msg, clientaddr);
    }else{
      cout << "Erro na mensagem" << endl;
    }
  }
}
//======================== Conexão UDP ============================== 


command parserComandos(string command){
  if((command == "queue") || (command == "1")){
    return printFila;
  }else if ((command == "history") || (command == "2")){
    return printHistorico;
  }else if ((command == "exit") || (command == "3")){
    return saindo;
  }     
}

void mostrarContador(){
  map<string, int>::iterator itr;
  cout << "PID\t\tContador\n";
  for (itr = contadorProcesso.begin(); itr != contadorProcesso.end(); ++itr){
    cout << itr->first << '\t'<<'\t' << " "<< itr->second << '\n';
  }
  cout << endl;
}

void mostrarFila(){
  int count = 1;
  queue<ProcessRequest> currentQueue = request_queue.copy_queue();
  cout << "#\tPID\n";
  while (!currentQueue.empty()){
    ProcessRequest enfileirado = currentQueue.front();
    currentQueue.pop();
    cout << count <<"\t" << enfileirado.get_msg().process_PID << '\n';
    count +=1;
  }
  cout << endl;
}

void mostrarInterface(){
  string command;
  while (1){
    getline(cin, command);
    switch (parserComandos(command)){
      case printFila:
        mostrarFila();
        break;
      case printHistorico:
        mostrarContador();
        break;
      case saindo:
        kill(getpid(), 9);      
    }
  }
}

//controla acesso a regiao critica --> passa valor do horario no log
void acessoRegiaoCritica(){
  int n;
  while (1){   
    ProcessRequest request = request_queue.dequeue();
    struct sockaddr_in clientaddr = request.get_clientaddr();
    string resposta = msg_to_string(GRANT);
    n = sendto(sockfd, resposta.c_str(), strlen(resposta.c_str()), 0, (struct sockaddr *) &clientaddr, sizeof(clientaddr));
    auto hora = chrono::system_clock::now();
    time_t horario = chrono::system_clock::to_time_t(hora);
    logFila.enqueue(editandoLog(to_string(horario),"GRANT", request.get_msg().process_PID));

    map<string, int>::iterator iterando = contadorProcesso.find(request.get_msg().process_PID);

    if (iterando == contadorProcesso.end()){
      contadorProcesso.insert(pair<string, int>(request.get_msg().process_PID, 1));
    }else{
      iterando->second ++; 
    }
    unique_lock<mutex> lock(mutex_);
    cv.wait(lock);
  }  
}

int main() {
  vector<thread> processo;
  processo.emplace_back(socketHandler);
  processo.emplace_back(acessoRegiaoCritica);
  processo.emplace_back(mostrarInterface);
  processo.emplace_back(escrevendoLog);

  for (auto& program_thread : processo){
    program_thread.join();
  }
}