#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <string>
#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/once.hpp>
#include <boost/ref.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

class Connection
{
public:

  Connection(const std::string& serverIp, unsigned receptionistPort);

  ~Connection();

  void connect(unsigned targetId);

  void disconnect(unsigned targetId);

  /** Asynchronously read a value of type T.
   * @param t Reference to the variable where the read value will be returned.
   * @param handler Function with signature "void handler()" which to be called
   *                when the operation completes.
   */
  template <typename T, typename Handler>
  inline void
  asyncRead(T& t, Handler handler)
  {
    this->inputSocket.async_receive
      (boost::asio::buffer(this->inboundData),
       boost::bind(&Connection::readData<T, Handler>,
                   this,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred,
                   boost::ref(t),
                   boost::make_tuple(handler)));
  }

private:

  friend class boost::thread;

  /// Length of TCP message header.
  static const unsigned HEADER_LENGTH = 8;

  /// Max length of UPD messages.
  static const unsigned UPD_MAX_LENGTH = 8192;

  static boost::asio::io_service ioService;
  static boost::asio::io_service::work work;

  static boost::scoped_ptr<boost::thread> th;

  static boost::once_flag onceFlag;

  static void runIoService();

  boost::asio::ip::tcp::socket receptionistSocket;

  const boost::asio::ip::tcp::endpoint receptionistEP;

  boost::asio::ip::udp::socket inputSocket;

  std::vector<char> inboundData;

  template <typename T>
  void writeToReceptionist(const T& t);

  template <typename T>
  void readFromReceptionist(T& t);

  template <typename T, typename Handler>
  void readData(const boost::system::error_code& e,
                std::size_t bytes_transferred,
                T& t,
                boost::tuple<Handler> handler);
};

#endif // CONNECTION_HPP
