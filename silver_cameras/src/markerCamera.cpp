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

#include "markerCamera.hpp"

#include "connection.ipp"
#include "silverTypes.hpp"
#include <math.h>

MarkerCamera::MarkerCamera(const std::vector<scene::Target> &vecTargets,
                           const scene::Camera& cameraConfig,
                           boost::shared_ptr<Connection> connection)
  :AbstractCamera(cameraConfig, connection)
  ,markerExtractor(new MarkerExtractor(cameraConfig.resolution.at(0),
                                       cameraConfig.resolution.at(1),
                                       vecTargets))
{}

MarkerCamera::~MarkerCamera()
{}

void
MarkerCamera::operator()()
{
  this->markerExtractor->initialize();

  std::vector<silver::Identity<silver::Pose> > vecEnte;
  std::vector<MarkerExtractor::MarkerPoints> vecMarkerPoints;
  std::vector<MarkerExtractor::MarkerPoints>::iterator iteMarkerPoints;
  double yaw;
  while(!this->stopping)
  {
    vecEnte.clear();
    this->updateFrame();

    this->markerExtractor->extract(this->currentFrame, vecMarkerPoints);

    iteMarkerPoints = vecMarkerPoints.begin();
    for (; iteMarkerPoints<vecMarkerPoints.end(); iteMarkerPoints++)
    {
      this->localize(iteMarkerPoints->center);
      this->localize(iteMarkerPoints->primaryVertex);
      this->localize(iteMarkerPoints->secondaryVertex);

      yaw =
        atan2(iteMarkerPoints->secondaryVertex.y - iteMarkerPoints->primaryVertex.y,
              iteMarkerPoints->secondaryVertex.x - iteMarkerPoints->primaryVertex.x);

      iteMarkerPoints->center.yaw = yaw;
      vecEnte.push_back(iteMarkerPoints->center);
    }

    this->serverConnection->send(vecEnte);
  }
}
