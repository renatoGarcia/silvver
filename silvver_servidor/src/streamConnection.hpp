#ifndef STREAM_CONNECTION_HPP
#define STREAM_CONNECTION_HPP

#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>

class StreamConnection
  : public boost::enable_shared_from_this<StreamConnection>
{
public:

  typedef boost::shared_ptr<StreamConnection> pointer;

  inline static pointer
  create(boost::asio::io_service& ioService)
  {
    return pointer(new StreamConnection(ioService));
  }

  inline boost::asio::ip::tcp::socket&
  getSocket()
  {
    return this->socket;
  }

  inline std::string
  getRemoteIp()
  {
    return this->socket.remote_endpoint().address().to_string();
  }

  /**
   * Asynchronously read a value of type T.
   * @param t Reference to the variable where the read value will be returned.
   * @param handler Function with signature "void handler()" which to be called
   *                when the operation completes.
   */
  template <typename T, typename Handler>
  inline void
  asyncRead(T& t, Handler handler)
  {
    boost::asio::async_read(this->socket,
                            boost::asio::buffer(inboundHeader),
                            boost::bind(&StreamConnection::readHeader<T, Handler>,
                                        shared_from_this(),
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred,
                                        boost::ref(t),
                                        boost::make_tuple(handler)));
  }

  template <typename T>
  void write(const T& t);

private:

  StreamConnection(boost::asio::io_service& ioService)
    :socket(ioService)
  {}

  template <typename T, typename Handler>
  void readHeader(const boost::system::error_code& e,
                  std::size_t bytes_transferred,
                  T& t,
                  boost::tuple<Handler> handler);

  template <typename T, typename Handler>
  void readData(const boost::system::error_code& e,
                std::size_t bytes_transferred,
                T& t,
                boost::tuple<Handler> handler);

  static const unsigned HEADER_LENGTH = 8;

  boost::asio::ip::tcp::socket socket;

  char inboundHeader[HEADER_LENGTH];

  std::vector<char> inboundData;
};

#endif // STREAM_CONNECTION_HPP
