#include <iostream>
using namespace std;

#include "silverDriver2_0.hpp"

#include <cstddef>
#include <string>

#include <boost/foreach.hpp>
#include <boost/date_time/time_duration.hpp>

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
  std::cout << "Numero: " << nTargets << std::endl;
  int targetId;
  for (int i = 0; i < nTargets; ++i)
  {
    targetId = cf->ReadTupleInt(section, "targets", i, 0);
    std::cout << "id: " << targetId << std::endl;
    this->targets.push_back(new Target(targetId,
                                       false,
                                       cf->ReadString(section,
                                                      "serverIp",
                                                      "127.0.0.1"),
                                       cf->ReadInt(section,
                                                   "receptionistPort",
                                                   12000)));
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
  if(Message::MatchMessage(hdr, PLAYER_MSGTYPE_REQ,
                           PLAYER_FIDUCIAL_REQ_GET_GEOM,
                           this->fiducialAddr))
  {
    player_fiducial_geom_t fiducialGeom;
    fiducialGeom.pose.px = 0;
    fiducialGeom.pose.py = 0;
    fiducialGeom.pose.pa = 0;
    fiducialGeom.size.sw = 0;
    fiducialGeom.size.sl = 0;
    fiducialGeom.fiducial_size.sw = 0;
    fiducialGeom.fiducial_size.sl = 0;

    this->Publish(this->fiducialAddr,
                  resp_queue,
                  PLAYER_MSGTYPE_RESP_ACK,
                  PLAYER_FIDUCIAL_REQ_GET_GEOM,
                  (void*)&fiducialGeom,
                  sizeof(fiducialGeom),
                  NULL);

    return 0;
  }
  else
  {
    cout << "Processando menssagen" << endl;
    cout << "Type: " << (int)hdr->type
         << " subtype: " << (int)hdr->subtype
         << " inf: " << hdr->addr.interf
         << endl;
    PLAYER_ERROR("Silver:: Unhandled message");
    return -1;
  }
}

void
SilverDriver2_0::Main()
{
  while (true)
  {
    pthread_testcancel();

    if (this->InQueue->Empty() == false)
    {
      ProcessMessages();
    }

    system("sleep 0.1");

    this->fiducialData.fiducials_count = 0;
    silver::Pose pose;

    boost::ptr_vector<Target>::iterator ite = this->targets.begin();
    boost::ptr_vector<Target>::iterator end = this->targets.end();
    for (int i = 0; ite != end; ++ite)
    {
      try
      {
        pose = ite->getNewPose(boost::posix_time::millisec(0));
      }
      catch(const Target::time_expired_error& e)
      {
        continue;
      }

      this->fiducialData.fiducials_count++;
      this->fiducialData.fiducials[i].id = ite->getId();
      this->fiducialData.fiducials[i].pose.px = pose.x;
      this->fiducialData.fiducials[i].pose.py = pose.y;
      this->fiducialData.fiducials[i].pose.pz = 0;
      this->fiducialData.fiducials[i].pose.proll = 0;
      this->fiducialData.fiducials[i].pose.ppitch = 0;
      this->fiducialData.fiducials[i].pose.pyaw = pose.theta;
      this->fiducialData.fiducials[i].upose.px = 0;
      this->fiducialData.fiducials[i].upose.py = 0;
      this->fiducialData.fiducials[i].upose.pz = 0;
      this->fiducialData.fiducials[i].upose.proll = 0;
      this->fiducialData.fiducials[i].upose.ppitch = 0;
      this->fiducialData.fiducials[i].upose.pyaw = 0;
      i++;
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
  table->AddDriver((char*)"silver", SilverDriver2_0_Init);
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
