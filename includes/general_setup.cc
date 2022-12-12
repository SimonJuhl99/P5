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

// --------------------------
// -- Own Includes & Setup
NS_LOG_COMPONENT_DEFINE (scenario);

// #include "tracing.cc"
// #include "../network_templates/8_build.cc"

using namespace ns3;
using std::string;
using std::cout;



std::map<uint32_t, bool> firstCwnd;
std::map<uint32_t, bool> firstRtt;
std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
std::map<uint32_t, uint32_t> cWndValue;

int start_time = 0;
// Time start_time = Seconds (0.0);     // Set simulation time
Time simulationEndTime = Seconds (3.0);     // Set simulation time
uint32_t maxBytes = 0;      // 0 means "no limit"

uint32_t prev = 0;
Time prevTime = Seconds (0);



