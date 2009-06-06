#ifndef TS_OSTREAM_HPP
#define TS_OSTREAM_HPP

#include <boost/thread/mutex.hpp>
#include <ostream>

template<class CharT, class Traits>
class basic_tsostream : public std::basic_ostream<CharT, Traits>
{
public:

  basic_tsostream(std::basic_streambuf<CharT, Traits>* sb)
    :std::basic_ostream<CharT, Traits>(sb)
  {}

  ~basic_tsostream()
  {}

  void lock()
  {
    this->outputMutex.lock();
  }

  void unlock()
  {
    this->outputMutex.unlock();
  }

private:

  boost::mutex outputMutex;
};

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
lock(std::basic_ostream<CharT, Traits>& os)
{
  basic_tsostream<CharT, Traits>* tsos =
    dynamic_cast<basic_tsostream<CharT,Traits>*>(&os);

  if(tsos)
  {
    tsos->lock();
  }
  return os;
}

template<class CharT, class Traits>
inline std::basic_ostream<CharT, Traits>&
unlock(std::basic_ostream<CharT, Traits>& os)
{
  basic_tsostream<CharT, Traits>* tsos =
    dynamic_cast<basic_tsostream<CharT,Traits>*>(&os);

  if(tsos)
  {
    tsos->unlock();
  }
  return os;
}

typedef basic_tsostream<char, std::char_traits<char> > tsostream;

#endif // TS_OSTREAM_HPP
