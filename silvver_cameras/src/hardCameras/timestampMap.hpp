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

#ifndef _TIMESTAMP_MAP_HPP_
#define _TIMESTAMP_MAP_HPP_

#include <map>
#include <string>
#include <utility>
#include <vector>

class TSMapWriter
{
public:
  TSMapWriter();

  ~TSMapWriter();

  void add(const std::string& key, unsigned value);

  void save(const std::string& fileName);

private:
  typedef std::pair<std::string, unsigned> Item;

private:
  std::vector<Item> tsMap;
};

class TSMapReader
{
public:
  TSMapReader(const std::string& fileName);

  ~TSMapReader();

  unsigned read(const std::string& key);

private:
  std::map<std::string, unsigned> tsMap;
};
#endif /* _TIMESTAMP_MAP_HPP_ */
