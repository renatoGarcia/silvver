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

#include "../connection.hpp"
#include "abstractCamera.hpp"
#include "targetDescriptions.hpp"

class ButterflyCamera: public AbstractCamera
{
public:
  ButterflyCamera(const scene::Camera& cameraConfig,
                  const scene::ButterflyTargets& confButterflies,
                  boost::shared_ptr<Connection> connection);

  ~ButterflyCamera();

  virtual void run();

  virtual void stop();

private:

  void doWork();

  const unsigned nButterflies;

  boost::scoped_ptr<boost::thread> runThread;
};

#endif /* _BUTTERFLY_CAMERA_HPP_ */
