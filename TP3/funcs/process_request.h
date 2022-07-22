#include "message.h"
#include <arpa/inet.h>

using namespace std;

// processa os pedidos dos clientes, retorna endereço do cliente e mensagem
class ProcessRequest{
  private:
    struct sockaddr_in clientaddr;
    message msg;

  public:
    ProcessRequest(message m, struct sockaddr_in c){
      msg = m;
      clientaddr = c;
    }

    ProcessRequest();

    struct sockaddr_in get_clientaddr(){
      return clientaddr;
    }

    message get_msg(){
      return msg;
    }    
};
