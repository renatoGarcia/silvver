#include "silverDriver2_0.hpp"

#include <cstddef>
#include <string>

#include <boost/foreach.hpp>

SilverDriver2_0::SilverDriver2_0(ConfigFile* cf, int section)
  :Driver(cf, section, true, PLAYER_MSGQUEUE_DEFAULT_MAXLEN)
{
  if (cf->ReadDeviceAddr(&this->fiducialAddr, section, "provides",
                         PLAYER_FIDUCIAL_CODE, -1, NULL) == 0)
  {
    if (this->AddInterface(this->fiducialAddr) != 0)
    {
      this->SetError(-1);
      return;
    }
  }

  int nTargets = cf->GetTupleCount(section, "targets");
  int targetId;
  for (int i = 0; i < nTargets; ++i)
  {
    targetId = cf->ReadTupleInt(section, "targets", i, 0);
    this->targets.push_back(new Target(targetId,
                                       false,
                                       cf->ReadString(section, "serverIp", "127.0.0.1"),
                                       cf->ReadInt(section, "receptionistPort", 12000)));
  }
}

SilverDriver2_0::~SilverDriver2_0()
{}

int
SilverDriver2_0::Setup()
{
  this->StartThread();
  return 0;
}

int
SilverDriver2_0::Shutdown()
{
  this->StopThread();
  return 0;
}

int
SilverDriver2_0::Subscribe(player_devaddr_t addr)
{
  BOOST_FOREACH(Target& target, this->targets)
  {
    target.connect();
  }

  return Driver::Subscribe(addr);
}

int
SilverDriver2_0::Unsubscribe(player_devaddr_t addr)
{
  BOOST_FOREACH(Target& target, this->targets)
  {
    target.disconnect();
  }

  return Driver::Unsubscribe(addr);
}

int
SilverDriver2_0::ProcessMessage(MessageQueue* resp_queue,
                                player_msghdr* hdr, void* data)
{
//   else
  {
    PLAYER_ERROR("Epuck:: Unhandled message");
    return -1;
  }
}

void
SilverDriver2_0::Main()
{
  player_camera_data_t cameraData;

  while (true)
  {
    pthread_testcancel();

    if (this->InQueue->Empty() == false)
    {
      ProcessMessages();
    }

    this->Publish(this->fiducialAddr, NULL,
                  PLAYER_MSGTYPE_DATA, PLAYER_FIDUCIAL_DATA_SCAN,
                  reinterpret_cast<void*>(&this->fiducialData),
                  sizeof(this->fiducialData), NULL);
  }
}

Driver*
SilverDriver2_0::SilverDriver2_0_Init(ConfigFile* cf, int section)
{
  return((Driver*)(new SilverDriver2_0(cf, section)));
}

void
SilverDriver2_0::SilverDriver2_0_Register(DriverTable* table)
{
  table->AddDriver((char*)"epuck", SilverDriver2_0_Init);
}

/*! \relates SilverDriver2_0
 * Function called from player server for to register the silver driver.
 */
extern "C"
{
  int player_driver_init(DriverTable* table)
  {
    PLAYER_MSG0(1, "Registering silver driver.");
    SilverDriver2_0::SilverDriver2_0_Register(table);
    return (0);
  }
}
