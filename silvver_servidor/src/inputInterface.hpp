#ifndef INPUT_INTERFACE_HPP
#define INPUT_INTERFACE_HPP

class InputInterface
{
public:

  virtual void confirmConnect()=0;

  virtual void run()=0;

};

#endif
