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

#ifndef _CF_READER_HPP_
#define _CF_READER_HPP_

#include <boost/shared_ptr.hpp>
#include <string>

#include "luaParser.hpp"
#include "scene.hpp"

class CfReader
{
public:
  CfReader(const std::string& filePath);

  ~CfReader();

  scene::Scene readConfigFile();

private:

  void readHardCamera();

  void readHardware(scene::Hardware& hardware);
  scene::PseudoCamera readPseudoCameraConfig();
  scene::DC1394 readDC1394Config();
  scene::V4l2 readV4l2Config();

  void readTargetSet();
  scene::ArtkpTargets readArtkp();
  void readArtkpTargets(boost::shared_ptr<scene::ArtkpTargets> artkpTargets);
  scene::ButterflyTargets readButterfly();

  LuaParser luaParser;

  scene::Scene sc;
};

#endif /* _CF_READER_HPP_ */
