#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>

#include <silverTypes.hpp>
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

#include "scene.hpp"

class Connection
{
public:

  Connection(const std::string& serverIp, unsigned receptionistPort);

  ~Connection();

  void connect(const std::string& targetType);

  void disconnect();

  template <typename T>
  void send(const T& t);

private:

  friend class boost::thread;

  static const unsigned HEADER_LENGTH = 8;

  static boost::asio::io_service ioService;

  static boost::scoped_ptr<boost::thread> th;

  static boost::once_flag onceFlag;

//   static void makeNewThread();

  static void runIoService();

  boost::asio::ip::tcp::socket receptionistSocket;

  const boost::asio::ip::tcp::endpoint receptionistEP;

  boost::asio::ip::udp::socket outputSocket;

//   char inboundHeader[HEADER_LENGTH];

//   std::vector<char> inboundData;

  template <typename T>
  void writeToReceptionist(const T& t);

  template <typename T>
  void readFromReceptionist(T& t);
};

#endif // CONNECTION_HPP
