/*
 * Copyright 2020 IFPEN-CEA
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "SolverStater.h"
/* Author : havep at Wed Sep  5 17:55:36 2012
 * Generated by createNew
 */

#ifdef WIN32
#include <windows.h>
#define ARCANE_TIMER_USE_CLOCK
#else
#include <errno.h>
#include <sys/time.h>
#endif

#include <arccore/base/FatalErrorException.h>
#include <arccore/base/TraceInfo.h>

/*---------------------------------------------------------------------------*/

namespace Alien
{

using namespace Arccore;
/*---------------------------------------------------------------------------*/

#ifdef ARCANE_TIMER_USE_CLOCK
static clock_t current_clock_value = 0;
#endif

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

Real BaseSolverStater::_getVirtualTime()
{
  // From Arcane 1.16.3 to work with historical timers and Windows
#ifdef ARCANE_TIMER_USE_CLOCK
  clock_t cv = ::clock();
  Real diffv = static_cast<Real>(cv - current_clock_value);
  return diffv / CLOCKS_PER_SEC;
#else
  struct itimerval time_val;
  int r = ::getitimer(ITIMER_VIRTUAL, &time_val);
  if (r != 0)
    _errorInTimer("getitimer()", r);
  Real v = static_cast<Real>(time_val.it_value.tv_sec) * 1. + static_cast<Real>(time_val.it_value.tv_usec) * 1e-6;
  return (5000000. - v);
#endif
}

/*---------------------------------------------------------------------------*/

Real BaseSolverStater::_getRealTime()
{
  // From Arcane 1.16.3 to work with old timers and Windows.
#ifdef WIN32
  SYSTEMTIME t;
  GetSystemTime(&t);
  Real hour = t.wHour * 3600.0;
  Real minute = t.wMinute * 60.0;
  Real second = t.wSecond;
  Real milli_second = t.wMilliseconds * 1e-3;
  return (hour + minute + second + milli_second);
#else
  struct timeval tp;
  int r = gettimeofday(&tp, 0);
  if (r != 0)
    _errorInTimer("gettimeofday()", r);
  Real tvalue =
  (static_cast<Real>(tp.tv_sec) * 1. + static_cast<Real>(tp.tv_usec) * 1.e-6);
  return tvalue;
#endif
}

/*---------------------------------------------------------------------------*/

void BaseSolverStater::_errorInTimer(const String& msg, int retcode)
{
  throw FatalErrorException(
  A_FUNCINFO, String::format("{0} return code: {1} errno: {2}", msg, retcode, errno));
}

/*---------------------------------------------------------------------------*/

void BaseSolverStater::_startTimer()
{
  ALIEN_ASSERT((m_state == eNone), ("Unexpected SolverStater state %d", m_state));
  m_real_time = _getRealTime();
  m_cpu_time = _getVirtualTime();
}

/*---------------------------------------------------------------------------*/

void BaseSolverStater::_stopTimer()
{
  ALIEN_ASSERT((m_state != eNone), ("Unexpected SolverStater state %d", m_state));
  m_real_time = _getRealTime() - m_real_time;
  m_cpu_time = _getVirtualTime() - m_cpu_time;
}

/*---------------------------------------------------------------------------*/

} // namespace Alien

/*---------------------------------------------------------------------------*/