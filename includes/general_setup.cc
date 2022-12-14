#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using std::string;
using std::cout;

// --------------------------
// -- Own Includes & Setup
NS_LOG_COMPONENT_DEFINE (scenario);



// // Congestion Window Variables
// static std::map<uint32_t, bool> firstCwnd;
// static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
// static std::map<uint32_t, uint32_t> cWndValue;
// // Roundtrip Time Variables
// static std::map<uint32_t, bool> firstRtt;
// static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
// // Timeout Variables
// static std::map<uint32_t, bool> firstRto;
// static std::map<uint32_t, Ptr<OutputStreamWrapper>> rtoStream;


// Simulation Time Setup
int start_time = 0;
int end_time = 10;
Time simulationEndTime = Seconds (end_time);     // Set simulation time

// Maximum Source Datarate Setup
uint32_t maxBytes = 0;      // 0 means "no limit"


