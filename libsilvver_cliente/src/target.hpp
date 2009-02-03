#ifndef TARGET_HPP
#define TARGET_HPP

#include <memory>
#include <string>

#include "silverTypes.hpp"

/// Reprents a target looked by Silver.
class Target
{
public:

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

  /** Get the last received pose.
   * @param x X coordinate [m].
   * @param y Y coordinate [m].
   * @param theta Angle of pose [rad].
   */
  void getPose(double &x, double &y, double &theta);

  /// Return the last received silver::Pose.
  silver::Pose getPose();

private:

  class CheshireCat;
  std::auto_ptr<CheshireCat> smile;
};

#endif
