#ifndef STREAM_CONNECTION_IPP
#define STREAM_CONNECTION_IPP

#include "streamConnection.hpp"

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/system/system_error.hpp>

template <typename T>
void
StreamConnection::write(const T& t)
{
  std::string outboundData;
  std::string outboundHeader;

  // Serialize the data first so we know how large it is.
  std::ostringstream archiveStream;
  boost::archive::text_oarchive archive(archiveStream);
  archive << t;
  outboundData = archiveStream.str();

  // Format the header.
  std::ostringstream headerStream;
  headerStream << std::setw(HEADER_LENGTH)
               << std::hex << outboundData.size();
  if (!headerStream || headerStream.str().size() != HEADER_LENGTH)
  {
//     // Something went wrong, inform the caller.
//     boost::system::error_code error(boost::asio::error::invalid_argument);
//     socket_.io_service().post(boost::bind(handler, error));
    return;
  }
  outboundHeader = headerStream.str();

  std::vector<boost::asio::const_buffer> buffers;
  buffers.push_back(boost::asio::buffer(outboundHeader));
  buffers.push_back(boost::asio::buffer(outboundData));

  boost::asio::write(this->socket, buffers);
}

// Handle a completed read of a message header. The handler is passed using
// a tuple since boost::bind seems to have trouble binding a function object
// created using boost::bind as a parameter.
template <typename T, typename Handler>
void
StreamConnection::readHeader(const boost::system::error_code& e,
                             std::size_t bytes_transferred,
                             T& t,
                             boost::tuple<Handler> handler)
{
  if (e)
  {
    throw boost::system::system_error(e);
  }
  else
  {
    // Determine the length of the serialized data.
    std::istringstream is(std::string(this->inboundHeader, HEADER_LENGTH));
    std::size_t inboundDataSize = 0;
    if (!(is >> std::hex >> inboundDataSize))
    {
      throw boost::system::system_error(boost::asio::error::invalid_argument);
    }

    this->inboundData.resize(inboundDataSize);
    boost::asio::async_read
      (this->socket,
       boost::asio::buffer(this->inboundData),
       boost::bind(&StreamConnection::readData<T, Handler>,
                   shared_from_this(),
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred,
                   boost::ref(t),
                   handler));
  }
}

template <typename T, typename Handler>
void
StreamConnection::readData(const boost::system::error_code& e,
                     std::size_t bytes_transferred,
                     T& t,
                     boost::tuple<Handler> handler)
{
  if (e)
  {
    throw boost::system::system_error(e);
  }
  else
  {
    // Extract the data structure from the data just received.
    try
    {
      std::string archiveData(&this->inboundData[0], this->inboundData.size());
      std::istringstream archiveStream(archiveData);
      boost::archive::text_iarchive archive(archiveStream);
      archive >> t;
    }
    catch (std::exception& e)
    {
      // Unable to decode data.
      throw boost::system::system_error(boost::asio::error::invalid_argument);
    }

    // Inform caller that data has been received ok.
    boost::get<0>(handler)();
  }
}

#endif // STREAM_CONNECTION_IPP

