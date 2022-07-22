#include "message.h"

using namespace std;

/*
Mensagem esperada:
  "ID Mensagem|PID do Processo|000"
"000" só é acrescentado para atingir o valor de bytes estabelecido.
*/

//transforma a mensagem para o formato desejado
string msg_to_string(const char* ID_message) {
  string message = ID_message;
  message += SEPARATOR;
  message += to_string(getpid());
  message += SEPARATOR;

  // fazendo o zero_padding
  while (message.size() < MESSAGE_SIZE) {
    message += "0";
  }
  return message;
}

// transforma mensagem em string para a struct criada
message msg_to_struct(string encodificada){
  message mensagem;
  size_t index = encodificada.find(SEPARATOR);
  if(index == string::npos){
    mensagem.readable = false;
  }
  string findType = encodificada.substr(0, index);
  if(findType != "1" && findType != "2" && findType != "3"){
    mensagem.readable = false;
    return mensagem;
  }
  if(findType == "1"){
    mensagem.ID_message = REQUEST;
  }
  if(findType == "2"){
    mensagem.ID_message = GRANT;
  }
  if(findType == "3"){
    mensagem.ID_message = RELEASE;
  }
  encodificada.erase(0, index + 1);
  index = encodificada.find(SEPARATOR);
  if(index == string::npos){
    mensagem.readable = false;
  }else{
    mensagem.process_PID = encodificada.substr(0, index);
    mensagem.readable = true;
  }
  return mensagem;
}
