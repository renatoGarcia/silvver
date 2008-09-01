#ifndef INPUT_HPP
#define INPUT_HPP

#include "inputInterface.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "connection.hpp"
#include "processor.hpp"
#include "processorFactory.hpp"

template <typename Type>
class Input : public InputInterface
{
public:

  Input(boost::shared_ptr<Connection> connection,
        ProcessorType processorType);

  virtual ~Input();

  virtual void confirmConnect();

  virtual void run();

  void operator()();

private:

  boost::shared_ptr<Connection> connection;

  bool stopping;

  boost::scoped_ptr<boost::thread> runThread;

  unsigned connectionPort;

  boost::shared_ptr< ProcessorInterface<Type> > processor;
};

#endif
