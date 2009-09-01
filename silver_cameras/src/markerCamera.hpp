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

#ifndef MARKER_CAMERA_HPP
#define MARKER_CAMERA_HPP

#include <vector>
#include <string>

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "abstractCamera.hpp"
#include "connection.hpp"
#include "markerExtractor.hpp"

/// Implements an abstract camerata which look for artp markers
class MarkerCamera : public AbstractCamera
{
public:

  MarkerCamera(const std::vector<scene::Target>& vecTargets,
               const scene::Camera& cameraConfig,
               boost::shared_ptr<Connection> connection);

  ~MarkerCamera();

  virtual void operator()();

private:

  const boost::scoped_ptr<MarkerExtractor> markerExtractor;
};

#endif
