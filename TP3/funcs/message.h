#include <stdio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#ifndef MESSAGE //--> correçao rápida do problema de redefinition
#define MESSAGE
#define COORDINATOR_PORT 2022
#define MESSAGE_SIZE 16
#define REQUEST "1"
#define GRANT "2"
#define RELEASE "3"
#define SEPARATOR "|"

using namespace std;

/*
Mensagem esperada:
  "ID Mensagem|PID do Processo|000"
"000" só é acrescentado para atingir o valor de bytes estabelecido.
*/

struct message{
  const char* ID_message; 
  string process_PID;
  string zero_padding;
  bool readable; 
};

string msg_to_string(const char* ID_message);

message msg_to_struct(string string_message);

#endif //---> correçao rápida do problema de redefinition