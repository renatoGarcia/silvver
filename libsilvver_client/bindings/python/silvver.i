%module silvver
%{
#include "silvverTypes.hpp"
#include "target.hpp"
%}

%include "std_string.i"

%rename(boostarray) boost::array<double, 9>;

namespace boost
{
  class array<double,9>
  {
  public:
    %extend{
      void __setitem__(int key, double value)
      {
        $self->at(key) = value;
      }

      double __getitem__(int key)
      {
        return $self->at(key);
      }

      char* __str__()
      {
        static char temp[256];
        sprintf(temp, "%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g", (*$self)[0],
                (*$self)[1], (*$self)[2], (*$self)[3], (*$self)[4],
                (*$self)[5], (*$self)[6], (*$self)[7], (*$self)[8]);
        return temp;
      }
    }
  };
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

    %extend{
      char* __str__()
      {
        static char temp[256];
        sprintf(temp,"%g\t%g\t%g", $self->x,$self->y,$self->z);
        return temp;
      }
    }
  };

  struct Pose: public Position
  {
    boost::array<double, 9> rotationMatrix;

    double theta();

    Pose();

    Pose(const Pose& pose);

    %extend{
      char* __str__()
      {
        static char temp[256];
        sprintf(temp,"%s\t%s", silvver_Position___str__($self),
                boost_array_Sl_double_Sc_9_Sg____str__(&$self->rotationMatrix));
        return temp;
      }
    }
  };

  template<class BaseClass>
  struct Identity: public BaseClass
  {
    unsigned uid;

    Identity();
    Identity(const BaseClass& base, const unsigned uid);
  };

  %template(PositionId) Identity<Position>;
  %extend Identity<Position>{
    char* __str__()
    {
      static char temp[256];
      sprintf(temp,"%u\t%s", $self->uid, silvver_Position___str__($self));
      return temp;
    }
  }

  %template(PoseId) Identity<Pose>;
  %extend Identity<Pose>{
    char* __str__()
    {
      static char temp[256];
      sprintf(temp,"%u\t%s", $self->uid, silvver_Pose___str__($self));
      return temp;
    }
  }

  template<class T>
  class Target
  {
  public:

    /** Target class constructor.
     * @param targetId Target identifier.
     * @param serverIp IP address of silvver-server.
     * @param receptionistPort Port number of silvver-server receptionist.
     */
    Target(unsigned targetId,
           const std::string& serverIp="127.0.0.1",
           unsigned receptionistPort=12000);

    ~Target() throw();

    /// Connect to the silvver-server.
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
  };

  // %template(TargetPosition) Target<Position>;
  %template(TargetPose) Target<Pose>;

} //silvver namespace
