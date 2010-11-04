/* Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef _BUTTERFLY_CAMERA_HPP_
#define _BUTTERFLY_CAMERA_HPP_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <butterfly.h>

#include "abstractCamera.hpp"
#include "mountedTarget.hpp"
#include "targetDescriptions.hpp"

class ButterflyCamera
  :public AbstractCamera
  ,private MountedTarget
{
public:
  ButterflyCamera(const scene::AnyHardCamera& anyHardCamera,
                  const scene::ButterflyTargets& confButterflies);

  ~ButterflyCamera();

  virtual void run();

  virtual void stop();

private:

  void doWork();

  static ButterflyInstance*
  createLibButterfly(const scene::anyHardCamera& anyHardCamera,
                     const scene::ButterflyTargets& confButterflies);

  const unsigned maxButterflies;

  ButterflyInstance* const libButterfly;

  boost::scoped_ptr<boost::thread> runThread;
};

#endif /* _BUTTERFLY_CAMERA_HPP_ */
