/********************************************************************************
*    Este programa exemplifica o mecanismo de conexão de um programa cliente    *
*  com o programa silver-servidor. Ele aceita até dois argumentos, sendo o      *
*  primeiro o id do marco a ser localizado, e o segundo o endereço IP           *
*  de onde o programa silver-servidor está sendo executado.                     *
********************************************************************************/

#include <target.hpp>
#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

int main(int argc,char **argv)
{
  int id = 1;
  std::string ip("127.0.0.1");

  if(argc>1)
  {
    id = atoi(argv[1]);
  }
  if(argc>2)
  {
    ip = argv[2];
  }

  Target target(id,false,ip);
  cout << "Conectando com o servidor..." << endl;
  target.connect();
  cout << "Conectado."<< endl << endl;

  double x,y,teta;

  for(int i=10; i>0; i--)
//   while(true)
  {
    target.getPose(x,y,teta);
    cout << "X: " << x << "\tY: " << y << "\tTeta: " << teta << endl;
    system("sleep 0.5");
  }

  return 0;
}
