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

#include "timestampMap.hpp"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

#include "../exceptions.hpp"

TSMapWriter::TSMapWriter()
  :tsMap()
{}

TSMapWriter::~TSMapWriter()
{}

void
TSMapWriter::add(const std::string& key, unsigned value)
{
  this->tsMap.push_back(std::make_pair(key, value));
}

void
TSMapWriter::save(const std::string& fileName)
{
  std::ofstream file(fileName.c_str());

  BOOST_FOREACH(Item& item, this->tsMap)
  {
    file << item.first << '=' << item.second << '\n';
  }
}

TSMapReader::TSMapReader(const std::string& fileName)
{
  // Used to optimize item insertion on map
  std::map<std::string, unsigned>::iterator lastItem = this->tsMap.begin();

  std::ifstream file(fileName.c_str());
  std::string line, image;
  unsigned timestamp;
  std::size_t endImage, startTimestamp;
  while (std::getline(file, line))
  {
    endImage = line.rfind('=');
    startTimestamp = endImage + 1;

    image = line.substr(0, endImage);
    timestamp = boost::lexical_cast<unsigned>(line.substr(startTimestamp));

    this->tsMap.insert(lastItem, std::make_pair(image, timestamp));
    ++lastItem;
  }
}

TSMapReader::~TSMapReader()
{}

unsigned
TSMapReader::read(const std::string& key)
{
  std::map<std::string, unsigned>::iterator item = this->tsMap.find(key);
  if (item == this->tsMap.end()) //If key was not found
  {
    throw file_parsing_error()
      << info_what("Error on parsing timestamps file")
      << info_fieldName(key);
  }

  return item->second;
}
