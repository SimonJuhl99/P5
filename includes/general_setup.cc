#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include <regex>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
using std::string;
using std::cout;

typedef std::map<string,bool> conf;     // Create datatype for configuration of scenarios
static conf default_config;     // Prepare variable for default scenario configuration
static conf current_config;     // Prepare variable for default scenario configuration
static string current_tcp_version;


// --------------------------
// -- Own Includes & Setup
NS_LOG_COMPONENT_DEFINE (scenario);


// Simulation Time Setup
int start_time = 0;
int end_time = start_time + 240;    // Maybe our "optimal" sim-time
Time simulationEndTime = Seconds (end_time);     // Set simulation time
int source_start_time = 0;


// Throughput Trace Interval
// int throughputTraceInterval = 0.05;

///////////////////////////////////////////////////
//  --  Scenario Preparation Variables  --


//  --  Average Satellite Data  --

static int datarateScale = 1000;    // Downscaling factor
int realDataRate = 500;     // 500Mbit/sec
float scaledDatarate = (float)realDataRate / datarateScale;     // Simulation Datarate
DataRate defaultDatarate( std::to_string(scaledDatarate) + "Mbps");     // DataRate scaled 1000, so is 0.5Mbps
float delayMs = 25;     // Milliseconds
float delay = delayMs * 0.001;    // Converted to seconds



//  --  Moving Scenario  --
int fullPass = 772;


//  --  Reroute Scenario  --
float first_reroute_time = start_time + 100.07;
float second_reroute_time = start_time + 150.07;

//  --  Moving & Reroute Scenario  --
int shift_first = 30;
int shift_second = 340;     // Reroute right before peak of second route.
// float first_dynamic_reroute = 712.07;
float first_dynamic_reroute = fullPass - (shift_first * 2);
float second_dynamic_reroute = 2500.07;     // After the simulation... set here because we only need one





////////////////////////////////////////////
//  --  Script argument variables  --

CommandLine cmd (__FILE__);
float argDataRate = 0;
int argScale = 0;
int argDelay = 0;
int argEndTime = 0;
int argShift = 0;
int argChange = 0;
int argChange2 = 0;
bool argAll = false;
bool argMove = false;
bool argCon = false;
bool argReroute = false;
bool argError = false;
bool argRunAll = false;




////////////////////////////////////////////
//  --  Generally Used Functions  --

void printIt(conf m) {
    cout << "Configuration is: " << std::endl; 
    cout << "    simulation time: " << end_time << " seconds" << std::endl;
    cout << "    default delay: " << delay << " seconds" << std::endl;
    cout << "    default datarate: " << scaledDatarate << " Mbit/s" << std::endl;
    // for(std::map<string,bool>::iterator it=m.begin();it!=m.end();++it)
    for(conf::iterator it=m.begin();it!=m.end();++it)
        cout << "    " << it->first<<": "<<it->second<< std::endl;
    cout << "\n";
}