/* Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
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

#ifndef SILVVER_DRIVER2_0_HPP
#define SILVVER_DRIVER2_0_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include <libplayercore/playercore.h>

#include "target.hpp"

/** \file
 * Header file where is the SilvverDriver2_0 class, for Player version 2.0
 */

/** Plug-in driver for the e-puck robot, in conformity with Player version 2.0.
 *
 * This class implements the player driver interface, and perform
 * all comunication with the player-server. It must to have a pointer
 * for each class that controls a e-puck device for which there are
 * a player interface, and bind them with player-server.
 * \author Renato Florentino Garcia
 * \date August 2008
 */
class SilvverDriver2_0 : public Driver
{
public:

  SilvverDriver2_0(ConfigFile* cf, int section);

  ~SilvverDriver2_0();

  virtual int Setup();

  virtual int Shutdown();

  virtual int ProcessMessage(MessageQueue* resp_queue,
                             player_msghdr* hdr, void* data);

  virtual int Subscribe(player_devaddr_t addr);

  virtual int Unsubscribe(player_devaddr_t addr);

  static void SilvverDriver2_0_Register(DriverTable* table);

  static Driver* SilvverDriver2_0_Init(ConfigFile* cf, int section);

private:

  virtual void Main();

  player_devaddr_t fiducialAddr;
  player_fiducial_data_t fiducialData;

  boost::ptr_vector<Target> targets;
};

#endif // SILVVER_DRIVER2_0_HPP
