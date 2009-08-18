/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <ostream>

template<class CharT, class Traits>
class basic_logging
{
public:
  basic_logging(std::basic_ostream<CharT, Traits>& os, unsigned threshold=0)
    :os(&os)
    ,threshold(threshold)
  {}

  ~basic_logging()
  {}

  void
  setThreshold(unsigned threshold)
  {
    this->threshold = threshold;
  }

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

  unsigned threshold;

  std::basic_ostream<CharT, Traits>* os;

  NullStream nullStream;
};

typedef basic_logging<char, std::char_traits<char> > logging;

#endif // LOGGING_HPP
