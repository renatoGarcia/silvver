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

#include <boost/array.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace scene
{
  struct Target
  {
    /// Key unique to each targetSet.
    unsigned targetSetUid;
  };

  /// Information about targets that is above (mounted) in a body (eg: a robot)
  struct Mounted
  {
    /// Translation of body center in target coordinate system.
    boost::array<double,3> bodyTranslation;

    /// Rotation matrix of body in target coordinate system.
    boost::array<double,9> bodyRotation;
  };

  /// Struct with definitions of ARToolKitPlus targets.
  struct ArtkpTargets
    :public Target
    ,public Mounted
  {
    /// ARToolKitPlus pattern width [mm].
    int patternWidth;

    /// Grey level threshold between black and white.
    int threshold;

    /// Each tuple is: the unique identifier that silvver will attribute to
    /// this pattern, and the path to the file describing the pattern.
    std::vector<boost::tuple<unsigned, std::string> > patterns;
  };

  struct ButterflyTargets
    :public Target
    ,public Mounted
  {
    // Size of one checkerboard square [mm]
    unsigned squareSize;

    // Max number of butterflys
    unsigned maxButterflies;
  };

  /// The AnyTarget is a type which can handle any of target structs.
  typedef boost::variant<ArtkpTargets,
                         ButterflyTargets> AnyTarget;
}

#endif /* _TARGET_DESCRIPTIONS_HPP_ */
