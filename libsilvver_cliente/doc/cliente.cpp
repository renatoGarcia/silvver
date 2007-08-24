/********************************************************************************
*    Este programa exemplifica o mecanismo de conexão de um programa cliente    *
*  com o programa silver-servidor. Ele aceita até dois argumentos, sendo o      *
*  primeiro o id do marco a ser localizado, e o segundo o endereço IP           *
*  de onde o programa silver-servidor está sendo executado.                     *
********************************************************************************/

#include <silver_cliente.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;

int main(int argc,char **argv)
{
  int id = 0;
  char ip[16] = "127.0.0.1";

  if(argc>1)
  {
    id = atoi(argv[1]);
  }
  if(argc>2)
  {
    strncpy(ip,argv[2],16);
  }

  Marco marco(ip,id);
  cout << "Conectando com o servidor..." << endl;
  marco.conectar();
  cout << "Conectado."<< endl << endl;

  double x,y,teta;

  for(int i=500; i>0; i--)
  {
    marco.getPose(x,y,teta);
    cout << "X: " << x << "\tY: " << y << "\tTeta: " << teta << endl;
  }

  return 0;
}
