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
#include <iostream>
#include <sstream>
#include <string>
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
  struct AbstractCameraUid
    :private boost::totally_ordered<AbstractCameraUid>
  {
    unsigned targetSystem;
    unsigned hardCamera;

    AbstractCameraUid();

    AbstractCameraUid(const unsigned targetSystem, const unsigned hardCamera);

    AbstractCameraUid(const AbstractCameraUid& uid);

    bool operator<(const AbstractCameraUid& uid) const;

    bool operator==(const AbstractCameraUid& uid) const;
  };

  struct TargetUid
    :private boost::totally_ordered<TargetUid>
  {
    unsigned targetSystem;
    unsigned internal;

    TargetUid();

    TargetUid(const unsigned targetSystem, const unsigned internal);

    TargetUid(const TargetUid& uid);

    bool operator<(const TargetUid& uid) const;

    bool operator==(const TargetUid& uid) const;
  };

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
    TargetUid uid;

    Identity();
    Identity(const BaseClass& base, const TargetUid& uid);
  };

  class Image
  {
  public:
    Image();

    Image(const Image& image);

    int  width;
    int  height;
  };

  template<class TargetType>
  struct CameraReading
  {
    AbstractCameraUid camUid;
    uint64_t timestamp;
    Image image;
    std::vector<Identity<TargetType> > localizations;

    CameraReading();
    CameraReading(const AbstractCameraUid& camUid,
                  uint64_t timestamp,
                  const Image& image,
                  std::vector<Identity<TargetType> > localizations);
  };

  template<class T>
  class Target
  {
  public:
    Target(const TargetUid& targetUid,
           const std::string& serverName="localhost",
           const std::string& receptionistPort="12000");

    ~Target() throw();

    TargetUid getUid();

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
                   const AbstractCameraUid& abstractCameraUid,
                   const std::string& serverName="localhost",
                   const std::string& receptionistPort="12000");

    ~AbstractCamera() throw();

    AbstractCameraUid getUid();
  };
} //silvver namespace
