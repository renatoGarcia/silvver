#ifndef INPUT_HPP
#define INPUT_HPP

#include "inputInterface.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include "ioConnection.hpp"
#include "processor.hpp"
#include "processorFactory.hpp"

template <typename Type>
class Input : public InputInterface
{
public:

  Input(boost::shared_ptr<IoConnection> connection,
        ProcessorType processorType);

  ~Input();

private:

  std::vector<Type> inputs;

  boost::shared_ptr<IoConnection> connection;

  unsigned connectionPort;

  boost::shared_ptr< ProcessorInterface<Type> > processor;

  void handleReceive();
};

#endif
