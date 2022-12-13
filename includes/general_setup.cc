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



// Congestion Window Variables
static std::map<uint32_t, bool> firstCwnd;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> cWndStream;
static std::map<uint32_t, uint32_t> cWndValue;
// Roundtrip Time Variables
static std::map<uint32_t, bool> firstRtt;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rttStream;
// Timeout Variables
static std::map<uint32_t, bool> firstRto;
static std::map<uint32_t, Ptr<OutputStreamWrapper>> rtoStream;


// Simulation Time Setup
// float offset = 0.00001;
// float start_time = 0 + offset;
int start_time = 0;
// int start_time = 1;
int end_time = 10;
Time simulationEndTime = Seconds (end_time);     // Set simulation time

// Source Setup
uint32_t maxBytes = 0;      // 0 means "no limit"


// // Old tracing
// uint32_t prev = 0;
// Time prevTime = Seconds (0);


// // Tracing

// uint32_t prevTx = 0;    // Earlier values for throughput tracing
// uint32_t prevRx = 0;    // Earlier values for throughput tracing
// Time prevTxTime = Seconds (0);    // Earlier timestamps for throughput tracing
// Time prevRxTime = Seconds (0);    // Earlier timestamps for throughput tracing


