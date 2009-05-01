#include "receptionist.hpp"
#include "streamConnection.ipp"
#include "ioConnection.ipp"
#include <boost/ref.hpp>
#include <iostream>
#include "inputFactory.hpp"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

extern bool verbose;
#define VERBOSE_PRINT(msg) if(verbose)std::cout<<msg;

namespace ba = boost::asio;
namespace bip = boost::asio::ip;

boost::asio::io_service Receptionist::ioService;

Receptionist::Receptionist(unsigned localPort)
  :acceptor(Receptionist::ioService,
            bip::tcp::endpoint(bip::tcp::v4(), localPort))
{
  this->outputs = ClientsMap::instantiate();
}

Receptionist::~Receptionist()
{
  this->ioService.stop();
  this->thReceptionist->join();
}

void
Receptionist::run()
{
  if(!this->thReceptionist)
  {
    StreamConnection::pointer connection =
      StreamConnection::create(Receptionist::ioService);
    this->acceptor.async_accept(connection->getSocket(),
                                boost::bind(&Receptionist::handleAccept,
                                            this,
                                            connection));

    thReceptionist.reset(new boost::thread(boost::ref(*this)));
  }
}

void
Receptionist::operator()()
{
  Receptionist::ioService.run();
}

void
Receptionist::handleAccept(StreamConnection::pointer connection)
{
  this->currentReception = connection;
  connection->asyncRead(this->request,
                        boost::bind(&Receptionist::handleRead,
                                    this));

  StreamConnection::pointer newConnection =
    StreamConnection::create(Receptionist::ioService);
  this->acceptor.async_accept(newConnection->getSocket(),
                              boost::bind(&Receptionist::handleAccept,
                                          this,
                                          newConnection));
}

void
Receptionist::handleRead()
{
  boost::apply_visitor(*this, this->request);
}

void
Receptionist::operator()(NullRequest& request) const
{}

void
Receptionist::operator()(AddOutput& request) const
{
  std::cout << "AddOutput. id: " << request.targetId << std::endl;
  boost::shared_ptr<IoConnection>
    ioConnection(new IoConnection(this->currentReception->getRemoteIp(),
                                  request.localPort));

  this->currentReception->write(ioConnection->getLocalPort());

  this->outputs->addOutput(request, ioConnection);
}

void
Receptionist::operator()(DelOutput& request) const
{
  std::cout << "DelOutput" << std::endl;
  this->outputs->removeOutput(request.targetId, request.localPort);
}

void
Receptionist::operator()(AddCamera& request)
{
  std::cout << "AddCamera" << std::endl;

  boost::shared_ptr<IoConnection>
    ioConnection(new IoConnection(this->currentReception->getRemoteIp(),
                                  request.localPort));

  this->currentReception->write(ioConnection->getLocalPort());

  boost::shared_ptr<InputInterface> input =
    InputFactory::createInput(request.targetType, ioConnection);

  this->mapInputs.insert(std::pair< unsigned,boost::shared_ptr<InputInterface> >
                         ((unsigned)ioConnection->getRemotePort(), input));
}

void
Receptionist::operator()(DelCamera& request)
{
  std::cout << "DelCamera" << std::endl;
  if (this->mapInputs.erase(request.localPort) != 1)
  {
    std::cerr << "Unknown camera delete request" << std::endl;
  }
}