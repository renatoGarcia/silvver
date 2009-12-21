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

#ifndef _OBSERVER_HPP_
#define _OBSERVER_HPP_

#include <list>

/** Base class to observing classes.
 * The classes which inherit from this class MUST call the detach method of
 * its respective Subject object befere its destruction.
 */
class Observer
{
public:
  virtual void update() = 0;

  virtual ~Observer()
  {}

protected:
  Observer()
  {}
};

/** Base class to observable classes.
 * This class holds raw pointers to Observer objects. The Observer objects
 * has the responsibility of call detach method before its destruction.
 */
class Subject
{
public:
  virtual ~Subject()
  {}

  void attach(Observer* observer);

  void detach(Observer* observer);

protected:
  Subject()
  {}

  virtual void notify();

private:
  std::list<Observer*> observers;
};

#endif /* _OBSERVER_HPP_ */
