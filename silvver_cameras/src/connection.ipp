#ifndef CONNECTION_IPP
#define CONNECTION_IPP

#include "connection.hpp"
#include <boost/serialization/vector.hpp>
#include <serializations.hpp>

namespace bip = boost::asio::ip;

template <typename T>
void
Connection::send(const T& t)
{
  std::string outboundData;

  std::ostringstream archiveStream;
  boost::archive::text_oarchive archive(archiveStream);
  archive << t;
  outboundData = archiveStream.str();

  this->outputSocket.send(boost::asio::buffer(outboundData));
}


template <typename T>
void
Connection::writeToReceptionist(const T& t)
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

  boost::asio::write(this->receptionistSocket, buffers);
}

template <typename T>
void
Connection::readFromReceptionist(T& t)
{
  char inHeader[HEADER_LENGTH];
  boost::asio::read(this->receptionistSocket, boost::asio::buffer(inHeader));

  // Determine the length of the serialized data.
  std::istringstream is(std::string(inHeader, HEADER_LENGTH));
  std::size_t inboundDataSize = 0;
  if (!(is >> std::hex >> inboundDataSize))
  {
    throw boost::system::system_error(boost::asio::error::invalid_argument);
  }

  std::vector<char> inData(inboundDataSize);
  boost::asio::read(this->receptionistSocket, boost::asio::buffer(inData));

  // Extract the data structure from the data just received.
  try
  {
    std::string archiveData(&inData[0], inData.size());
    std::istringstream archiveStream(archiveData);
    boost::archive::text_iarchive archive(archiveStream);
    archive >> t;
  }
  catch (std::exception& e)
  {
    // Unable to decode data.
    throw boost::system::system_error(boost::asio::error::invalid_argument);
  }
}

#endif // CONNECTION_IPP
