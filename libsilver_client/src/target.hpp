#ifndef TARGET_HPP
#define TARGET_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include <boost/date_time/time_duration.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "silverTypes.hpp"

namespace silver
{
  template<class U> class CheshireCat;

  /// Represents a target looked by Silver.
  template<class T>
  class Target
  {
  public:

    class time_expired_error : public std::runtime_error
    {
    public:
      time_expired_error(const std::string& whatArg)
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

    /// Get the last received target localization.
    Identity<T> getLast();

    /** Get a never gotten taget localization, or throw an
     * old_pose_error exception.
     * @param waitTime
     *
     * @return
     */
    Identity<T> getNew(const boost::posix_time::time_duration&
                       waitTime = boost::date_time::pos_infin);

    /** Return the next received target localization.
     * This function wait until a new Pose arrives and returns;
     */
    Identity<T> getNext(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);

  private:

    std::auto_ptr<CheshireCat<T> > smile;
  };
}
#endif // TARGET_HPP
