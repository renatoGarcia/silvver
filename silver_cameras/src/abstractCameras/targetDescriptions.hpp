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

/** \file targetDescriptions.hpp
 * \brief This file contain all structs with the configuration of each
 *        target class. The data described here will be read from the lua
 * config file, and passed to related camera class to proper initialization.
 * In this structs be put all data read from lua config file.
 */

#ifndef _TARGET_DESCRIPTIONS_HPP_
#define _TARGET_DESCRIPTIONS_HPP_

#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>

namespace scene
{
  /// Struct with definitions of ARToolKitPlus targets.
  struct ArtkpTargets
  {
    /// ARToolKitPlus patter width [mm].
    int patternWidth;

    /// Each tuple is a pair of: the unique identifier that silvver will
    /// attribute to this pattern, and the path to the file describing
    /// the pattern.
    std::vector< boost::tuple<unsigned, std::string> > patterns;
  };

  /// boost::variant with all structs of target configurations.
  typedef boost::variant<ArtkpTargets> AnyTarget;
}
#endif /* _TARGET_DESCRIPTIONS_HPP_ */
