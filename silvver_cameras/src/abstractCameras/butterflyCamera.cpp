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

#include "butterflyCamera.hpp"

#include <cstddef>
#include <opencv/cv.h>

ButterflyCamera::ButterflyCamera(const scene::Camera& cameraConfig,
                                 const scene::ButterflyTargets& confButterflies,
                                 boost::shared_ptr<Connection> connection)
  :AbstractCamera(cameraConfig, connection)
  ,nButterflies(confButterflies.nButterflies)
  ,runThread()
{
  init_butterfly(NULL, NULL, confButterflies.squareSize,
                 cvSize(this->currentFrame.size().width,
                        this->currentFrame.size().height));
}

ButterflyCamera::~ButterflyCamera()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }
}

void
ButterflyCamera::run()
{
  this->runThread.reset(new boost::thread(&ButterflyCamera::doWork, this));
}

void
ButterflyCamera::stop()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }
}

void
ButterflyCamera::doWork()
{
  Butterfly* butterflies = new Butterfly[this->nButterflies];
  unsigned foundButterflies = 0;

  while(true)
  {
    boost::this_thread::interruption_point();

    this->updateFrame();

    foundButterflies = find_butterflies(this->currentFrame,
                                        butterflies, this->nButterflies);
  }

  delete[] butterflies;
}
