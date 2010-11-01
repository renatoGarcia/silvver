#ifndef _CALLBACK_WRAPPER_HPP_
#define _CALLBACK_WRAPPER_HPP_

#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/exceptions.hpp>

#include <silvver/exceptions.hpp>

namespace silvver {

namespace error_code {
enum ErrorCode
{
  success = 0,
  connection_error
};
}
using error_code::ErrorCode;


template <class T>
class CallbackWrapper
{
public:
  typedef boost::function<void (typename T::LocalizationType, ErrorCode)> Handler;

  CallbackWrapper(T* client, Handler handler);

  ~CallbackWrapper();

private:
  void receive();

  boost::scoped_ptr<T> client;

  Handler handler;

  boost::thread receiveThread;
};

template <class T>
CallbackWrapper<T>::CallbackWrapper(T* client, Handler handler)
  :client(client)
  ,handler(handler)
  ,receiveThread(&CallbackWrapper<T>::receive, this)
{}

template <class T>
CallbackWrapper<T>::~CallbackWrapper()
{
  this->receiveThread.interrupt();
  this->receiveThread.join();
}

template <class T>
void
CallbackWrapper<T>::receive()
{
  try
  {
    while (true)
    {
      boost::this_thread::interruption_point();
      this->handler(this->client->getUnseen(), error_code::success);
    }
  }
  catch (const boost::thread_interrupted& e)
  {
    return;
  }
  catch (const connection_error& e)
  {
    this->handler(typename T::LocalizationType(),
                  error_code::connection_error);
    return;
  }
}

} // namespace silvver

#endif /* _CALLBACK_WRAPPER_HPP_ */
