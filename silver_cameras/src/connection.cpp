#include "connection.ipp"
#include <iostream>
#include <string>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/serialization/variant.hpp>
#include <vector>
#include <request.hpp>

namespace bip = boost::asio::ip;

boost::asio::io_service Connection::ioService;
boost::scoped_ptr<boost::thread> Connection::th;
boost::once_flag Connection::onceFlag = BOOST_ONCE_INIT;

void
Connection::runIoService()
{
  Connection::ioService.run();
}

Connection::Connection(const std::string& serverIp, unsigned receptionistPort)
  :receptionistSocket(Connection::ioService)
  ,receptionistEP(bip::address::from_string(serverIp), receptionistPort)
  ,outputSocket(Connection::ioService, bip::udp::endpoint())
{
  boost::call_once(Connection::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &Connection::th,
                               new boost::thread(Connection::runIoService)));
}

Connection::~Connection()
{
  if(this->outputSocket.is_open())
  {
    try
    {
      this->disconnect();
    }
    catch(...)
    {
      std::cerr << "Error on closing the connection with server" << std::endl;
    }
  }
}

void
Connection::connect(const std::string& targetType)
{
  this->receptionistSocket.connect(this->receptionistEP);

  Request request = AddCamera(targetType,
                              this->outputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  unsigned short remotePort;
  this->readFromReceptionist(remotePort);

  this->outputSocket.connect(bip::udp::endpoint(this->receptionistEP.address(),
                                                remotePort));

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}

void
Connection::disconnect()
{
  this->receptionistSocket.connect(this->receptionistEP);

  Request request = DelCamera(this->outputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  this->outputSocket.shutdown(bip::udp::socket::shutdown_both);
  this->outputSocket.close();

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}
