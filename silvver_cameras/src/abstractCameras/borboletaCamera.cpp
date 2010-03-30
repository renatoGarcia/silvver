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

#include "borboletaCamera.hpp"

#include <cstddef>
#include <opencv/cv.h>

BorboletaCamera::BorboletaCamera(const scene::Camera& cameraConfig,
                                 const scene::BorboletaTargets& confBorboletas,
                                 boost::shared_ptr<Connection> connection)
  :AbstractCamera(cameraConfig, connection)
  ,nBorboletas(confBorboletas.nBorboletas)
  ,runThread()
{
  init_borboletas(NULL, NULL, confBorboletas.squareSize,
                  cvSize(this->currentFrame.size().width,
                         this->currentFrame.size().height));
}

BorboletaCamera::~BorboletaCamera()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }
}

void
BorboletaCamera::run()
{
  this->runThread.reset(new boost::thread(&BorboletaCamera::doWork, this));
}

void
BorboletaCamera::stop()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }
}

void
BorboletaCamera::doWork()
{
  Borboleta* borboletas = new Borboleta[this->nBorboletas];
  unsigned foundBorboletas = 0;

  while(true)
  {
    boost::this_thread::interruption_point();

    this->updateFrame();

    foundBorboletas = find_borboletas(this->currentFrame,
                                      borboletas, this->nBorboletas);
  }

  delete[] borboletas;
}
