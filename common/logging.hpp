#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <ostream>

template<class CharT, class Traits>
class basic_logging
{
public:
  basic_logging(std::basic_ostream<CharT, Traits>& os, unsigned threshold)
    :os(&os)
    ,threshold(threshold)
  {}

  ~basic_logging()
  {}

  std::basic_ostream<CharT, Traits>&
  operator()(unsigned level)
  {
    if (level > this->threshold)
    {
      return this->nullStream;
    }
    else
    {
      return *(this->os);
    }
  }


private:

  class NullStream : public std::basic_ostream<CharT, Traits>
  {
    template<class T>
    NullStream& operator<<(T t)
    {
      return *this;
    }
  };

  const unsigned threshold;

  std::basic_ostream<CharT, Traits>* os;

  NullStream nullStream;
};

typedef basic_logging<char, std::char_traits<char> > logging;

#endif // LOGGING_HPP
