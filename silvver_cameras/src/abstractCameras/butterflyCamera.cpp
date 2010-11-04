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
#include <vector>

#include <opencv/cv.h>

#include "../exceptions.hpp"

ButterflyCamera::ButterflyCamera(const scene::AnyHardCamera& anyHardCamera,
                                 const scene::ButterflyTargets& confButterflies)
  :AbstractCamera(anyHardCamera, confButterflies, procOpt::Marker())
  ,maxButterflies(confButterflies.maxButterflies)
  ,libButterfly(ButterflyCamera::createLibButterfly(anyHardCamera,
                                                    confButterflies))
  ,runThread()
{
  if (this->libButterfly == NULL)
  {
    throw init_target_error()
      << info_what("Failed when creating struct ButterflyInstance.");
  }
}

ButterflyCamera::~ButterflyCamera()
{
  if (this->runThread)
  {
    this->runThread->interrupt();
    this->runThread->join();
  }

  if (this->libButterfly)
    butterfly_delete(this->libButterfly);
}

ButterflyInstance*
ButterflyCamera::createLibButterfly(const scene::AnyHardCamera& anyHardCamera,
                                    const scene::ButterflyTargets& confButterflies)
{
  const scene::HardCamera hardCamera =
    boost::apply_visitor(scene::GetHardCamera(), anyHardCamera);

  Butterfly_CamParams camParam;
  camParam.image_size = cvSize(hardCamera.resolution.at(0),
                               hardCamera.resolution.at(1));
  camParam.focal_length[0] = hardCamera.focalLength.at(0);
  camParam.focal_length[1] = hardCamera.focalLength.at(1);
  camParam.principal_point[0] = hardCamera.principalPoint.at(0);
  camParam.principal_point[1] = hardCamera.principalPoint.at(1);
  camParam.radial_coef[0] = hardCamera.radialCoef.at(0);
  camParam.radial_coef[1] = hardCamera.radialCoef.at(1);
  camParam.radial_coef[2] = hardCamera.radialCoef.at(2);
  camParam.tangential_coef[0] = hardCamera.tangentialCoef.at(0);
  camParam.tangential_coef[1] = hardCamera.tangentialCoef.at(1);

  return butterfly_construct(confButterflies.squareSize, camParam,
                             confButterflies.maxButterflies, 0);
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
  std::vector<silvver::Identity<silvver::Pose> > poses;
  silvver::Identity<silvver::Pose> tmpPose;

  std::vector<Butterfly> butterflies(this->maxButterflies);
  unsigned foundButterflies = 0;

  while(true)
  {
    boost::this_thread::interruption_point();

    poses.clear();
    this->updateFrame();

    foundButterflies = find_butterflies(this->libButterfly,
                                        this->currentFrame.image,
                                        &butterflies.front());

    for (int k = 0; k < foundButterflies; ++k)
    {
      tmpPose.uid = silvver::TargetUid(this->abstractCameraUid.targetSystem,
                                       butterflies[k].uid);
      tmpPose.x = butterflies[k].x;
      tmpPose.y = butterflies[k].y;
      tmpPose.z = butterflies[k].z;
      for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j)
          tmpPose.rotationMatrix[(3*i)+j] = butterflies[k].rot_matrix[i][j];

      localizeBody(tmpPose);
      toWorld(tmpPose);

      poses.push_back(tmpPose);
    }

    sendLocalizations(poses);
  }
}
