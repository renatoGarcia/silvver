#include "entradas.hpp"
#include <iostream>

mutex Inputs::mutexInstanciacao;
auto_ptr<Inputs> Inputs::instanciaUnica;

Inputs* Inputs::Instanciar()
{
  mutex::scoped_lock lock(mutexInstanciacao);
  if( instanciaUnica.get()==0 )
      instanciaUnica.reset(new Inputs());

  return instanciaUnica.get();
}

Inputs::Inputs()
{
  saidas = Saidas::Instanciar();
}

void Inputs::AdicionarEntrada(Connection *conexao)
{
  char msg[3];
  DataType dataType;

  conexao->receive( msg,sizeof(msg) );
  cout << "Confirma conexao: " << msg << endl;

  conexao->receive( &dataType,sizeof(DataType) );
  cout << "Tipo de dado: " << tipoDado2string(dataType) << endl << endl;

  switch(dataType)
  {
    case CORES:
      threadCliente.push_back(new thread(bind(&Inputs::ClienteBlobCam,
                                              this,conexao)));
      break;
    case MARCAS:
      threadCliente.push_back(new thread(bind(&Inputs::ClienteMarcaCam,
                                              this,conexao)));
      break;
    default:
      break;
  }
}

string Inputs::tipoDado2string(DataType td)
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

void Inputs::ClienteBlobCam(Inputs *objeto, Connection *conexao)
{
  char msgOK[3] = "OK";

  BlobTratador *tratador = BlobTratador::Instanciar();

  conexao->send(msgOK,sizeof(msgOK));

  Pacote<Marca> pacote;
  while(true)
  {
    conexao->receive( &pacote,sizeof(pacote) );
    tratador->EntregarPacotes(pacote);
    //saidas->
  }
}

void Inputs::ClienteMarcaCam(Inputs *objeto, Connection *conexao)
{
  char msgOK[3] = "OK";

  MarkerProcessor *processor = MarkerProcessor::instantiate();

  conexao->send(msgOK,sizeof(msgOK));

  Pacote<Ente> pacote;
  vector<Ente> resultados;
  while(true)
  {
    conexao->receive( &pacote,sizeof(pacote) );
    processor->EntregarPacotes(pacote);
    processor->localize(resultados);
    objeto->saidas->ReceberEstado(resultados);
  }
}
