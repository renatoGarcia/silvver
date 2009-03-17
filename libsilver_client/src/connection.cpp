#include "connection.ipp"

#include <iostream>

#include "request.hpp"

namespace bip = boost::asio::ip;

boost::asio::io_service Connection::ioService;
boost::scoped_ptr<boost::thread> Connection::th;
boost::asio::io_service::work Connection::work(Connection::ioService);

void
Connection::runIoService()
{
  try
  {
    Connection::ioService.run();
  }
  // This system_error is threw when closing the udp socket, and there are
  // any asynchronous send, receive or connect operations yet.
  catch(boost::system::system_error& e)
  {}
}

Connection::Connection(const std::string& serverIp, unsigned receptionistPort)
  :receptionistSocket(Connection::ioService)
  ,receptionistEP(bip::address::from_string(serverIp), receptionistPort)
  ,inputSocket(Connection::ioService)
  ,inboundData(UPD_MAX_LENGTH)
{}

Connection::~Connection()
{
  if(this->inputSocket.is_open())
  {
    try
    {
      this->inputSocket.shutdown(bip::udp::socket::shutdown_both);
      this->inputSocket.close();
    }
    catch(...)
    {}
  }
}

void
Connection::connect(unsigned targetId)
{
  this->th.reset(new boost::thread(Connection::runIoService));

  this->inputSocket.open(bip::udp::v4());
  this->inputSocket.bind(bip::udp::endpoint());

  this->receptionistSocket.connect(this->receptionistEP);

  Request request = AddOutput(targetId,
                              this->inputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  unsigned short remotePort;
  this->readFromReceptionist(remotePort);

  this->inputSocket.connect(bip::udp::endpoint(this->receptionistEP.address(),
                                               remotePort));

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}

void
Connection::disconnect(unsigned targetId)
{
  Connection::ioService.stop();

  this->receptionistSocket.connect(this->receptionistEP);

  Request request = DelOutput(targetId,
                              this->inputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  this->inputSocket.shutdown(bip::udp::socket::shutdown_both);
  this->inputSocket.close();

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}
