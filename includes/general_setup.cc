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


uint32_t prev = 0;
Time prevTime = Seconds (0);

static std::map<uint32_t, bool> firstCwnd;
static std::map<uint32_t, bool> firstRtt;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
static std::map<uint32_t, uint32_t> cWndValue;


