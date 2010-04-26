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

%include ../silvver.i

%apply unsigned { uint64_t };

%header %{

#define PYTHON_THREAD_BEGIN_BLOCK PyGILState_STATE thread_block = PyGILState_Ensure()

#define PYTHON_THREAD_END_BLOCK PyGILState_Release(thread_block)

static PyObject* pTimeExpired;
static PyObject* pConnectionError;

template <class T>
class Callback
{
public:
  Callback(PyObject* pyFunction)
    :pyFunction(pyFunction)
  {
    if (!PyCallable_Check(this->pyFunction)) {
      PyErr_SetString(PyExc_TypeError, "Construct fail! Expected a callable object.");
    }
    Py_INCREF(this->pyFunction);
  }

  Callback(const Callback& cc)
    :pyFunction(cc.pyFunction)
  {
    Py_INCREF(this->pyFunction);
  }

  ~Callback()
  {
    Py_DECREF(this->pyFunction);
  }

  PyObject* makeSwigPyObject(silvver::CameraReading<silvver::Position>* p)
  {
    return SWIG_NewPointerObj(p,
                              SWIGTYPE_p_silvver__CameraReadingT_silvver__Position_t,
                              0);
  }

  PyObject* makeSwigPyObject(silvver::CameraReading<silvver::Pose>* p)
  {
    return SWIG_NewPointerObj(p,
                              SWIGTYPE_p_silvver__CameraReadingT_silvver__Pose_t,
                              0);
  }

  // This method will be called from another thread, the one of ioService from
  // Connection class. Because of this is needed the SWIG_PYTHON_THREAD_*
  // macros.
  void operator()(silvver::CameraReading<T> cameraReading)
  {
    PYTHON_THREAD_BEGIN_BLOCK;
    PyObject* argument = this->makeSwigPyObject(&cameraReading);
    PyObject* arg = Py_BuildValue("(O)", argument);

    PyObject_CallObject(this->pyFunction, arg);
    Py_DECREF(argument);
    Py_DECREF(arg);
    PYTHON_THREAD_END_BLOCK;
  }
private:
  PyObject* const pyFunction;
};

%}

%init{
pTimeExpired = PyErr_NewException(const_cast<char*>("_silvver.TimeExpired"),
                                  NULL, NULL);
pConnectionError = PyErr_NewException(const_cast<char*>("_silvver.ConnectionError"),
                                      NULL, NULL);
PyModule_AddObject(m, "TimeExpired", pTimeExpired);
PyModule_AddObject(m, "ConnectionError", pConnectionError);

PyEval_InitThreads();
}

%exception {
  try
  {
    $action
  }
  catch (const silvver::connection_error& e)
  {
    PyErr_SetString(pConnectionError, const_cast<char*>(e.what()));
    return NULL;
  }
  catch (const silvver::time_expired_error& e)
  {
    PyErr_SetString(pTimeExpired, const_cast<char*>(e.what()));
    return NULL;
  }
  catch (const std::out_of_range& e)
  {
    PyErr_SetString(PyExc_StopIteration, const_cast<char*>(e.what()));
    return NULL;
  }
  catch (const std::exception& e)
  {
    PyErr_SetString(PyExc_SystemError, e.what());
    return NULL;
  }
  catch (...)
  {
    PyErr_SetString(PyExc_SystemError, "Unknown c++ exceptin threw.");
    return NULL;
  }
}

namespace boost
{
  %extend array<double, 9>
  {
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
}

namespace silvver
{
  %extend Position{
    char* __str__()
    {
      static char temp[256];
      sprintf(temp,"%g\t%g\t%g", $self->x,$self->y,$self->z);
      return temp;
    }
  }

  %extend Pose{
    char* __str__()
    {
      static char temp[256];
      sprintf(temp,"%s\t%s", silvver_Position___str__($self),
              boost_array_Sl_double_Sc_9_Sg____str__(&$self->rotationMatrix));
      return temp;
    }
  }

  %extend Identity<Position>{
    char* __str__()
    {
      static char temp[256];
      sprintf(temp,"%u\t%s", $self->uid, silvver_Position___str__($self));
      return temp;
    }
  }

  %extend Identity<Pose>{
    char* __str__()
    {
      static char temp[256];
      sprintf(temp,"%u\t%s", $self->uid, silvver_Pose___str__($self));
      return temp;
    }
  }

  %extend Target{
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

  %feature("shadow") Target::getNew %{
    def getNew(self, timedelta='infinity'):
        if timedelta == 'infinity':
            return self._getNew()
        else:
            return self._getNew(timedelta.days, timedelta.seconds,
                                timedelta.microseconds)
  %}

  %typemap(typecheck) boost::function<void (CameraReading<Position>)> {
    $1 = PyCallable_Check($input) ? 1 : 0;
  }

  %typemap(typecheck) boost::function<void (CameraReading<Pose>)> {
    $1 = PyCallable_Check($input) ? 1 : 0;
  }

  %typemap(in) boost::function<void (CameraReading<Position>)> {
    if (!PyCallable_Check($input))
    {
      PyErr_SetString(PyExc_TypeError, "Need a callable object!");
      return NULL;
    }
    Callback<silvver::Position> cb($input);
    $1 = cb;
  }

  %typemap(in) boost::function<void (CameraReading<Pose>)> {
    if (!PyCallable_Check($input))
    {
      PyErr_SetString(PyExc_TypeError, "Need a callable object!");
      return NULL;
    }
    Callback<silvver::Pose> cb($input);
    $1 = cb;
  }

  %template(PositionId) Identity<Position>;
  %template(PoseId) Identity<Pose>;

  %template(CameraReadingPosition) CameraReading<Position>;
  %template(CameraReadingPose) CameraReading<Pose>;

  %template(TargetPosition) Target<Position>;
  %template(TargetPose) Target<Pose>;

  %template(AbstractCameraPosition) AbstractCamera<Position>;
  %template(AbstractCameraPose) AbstractCamera<Pose>;
}

namespace std
{
  %template(VectorPositionId) vector<silvver::Identity<silvver::Position> >;
  %template(VectorPoseId) vector<silvver::Identity<silvver::Pose> >;
};
