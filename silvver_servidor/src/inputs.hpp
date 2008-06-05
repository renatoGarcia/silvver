#ifndef INPUTS_HPP
#define INPUTS_HPP

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include "connection.hpp"
#include "cameraControllers.hpp"

using namespace silver;

class MarkerCameraControl;

/** Gerencia o conjunto de clientes que fornecem dados de entrada ao silvver-servidor.
 * Para cada tipo de dado de entrada deverá haver um método para tratá-lo.
 * Atualmente todos os clientes que fornecem dados de entrada são câmeras, e os tipos de dados
 * aceitos são a pose de marcos geométricos e a posição de blobs de cor.
 */
class Inputs
{
public:

  enum DataType
  {
    BLOB   = 103,
    MARKER = 104
  };

  Inputs();

  ~Inputs();

  /// Recebe um objeto da classe conexão onde se encontram informações sobre um novo cliente e inicia o tratamento dos dados que serão recebidos.
  void addInput(Connection *connection);

private:

  std::vector< boost::shared_ptr<MarkerCameraController> > markerControllers;

  std::vector< boost::shared_ptr<BlobCameraController> > blobControllers;

  std::string tipoDado2string(DataType td);

};

#endif
