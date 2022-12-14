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


// Simulation Time Setup
int start_time = 0;
int end_time = start_time + 10;
Time simulationEndTime = Seconds (end_time);     // Set simulation time

// Set Default Network Speeds
DataRate defaultDatarate("8Mbps");
Time defaultLinkDelay = MilliSeconds (5);

// Throughput Trace Interval
// int throughputTraceInterval = 0.05;
