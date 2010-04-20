%module silvver

%{
#include "silvverTypes.hpp"
#include "target.hpp"
#include "abstractCamera.hpp"
#include <boost/system/system_error.hpp>
static PyObject* pTimeExpired;
static PyObject* pConnectionError;
%}

%init{
pTimeExpired = PyErr_NewException(const_cast<char*>("_silvver.TimeExpired"),
                                  NULL, NULL);
pConnectionError = PyErr_NewException(const_cast<char*>("_silvver.ConnectionError"),
                                      NULL, NULL);
PyModule_AddObject(m, "TimeExpired", pTimeExpired);
PyModule_AddObject(m, "ConnectionError", pConnectionError);
}

%include "std_string.i"
%include "std_vector.i"
namespace std {
  %template(VectorPosition) vector<silvver::Identity<silvver::Position> >;
  %template(VectorPose) vector<silvver::Identity<silvver::Pose> >;
};

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
  %template(CameraReadingPosition) CameraReading<Position>;
  %template(CameraReadingPose) CameraReading<Pose>;

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
    /// Can throw boost::system::system_error when silvver-server
    /// is unreachable.
    void connect();
    %exception connect{
      try
      {
        $action
      }
      catch (boost::system::system_error& e)
      {
        PyErr_SetString(pConnectionError, const_cast<char*>(e.what()));
        return NULL;
      }
    }

    void disconnect();
    %exception disconnect{
      try
      {
        $action
      }
      catch (boost::system::system_error& e)
      {
        PyErr_SetString(pConnectionError, const_cast<char*>(e.what()));
        return NULL;
      }
    }

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

    %extend{
      Identity<T> _getNew(int days, int seconds, int microseconds)
      {
        return $self->getNew(boost::posix_time::hours(days*24) +
                             boost::posix_time::seconds(seconds) +
                             boost::posix_time::microseconds(microseconds));
      }

      Identity<T> _getNew()
      {
        return $self->getNew(boost::date_time::pos_infin);
      }
    }

    %pythoncode{
      def getNew(self, timedelta='infinity'):
          if timedelta == 'infinity':
              return self._getNew()
          else:
              return self._getNew(timedelta.days, timedelta.seconds,
                                  timedelta.microseconds)
    }

    %exception _getNew{
      try
      {
        $action
      }
      catch (silvver::time_expired_error& e)
      {
        PyErr_SetString(pTimeExpired, const_cast<char*>(e.what()));
        return NULL;
      }
    }

    /** Return the next received target localization.
     * This function wait until a new Pose arrives and returns;
     */
    Identity<T> getNext(const boost::posix_time::time_duration&
                        waitTime = boost::date_time::pos_infin);

    %extend{
      Identity<T> _getNext(int days, int seconds, int microseconds)
      {
        return $self->getNext(boost::posix_time::hours(days*24) +
                              boost::posix_time::seconds(seconds) +
                              boost::posix_time::microseconds(microseconds));
      }

      Identity<T> _getNext()
      {
        return $self->getNext(boost::date_time::pos_infin);
      }
    }

    %pythoncode{
      def getNext(self, timedelta='infinity'):
          if timedelta == 'infinity':
              return self._getNext()
          else:
              return self._getNext(timedelta.days, timedelta.seconds,
                                   timedelta.microseconds)
    }

    %exception _getNext{
      try
      {
        $action
      }
      catch (silvver::time_expired_error& e)
      {
        PyErr_SetString(pTimeExpired, const_cast<char*>(e.what()));
        return NULL;
      }
    }

  };
  %template(TargetPosition) Target<Position>;
  %template(TargetPose) Target<Pose>;


  template<class T>
  class AbstractCamera
  {
  public:
    AbstractCamera(boost::function<void (CameraReading<T>)> callbackFunction,
                   std::string abstractCameraUid,
                   const std::string& serverIp="127.0.0.1",
                   unsigned receptionistPort=12000);

    ~AbstractCamera() throw();

    void connect();

    void disconnect();

    /** Get the id of camera.
     * @return The id of this camera.
     */
    std::string getId();

  private:
    class CheshireCat;
    std::auto_ptr<CheshireCat> smile;
  };



} //silvver namespace
