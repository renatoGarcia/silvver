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
boost::asio::io_service::work Connection::work(Connection::ioService);

void
Connection::runIoService()
{
  Connection::ioService.run();
}

Connection::Connection(const std::string& serverIp, unsigned receptionistPort)
  :receptionistSocket(Connection::ioService)
  ,receptionistEP(bip::address::from_string(serverIp), receptionistPort)
  ,inputSocket(Connection::ioService, bip::udp::endpoint())
  ,inboundData(UPD_MAX_LENGTH)
{
  boost::call_once(Connection::onceFlag,
                   boost::bind(&boost::scoped_ptr<boost::thread>::reset,
                               &Connection::th,
                               new boost::thread(Connection::runIoService)));
}

Connection::~Connection()
{
  if(this->inputSocket.is_open())
  {
    try
    {
      std::cerr << "Destructing connection without call disconet"
                << " before."
                << std::endl;
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
  this->receptionistSocket.connect(this->receptionistEP);

  Request request = DelOutput(targetId,
                              this->inputSocket.local_endpoint().port());
  this->writeToReceptionist(request);

  this->inputSocket.shutdown(bip::udp::socket::shutdown_both);
  this->inputSocket.close();

  this->receptionistSocket.shutdown(bip::tcp::socket::shutdown_both);
  this->receptionistSocket.close();
}
