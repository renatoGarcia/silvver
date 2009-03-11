#ifndef TARGET_HPP
#define TARGET_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include "silverTypes.hpp"

/// Reprents a target looked by Silver.
class Target
{
public:

  class old_pose_error : public std::runtime_error
  {
  public:
    old_pose_error(const std::string& whatArg)
      :runtime_error(whatArg){}
  };


  /** Target class constructor.
   * @param targetId Target identifier.
   * @param log True to save the received poses in a text file.
   * @param serverIp IP address of silver-server.
   * @param receptionistPort Port number of silver-server receptionist.
   */
  Target(unsigned targetId,
         bool log=false,
         const std::string& serverIp="127.0.0.1",
         unsigned receptionistPort=12000);

  ~Target() throw();

  /// Connect to the silver-server.
  void connect();

  void disconnect();

  /** Get the id of target.
   * @return The id of this target.
   */
  unsigned getId();

  /** Get the last received pose.
   * @param x X coordinate [m].
   * @param y Y coordinate [m].
   * @param theta Angle of pose [rad].
   */
  void getLastPose(double &x, double &y, double &theta);

  /// Get the last received silver::Pose.
  silver::Pose getLastPose();

  /** Get a never gotten pose or throw an old_pose_error exception.
   * @param x X coordinate [m].
   * @param y Y coordinate [m].
   * @param theta Angle of pose [rad].
   */
  void getNewPose(double &x, double &y, double &theta);

  /// Get a never gotten pose or throw an old_pose_error exception.
  silver::Pose getNewPose();

  /** Get the next received pose.
   * This function wait until a new Pose arrives and returns;
   * @param x X coordinate [m].
   * @param y Y coordinate [m].
   * @param theta Angle of pose [rad].
   */
  void getNextPose(double &x, double &y, double &theta);

  /** Return the next received silver::Pose.
   * This function wait until a new Pose arrives and returns;
   */
  silver::Pose getNextPose();

private:

  class CheshireCat;
  std::auto_ptr<CheshireCat> smile;
};

#endif // TARGET_HPP
