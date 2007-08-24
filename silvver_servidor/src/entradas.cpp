#include "entradas.h"
#include <iostream>

mutex Entradas::mutexInstanciacao;
auto_ptr<Entradas> Entradas::instanciaUnica;

Entradas* Entradas::Instanciar()
{
  mutex::scoped_lock lock(mutexInstanciacao);
  if( instanciaUnica.get()==0 )
      instanciaUnica.reset(new Entradas());

  return instanciaUnica.get();
}

Entradas::Entradas()
{
  saidas = Saidas::Instanciar();
}

void Entradas::AdicionarEntrada(Conexao *conexao)
{
  char msg[3];
  TipoDado tipoDado;

  conexao->Receber( msg,sizeof(msg) );
  cout << "Confirma conexao: " << msg << endl;

  conexao->Receber( &tipoDado,sizeof(TipoDado) );
  cout << "Tipo de dado: " << tipoDado2string(tipoDado) << endl << endl;

  switch(tipoDado)
  {
    case CORES:
      threadCliente.push_back( new thread( bind(&Entradas::ClienteBlobCam,this,conexao) ) );
      break;
    case MARCAS:
      threadCliente.push_back( new thread( bind(&Entradas::ClienteMarcaCam,this,conexao) ) );
      break;
    default:
      break;
  }
}

string Entradas::tipoDado2string(TipoDado td)
{
  string str;
  switch(td)
  {
    case CORES:
      str = "cores";
      break;
    case MARCAS:
      str = "marcas";
      break;
    default:
      str = "tipo não identificado";
      break;
  }
  return str;
}

void Entradas::ClienteBlobCam(Entradas *objeto, Conexao *conexao)
{
  char msgOK[3] = "OK";

  BlobTratador *tratador = BlobTratador::Instanciar();

  conexao->Enviar(msgOK,sizeof(msgOK));

  Pacote<Marca> pacote;
  while(true)
  {
    conexao->Receber( &pacote,sizeof(pacote) );
    tratador->EntregarPacotes(pacote);
    //saidas->
  }
}

void Entradas::ClienteMarcaCam(Entradas *objeto, Conexao *conexao)
{
  char msgOK[3] = "OK";

  MarcaTratador *tratador = MarcaTratador::Instanciar();

  conexao->Enviar(msgOK,sizeof(msgOK));

  Pacote<Ente> pacote;
  vector<Ente> resultados;
  while(true)
  {
    conexao->Receber( &pacote,sizeof(pacote) );
    tratador->EntregarPacotes(pacote);
    tratador->Localizar(resultados);
    objeto->saidas->ReceberEstado(resultados);
  }
}
