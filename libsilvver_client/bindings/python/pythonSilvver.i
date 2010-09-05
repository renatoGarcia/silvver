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

%pythoncode %{
import cv
TimeExpired = _silvver.TimeExpired
ConnectionError = _silvver.ConnectionError
%}

namespace silvver {

%typemap(typecheck) AbstractCameraUid&
{
  if ((PySequence_Check($input)) && (PySequence_Size($input) == 2))
  {
    PyObject* first = PySequence_GetItem($input, 0);
    PyObject* second = PySequence_GetItem($input, 1);
    if ((PyInt_AsLong(first) > 0) && (PyInt_AsLong(second) > 0))
    {
      $1 = true;
    }
    else
    {
      $1 = false;
    }
    Py_DECREF(first);
    Py_DECREF(second);
  }
  else
  {
    void* temp;
    if (!SWIG_IsOK(SWIG_ConvertPtr($input, &temp, $1_descriptor, 0)))
    {
      $1 = false;
    }
    else
    {
      $1 = true;
    }
  }
}

%typemap(in) AbstractCameraUid& (bool isTuple)
{
  if (PySequence_Check($input))
  {
    if (PySequence_Size($input) != 2)
    {
      SWIG_exception_fail(SWIG_TypeError,
                          "Sequences to be set as $1_type "
                          "must have length 2");
    }
    isTuple = true;
    PyObject* first = PySequence_GetItem($input, 0);
    PyObject* second = PySequence_GetItem($input, 1);
    $1 = new $1_basetype(PyInt_AsUnsignedLongMask(first),
                         PyInt_AsUnsignedLongMask(second));
    Py_DECREF(first);
    Py_DECREF(second);
  }
  else
  {
    isTuple = false;
    int res2 = SWIG_ConvertPtr($input, (void**) &$1, $1_descriptor, 0);
    if (!SWIG_IsOK(res2))
    {
      SWIG_exception_fail(SWIG_ArgError(res2),
                          "Erron converting argument to $1_type "
                          "in function $symname");
    }
    if (!$1)
    {
      SWIG_exception_fail(SWIG_ValueError,
                          "invalid null reference of type $1_type "
                          "in function $symname");
    }
  }
}

%typemap(freearg) AbstractCameraUid&
{
  if (isTuple$argnum)
    delete $1;
}

%typemap(typecheck) TargetUid& = AbstractCameraUid&;
%typemap(in) TargetUid& = AbstractCameraUid&;
%typemap(freearg) TargetUid& = AbstractCameraUid&;

} // namespace silvver

%apply unsigned { uint64_t };

%include ../silvver.i

%header %{
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

namespace boost {

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

} // namespace boost

namespace silvver {

%extend AbstractCameraUid{
  const char* __str__()
  {
    static std::stringstream ss(std::stringstream::out);
    ss.str("");
    ss << *$self;
    return ss.str().c_str();
  }
}

%extend TargetUid{
  const char* __str__()
  {
    static std::stringstream ss(std::stringstream::out);
    ss.str("");
    ss << *$self;
    return ss.str().c_str();
  }
}

%extend Position{
  const char* __str__()
  {
    static std::stringstream ss(std::stringstream::out);
    ss.str("");
    ss << *$self;
    return ss.str().c_str();
  }
}

%extend Pose{
  const char* __str__()
  {
    static std::stringstream ss(std::stringstream::out);
    ss.str("");
    ss << *$self;
    return ss.str().c_str();
  }
}

%extend Identity{
  const char* __str__()
  {
    static std::stringstream ss(std::stringstream::out);
    ss.str("");
    ss << *$self;
    return ss.str().c_str();
  }
}

%extend Image{
  PyObject* toString()
  {
    return PyString_FromStringAndSize($self->imageData,
                                      (Py_ssize_t)$self->imageSize);
  }
}

%pythoncode {
    def toIplImage(self):
        cv_img = cv.CreateImageHeader((self.width,self.height), cv.IPL_DEPTH_8U, 3) # RGB image
        cv.SetData(cv_img, self.toString())
        return cv_img

    Image.toIplImage = toIplImage
    del toIplImage
%}

%extend Target{
  Identity<T> _getUnseen(int days, int seconds, int microseconds)
  {
    return $self->getUnseen(boost::posix_time::hours(days*24) +
                            boost::posix_time::seconds(seconds) +
                            boost::posix_time::microseconds(microseconds));
  }

  Identity<T> _getUnseen()
  {
    return $self->getUnseen(boost::date_time::pos_infin);
  }

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

%feature("shadow") Target::getUnseen %{
    def getUnseen(self, timedelta='infinity'):
        if timedelta == 'infinity':
            return self._getUnseen()
        else:
            return self._getUnseen(timedelta.days, timedelta.seconds,
                                   timedelta.microseconds)
%}

%feature("shadow") Target::getNext %{
    def getNext(self, timedelta='infinity'):
        if timedelta == 'infinity':
            return self._getNext()
        else:
            return self._getNext(timedelta.days, timedelta.seconds,
                                 timedelta.microseconds)
%}

%template(Identity_Position) Identity<Position>;
%template(Identity_Pose) Identity<Pose>;

%template(CameraReading_Position) CameraReading<Position>;
%template(CameraReading_Pose) CameraReading<Pose>;

%template(Target_Position) Target<Position>;
%template(Target_Pose) Target<Pose>;

%template(AbstractCamera_Position) AbstractCamera<Position>;
%template(AbstractCamera_Pose) AbstractCamera<Pose>;

} // namespace silvver

namespace std {

%template(VectorPositionId) vector<silvver::Identity<silvver::Position> >;
%template(VectorPoseId) vector<silvver::Identity<silvver::Pose> >;

} // namespace std
