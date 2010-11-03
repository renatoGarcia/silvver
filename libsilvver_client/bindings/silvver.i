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
#include "abstractCamera.hpp"
#include "target.hpp"
#include "targetSet.hpp"
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

namespace silvver{

typedef unsigned TargetSetUid;

struct AbstractCameraUid
{
  TargetSetUid targetSet;
  unsigned hardCamera;

  AbstractCameraUid();

  AbstractCameraUid(const TargetSetUid targetSet, const unsigned hardCamera);

  AbstractCameraUid(const AbstractCameraUid& uid);

  bool operator<(const AbstractCameraUid& uid) const;

  bool operator==(const AbstractCameraUid& uid) const;
};

struct TargetUid
{
  TargetSetUid targetSet;
  unsigned internal;

  TargetUid();

  TargetUid(const TargetSetUid targetSet, const unsigned internal);

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

struct Pose
  :public Position
{
  boost::array<double, 9> rotationMatrix;

  double theta();

  Pose();

  Pose(const Pose& pose);
};

template<class BaseClass>
struct Identity
  :public BaseClass
{
  TargetUid uid;

  Identity();
  Identity(const BaseClass& base, const TargetUid& uid);
};

template<class TargetType>
struct CameraReading
{
  AbstractCameraUid uid;
  uint64_t timestamp;
  std::vector<Identity<TargetType> > localizations;

  CameraReading();
  CameraReading(const AbstractCameraUid& uid,
                uint64_t timestamp,
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

  TargetUid getUid() const;

  Identity<T> getLast();

  Identity<T> getUnseen(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);

  Identity<T> getNext(const boost::posix_time::time_duration&
                      waitTime = boost::date_time::pos_infin);

  void exitWait();
};

template<class T>
class AbstractCamera
{
public:
  AbstractCamera(const AbstractCameraUid& abstractCameraUid,
                 const std::string& serverName="localhost",
                 const std::string& receptionistPort="12000");

  ~AbstractCamera() throw();

  AbstractCameraUid getUid() const;

  CameraReading<T> getLast();

  CameraReading<T> getUnseen(const boost::posix_time::time_duration&
                             waitTime = boost::date_time::pos_infin);

  CameraReading<T> getNext(const boost::posix_time::time_duration&
                           waitTime = boost::date_time::pos_infin);

  void exitWait();
};

template<class T>
class TargetSet
{
public:
  TargetSet(const TargetSetUid& targetSetUid,
            const std::string& serverName="localhost",
            const std::string& receptionistPort="12000");

  ~TargetSet() throw();

  TargetSetUid getUid() const;

  std::vector<Identity<T> > getLast();

  std::vector<Identity<T> > getUnseen(const boost::posix_time::time_duration&
                                      waitTime = boost::date_time::pos_infin);

  std::vector<Identity<T> > getNext(const boost::posix_time::time_duration&
                                    waitTime = boost::date_time::pos_infin);

  void exitWait();
};

} //silvver namespace
