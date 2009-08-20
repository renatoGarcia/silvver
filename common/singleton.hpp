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

#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

#include <boost/utility.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>

template<class T>
class Singleton : private boost::noncopyable
{
public:

  static boost::shared_ptr<T> instantiate()
  {
    boost::call_once(init, Singleton::onceFlag);
    return t;
  }

  static void init()
  {
    Singleton::t.reset(new T());
  }

protected:

  Singleton()
  {}

  virtual ~Singleton()
  {}

private:

  static boost::shared_ptr<T> t;
  static boost::once_flag onceFlag;

};

template<class T> boost::shared_ptr<T> Singleton<T>::t;
template<class T> boost::once_flag Singleton<T>::onceFlag = BOOST_ONCE_INIT;


#endif /* _SINGLETON_HPP_ */
