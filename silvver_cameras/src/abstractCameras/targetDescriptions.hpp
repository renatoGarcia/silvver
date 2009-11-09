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

#include <boost/preprocessor/seq/for_each_i.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>

// This is the sequence which contain all target description structs.
// It will be expanded in places where a list of all targets types is required.
#define ALL_TARGETS_SEQ (ArtkpTargets)

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

  // This macro will expand to a list of all structs in ALL_TARGETS_SEQ.
  // Except in the first struct there will be a comma before each of.
#define ANY_target_macro(r, data, i, elem) BOOST_PP_COMMA_IF(i) elem

  /** The AnyTarget is a type which can handle any of target structs listed in
   * ALL_TARGETS_SEQ. */
  typedef boost::variant<BOOST_PP_SEQ_FOR_EACH_I(ANY_target_macro,
                                                 _,
                                                 ALL_TARGETS_SEQ) > AnyTarget;
}

#undef ANY_target_macro

#endif /* _TARGET_DESCRIPTIONS_HPP_ */
