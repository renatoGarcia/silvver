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

%module silvver

%header %{
#include "silvverTypes.hpp"
#include "target.hpp"
#include "abstractCamera.hpp"
#include <boost/system/system_error.hpp>
%}

%include "std_string.i"
%include "std_vector.i"

%rename(boostarray) boost::array<double, 9>;

namespace boost
{
  class array<double, 9>
  {};
}

namespace silvver
{
  struct Position
  {
    double x;
    double y;
    double z;

    Position(const double x=0.0, const double y=0.0, const double z=0.0);

    Position(const Position& position);
  };

  struct Pose: public Position
  {
    boost::array<double, 9> rotationMatrix;

    double theta();

    Pose();

    Pose(const Pose& pose);
  };

  template<class BaseClass>
  struct Identity: public BaseClass
  {
    unsigned uid;

    Identity();
    Identity(const BaseClass& base, const unsigned uid);
  };

  template<class TargetType>
  struct CameraReading
  {
    std::string abstractCameraUid;
    uint64_t timestamp;

    std::vector<Identity<TargetType> > localizations;

    CameraReading();
    CameraReading(std::string cameraUid, uint64_t timestamp,
                  std::vector<Identity<TargetType> > localizations);
  };

  template<class T>
  class Target
  {
  public:
    Target(unsigned targetId,
           const std::string& serverIp="127.0.0.1",
           unsigned receptionistPort=12000);

    ~Target() throw();

    void connect();

    void disconnect();

    unsigned getId();

    Identity<T> getLast();

    Identity<T> getNew(const boost::posix_time::time_duration&
                       waitTime = boost::date_time::pos_infin);

    Identity<T> getNext(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);
  };

  template<class T>
  class AbstractCamera
  {
  public:
    AbstractCamera(boost::function<void(CameraReading<T>)> callback,
                   std::string abstractCameraUid,
                   const std::string& serverIp="127.0.0.1",
                   unsigned receptionistPort=12000);

    ~AbstractCamera() throw();

    void connect();

    void disconnect();

    std::string getId();
  };
} //silvver namespace
