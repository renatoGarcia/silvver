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
TimeExpired = _silvver.TimeExpired
ForcedExit = _silvver.ForcedExit
ConnectionError = _silvver.ConnectionError

import thread
import threading
import weakref

class CallbackWrapper(object):
    def __init__(self, client, handler):
        self._client = client
        self._handler = handler

        self._receive_ended = threading.Condition()
        thread.start_new_thread(CallbackWrapper._receive,
                                (weakref.proxy(self), self._receive_ended))

    def _receive(weak_self, condition):
        while True:
            try:
                weak_self._handler(weak_self._client.getUnseen())
            except ForcedExit:
                condition.acquire()
                condition.notify()
                condition.release()
                return

    def __del__(self):
        self._receive_ended.acquire()
        self._client.exitWait()
        self._receive_ended.wait()
        self._receive_ended.release()
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
static PyObject* pForcedExit;
static PyObject* pConnectionError;
%}

%init{
pTimeExpired = PyErr_NewException(const_cast<char*>("_silvver.TimeExpired"),
                                  NULL, NULL);
pForcedExit = PyErr_NewException(const_cast<char*>("_silvver.ForcedExit"),
                                 NULL, NULL);
pConnectionError = PyErr_NewException(const_cast<char*>("_silvver.ConnectionError"),
                                      NULL, NULL);
PyModule_AddObject(m, "TimeExpired", pTimeExpired);
PyModule_AddObject(m, "ForcedExit", pForcedExit);
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
  catch (const silvver::forced_exit& e)
  {
    PyErr_SetString(pForcedExit, const_cast<char*>(e.what()));
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

%extend AbstractCamera{
  CameraReading<T> _getUnseen(int days, int seconds, int microseconds)
  {
    silvver::CameraReading<T> cr;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      cr = $self->getUnseen(boost::posix_time::hours(days*24) +
                            boost::posix_time::seconds(seconds) +
                            boost::posix_time::microseconds(microseconds));
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return cr;
  }

  CameraReading<T> _getUnseen()
  {
    silvver::CameraReading<T> cr;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      cr = $self->getUnseen(boost::date_time::pos_infin);
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return cr;
  }

  CameraReading<T> _getNext(int days, int seconds, int microseconds)
  {
    silvver::CameraReading<T> cr;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      cr =  $self->getNext(boost::posix_time::hours(days*24) +
                           boost::posix_time::seconds(seconds) +
                           boost::posix_time::microseconds(microseconds));
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return cr;
  }

  CameraReading<T> _getNext()
  {
    silvver::CameraReading<T> cr;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      cr = $self->getNext(boost::date_time::pos_infin);
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return cr;
  }
}

%feature("shadow") AbstractCamera::getUnseen %{
    def getUnseen(self, timedelta='infinity'):
        if timedelta == 'infinity':
            return self._getUnseen()
        else:
            return self._getUnseen(timedelta.days, timedelta.seconds,
                                   timedelta.microseconds)
%}

%feature("shadow") AbstractCamera::getNext %{
    def getNext(self, timedelta='infinity'):
        if timedelta == 'infinity':
            return self._getNext()
        else:
            return self._getNext(timedelta.days, timedelta.seconds,
                                 timedelta.microseconds)
%}

%extend Target{
  Identity<T> _getUnseen(int days, int seconds, int microseconds)
  {
    silvver::Identity<T> id;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      id = $self->getUnseen(boost::posix_time::hours(days*24) +
                            boost::posix_time::seconds(seconds) +
                            boost::posix_time::microseconds(microseconds));
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return id;
  }

  Identity<T> _getUnseen()
  {
    silvver::Identity<T> id;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      id = $self->getUnseen(boost::date_time::pos_infin);
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return id;
  }

  Identity<T> _getNext(int days, int seconds, int microseconds)
  {
    silvver::Identity<T> id;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      id = $self->getNext(boost::posix_time::hours(days*24) +
                          boost::posix_time::seconds(seconds) +
                          boost::posix_time::microseconds(microseconds));
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return id;
  }

  Identity<T> _getNext()
  {
    silvver::Identity<T> id;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      id = $self->getNext(boost::date_time::pos_infin);
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return id;
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

%extend TargetSet{
  std::vector<Identity<T> > _getUnseen(int days, int seconds, int microseconds)
  {
    std::vector<silvver::Identity<T> > vecId;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      vecId = $self->getUnseen(boost::posix_time::hours(days*24) +
                               boost::posix_time::seconds(seconds) +
                               boost::posix_time::microseconds(microseconds));
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return vecId;
  }

  std::vector<Identity<T> > _getUnseen()
  {
    std::vector<silvver::Identity<T> > vecId;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      vecId = $self->getUnseen(boost::date_time::pos_infin);
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return vecId;
  }

  std::vector<Identity<T> > _getNext(int days, int seconds, int microseconds)
  {
    std::vector<silvver::Identity<T> > vecId;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      vecId = $self->getNext(boost::posix_time::hours(days*24) +
                             boost::posix_time::seconds(seconds) +
                             boost::posix_time::microseconds(microseconds));
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return vecId;
  }

  std::vector<Identity<T> > _getNext()
  {
    std::vector<silvver::Identity<T> > vecId;
    Py_BEGIN_ALLOW_THREADS
    try
    {
      vecId = $self->getNext(boost::date_time::pos_infin);
    }
    catch (...)
    {
      Py_BLOCK_THREADS
      throw;
    }
    Py_END_ALLOW_THREADS
    return vecId;
  }
}

%feature("shadow") TargetSet::getUnseen %{
    def getUnseen(self, timedelta='infinity'):
        if timedelta == 'infinity':
            return self._getUnseen()
        else:
            return self._getUnseen(timedelta.days, timedelta.seconds,
                                   timedelta.microseconds)
%}

%feature("shadow") TargetSet::getNext %{
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

%template(AbstractCamera_Position) AbstractCamera<Position>;
%template(AbstractCamera_Pose) AbstractCamera<Pose>;

%template(Target_Position) Target<Position>;
%template(Target_Pose) Target<Pose>;

%template(TargetSet_Position) TargetSet<Position>;
%template(TargetSet_Pose) TargetSet<Pose>;

} // namespace silvver

namespace std {

%template(VectorPositionId) vector<silvver::Identity<silvver::Position> >;
%template(VectorPoseId) vector<silvver::Identity<silvver::Pose> >;

 } // namespace std
