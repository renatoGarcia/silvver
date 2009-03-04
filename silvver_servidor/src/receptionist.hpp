#ifndef RECEPTIONIST_HPP
#define RECEPTIONIST_HPP

#include <boost/thread/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include "inputInterface.hpp"
#include "clientsMap.hpp"
#include "streamConnection.hpp"
#include <map>
#include <boost/asio.hpp>
#include <request.hpp>

extern bool verbose;

/** Cuida dos pedidos de conexão com o silvver-servidor.
 * Esta é a classe central de silvver servidor. Ela cuida de todos os pedidos de conexão,
 * cria um objeto Conexao permanente, e o redistribui para a classe apropriada.
 */
class Receptionist
{
public:
  Receptionist(unsigned localPort);

  ~Receptionist();

  void run();

  void operator()();

  // Necessary for boost::apply_visitor function to receive a Receptionist
  // object as an argument. With this typedef, the Receptionist class fulfills
  // the requirements of a "static visitor" boost::variant concept.
  typedef void result_type;

  // Visitors of the boost::variant Request type.
  void operator()(NullRequest& request) const;
  void operator()(AddOutput& request) const;
  void operator()(DelOutput& request) const;
  void operator()(AddCamera& request);
  void operator()(DelCamera& request);

private:

  static boost::asio::io_service ioService;

  boost::asio::ip::tcp::acceptor acceptor;

  StreamConnection::pointer currentReception;
  Request request;

  void handleAccept(StreamConnection::pointer connection);

  void handleRead();

  std::map<unsigned, boost::shared_ptr<InputInterface> > mapInputs;

  boost::shared_ptr<ClientsMap> outputs;

  /// Thread onde onde será executado o método Recepcionista.
  boost::scoped_ptr<boost::thread> thReceptionist;
};

#endif
