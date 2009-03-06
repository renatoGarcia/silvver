#ifndef SILVER_DRIVER2_0_HPP
#define SILVER_DRIVER2_0_HPP

#include <boost/ptr_container/ptr_vector.hpp>

#include <libplayercore/playercore.h>

#include "target.hpp"

/** \file
 * Header file where is the SilverDriver2_0 class, for Player version 2.0
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
class SilverDriver2_0 : public Driver
{
public:

  SilverDriver2_0(ConfigFile* cf, int section);

  ~SilverDriver2_0();

  virtual int Setup();

  virtual int Shutdown();

  virtual int ProcessMessage(MessageQueue* resp_queue,
                             player_msghdr* hdr, void* data);

  virtual int Subscribe(player_devaddr_t addr);

  virtual int Unsubscribe(player_devaddr_t addr);

  static void SilverDriver2_0_Register(DriverTable* table);

  static Driver* SilverDriver2_0_Init(ConfigFile* cf, int section);

private:

  virtual void Main();

  player_devaddr_t fiducialAddr;
  player_fiducial_data_t fiducialData;

  boost::ptr_vector<Target> targets;
};

#endif // SILVER_DRIVER2_0_HPP
