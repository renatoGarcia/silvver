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

#ifndef _PROCESSOR_OPTIONS_HPP_
#define _PROCESSOR_OPTIONS_HPP_

#include <boost/variant/variant.hpp>

#include "silvverTypes.hpp"

namespace procOpt
{
  struct Marker
  {
    typedef silvver::Pose InputType;

    bool operator==(const Marker&) const
    {
      // There is no attributes, then objects will be ever equal.
      return true;
    }

    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version)
    {}
  };

  typedef boost::variant<Marker> AnyProcOpt;
}

#endif /* _PROCESSOR_OPTIONS_HPP_ */
