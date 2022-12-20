Address sinkAddress;
ApplicationContainer sinkApp;
ApplicationContainer sourceApp;


// Not sure what this is for... splitting something obviously
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
  size_t pos = txt.find( ch );
  size_t initialPos = 0;
  strs.clear();
  // Decompose statement
  while( pos != std::string::npos ) {
    strs.push_back( txt.substr( initialPos, pos - initialPos ) );
    initialPos = pos + 1;
    pos = txt.find( ch, initialPos );
  }
  // Add the last one
  strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );
  return strs.size();
}


// Callback function to change DataRate
void ChangeDataRate(Ptr<PointToPointNetDevice> d0, Ptr<PointToPointNetDevice> d1, Ptr<PointToPointChannel> c, double dr)
{
  // NS_LOG_INFO("Inside Datarate.");

  std::string dr_string = std::to_string(dr) + "Mbps";
  // NS_LOG_INFO("Create string.");
  d0->SetDataRate(DataRate(dr_string));
  // NS_LOG_INFO("Updated first.");
  d1->SetDataRate(DataRate(dr_string));
  // NS_LOG_INFO("Ending Datarate.");
}


// Callback function to change Propagation Delay
void ChangePropDelay(Ptr<PointToPointChannel> c, double delay)
{
  // NS_LOG_INFO("Inside Delay.");

  c->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));
  // c->SetDelay(MilliSeconds(delay));
  // NS_LOG_INFO("Ending Delay.");
}



void
// ScheduleDataRateAndDelay(Ptr<PointToPointNetDevice> d0, Ptr<PointToPointNetDevice> d1, Ptr<PointToPointChannel> c, bool oppo, int shift)
ScheduleDataRateAndDelay(int link_id, int shift = 0, bool oppo = true)
{

  // NS_LOG_INFO("Inde i Delay-dims");
  uint32_t speed_of_light = 299792458;
  int oppo_len = 772;
  int parallel_len = 2901;
  int arr_len;

  std::string oppo_dr_arr[oppo_len + 1];
  std::string oppo_dist_arr[oppo_len + 1];
  std::string parallel_dr_arr[parallel_len + 1];
  std::string parallel_dist_arr[parallel_len + 1];

  std::string mystring;
  std::fstream myfile;

  ///////////////////////////////////////
  // Find path to P5 folder !!!!

  char buff[FILENAME_MAX]; //create string buffer to hold path
  GetCurrentDir( buff, FILENAME_MAX );
  string current_working_dir(buff);
  // cout << "Current working dir is: " << current_working_dir << std::endl;

  std::smatch match;
  std::regex expression ("((.*)/ns-3.36.1/)(.*)");
  std::regex_search ( current_working_dir, match, expression );
  string path = match[1];
  path = path + "P5/satLinkData/";
  // cout << "New path is: " << path << std::endl;


  // Find path to P5 folder !!!!
  ///////////////////////////////////////


  if (oppo) {
    arr_len = oppo_len;
    myfile.open( path + "oppo.data");     // FILE PARSING HERE!!
  } else {
    arr_len = parallel_len;
    myfile.open( path + "parallel.data");     // FILE PARSING HERE!!
  }

  int i = 0;
  std::vector<std::string> v;

  if ( myfile.is_open() ) { // always check whether the file is open
    while(!myfile.eof()) {
      getline(myfile, mystring);
      split(mystring, v, ' ');
      if (v.size() > 1) {
        // std::cout << v.at(0) << " " << v.at(1) << " "<< v.at(2) << "\n";
        if (oppo) {
          oppo_dr_arr[i]=v.at(1);
          oppo_dist_arr[i]=v.at(2);
        } else {
          parallel_dr_arr[i]=v.at(1);
          parallel_dist_arr[i]=v.at(2);
        }
      }
      i++;
    }
    myfile.close();
  }

  int int_converted_dr_vals;
  double prop_delay;

  // std::cout << std::stoi(parallel_dr_arr[1268]) << " yo" << "\n";
  for(int i = 0; i < end_time ; i++) {
    // cout << "Index in loop is: " << ((i + shift) % arr_len) << std::endl;
    if (oppo) {
      int_converted_dr_vals = std::stoi(oppo_dr_arr[(i + shift) % arr_len]);
      prop_delay = (double)std::stoi(oppo_dist_arr[(i + shift) % arr_len]) / (double)speed_of_light;
    } else {
      int_converted_dr_vals = std::stoul(parallel_dr_arr[(i + shift) % arr_len]);
      prop_delay = (double)std::stoul(parallel_dist_arr[(i + shift) % arr_len]) / (double)speed_of_light;
    }
    // int int_converted_dr_vals = std::stoi(oppo_dr_arr[i]);
    // double prop_delay = (double)std::stoi(oppo_dist_arr[i]) / (double)speed_of_light;
    double megabits_per_sec = (double)int_converted_dr_vals / (double)1000000;
    // double scaled_datarate = megabits_per_sec / 1000.0;
    // cout << "Scale is: " << (double)scale << std::endl; 
    double scaled_datarate = megabits_per_sec / (double)scale;
    Simulator::Schedule (Seconds (i), &ChangeDataRate, link_devices[link_id][0], link_devices[link_id][1], link_channel[link_id], scaled_datarate);
    Simulator::Schedule (Seconds (i), &ChangePropDelay, link_channel[link_id], prop_delay);
  }

}


static void
ActivateError(Ptr<NetDevice> d, bool activate)
{  
  
  // NS_LOG_INFO("Inside ActivateError, functions - line 148");

  Ptr<RateErrorModel> errorFree = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> error = CreateObject<RateErrorModel> ();
  // an error rate of 1 means that error percentage is 100%
  // NS_LOG_INFO("Inside ActivateError, functions - line 153");
  errorFree->SetAttribute ("ErrorRate", DoubleValue (0));
  error->SetAttribute ("ErrorRate", DoubleValue (1));
  // if activate is true set error percentage to 100%
  // NS_LOG_INFO("Inside ActivateError, functions - line 157");
  if (activate) {
    // NS_LOG_INFO("Inside ActivateError, functions - line 159");
    d->SetAttribute ("ReceiveErrorModel", PointerValue (error));
  } else {
    // NS_LOG_INFO("Inside ActivateError, functions - line 162");
    d->SetAttribute ("ReceiveErrorModel", PointerValue (errorFree));
  }
}

Address createSink(int nodeId, int linkId, string tcp_version, int device_id = 1, int port = 8080)
// Address createSink(int nodeId, string tcp_version, int port = 8080)
{
  Address newSinkAddress ( InetSocketAddress ( link_interface[linkId].GetAddress(device_id), port ) );     // setup sink interface on node 7
  // Address newSinkAddress ( InetSocketAddress ( port ) );     // setup sink interface on node 7
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress ( Ipv4Address::GetAny (), port ) );

  sinkApp = packetSinkHelper.Install( node.Get (nodeId) );   // Install sink application on node

  // sinkApp.Start ( Seconds ( start_time ) );
  sinkApp.Start ( Seconds ( 0 ) );
  sinkApp.Stop ( simulationEndTime );

  return newSinkAddress;
}

BulkSendHelper createSource(int nodeId, string tcp_version, Address sink, string error, int datarate = maxBytes)
{
  BulkSendHelper newSource ("ns3::TcpSocketFactory", sink);
  newSource.SetAttribute ("MaxBytes", UintegerValue (datarate));

  string node_file = "";

  if (nodeId != 0) {
    sourceApp.Add(newSource.Install(node.Get(nodeId)));
    node_file = "-" + std::to_string(nodeId);
  }
  else {
    sourceApp = newSource.Install ( node.Get (nodeId) );
  }


  sourceApp.Start ( Seconds ( source_start_time ) );
  sourceApp.Stop ( simulationEndTime );


  // Setup tracing for RTT, Congestion Window and Transmitted Data
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceCwnd,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceCwnd,
                       tcp_version + "-cwnd" + node_file + error + ".data", nodeId);
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceRtt,
                       tcp_version + "-rtt" + node_file + error + ".data", nodeId);

  return newSource;
}

int setupThroughputTracing(string tcp_version, string error)
{
  monitor = flowmon.InstallAll ();
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceTxThroughput,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceTxThroughput,
                       tcp_version + "-tx-throughput" + error, monitor);
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRxThroughput,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceRxThroughput,
                       tcp_version + "-rx-throughput" + error, monitor);

  return 1;
}


int setupConThroughputTracing(string tcp_version, string error)
{
  monitor = flowmon.InstallAll ();

  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceConTxThroughput,
                       tcp_version + "-tx-throughput" + error, monitor);

  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceConRxThroughput,
                       tcp_version + "-rx-throughput" + error, monitor);

  return 1;
}





auto setupDefaultNodeTraffic(string tcp_version, string error, std::map<string,bool> config = default_config )
// int setupDefaultNodeTraffic(string tcp_version)
{
  emptyTraceFiles(tcp_version, error);
  Address sinkAddress = createSink(7, 7, tcp_version);

  // Address sinkAddress = createSink(5, 4, tcp_version);


  BulkSendHelper source = createSource(0, tcp_version, sinkAddress, error);

  if ( config["congestion"] == true ) {
    setupConThroughputTracing(tcp_version, error);
  }
  else {
    setupThroughputTracing(tcp_version, error);
  }

  struct retVals {        // Declare a local structure
    BulkSendHelper src;
    Address add;
  };

  // return 1;
  return retVals {source, sinkAddress};
  // return {source, sinkAddress};
}

int linkDrops(int linkId, float time, bool state)
{
  // NS_LOG_INFO("Inside linkDrop, functions - line 255");

  //   linkDrops(3, second_reroute_time, true);

  Simulator::Schedule (Seconds (time), &ActivateError, link_container[linkId].Get (1), state);
  Simulator::Schedule (Seconds (time), &ActivateError, link_container[linkId].Get (0), state);
  // Simulator::Schedule (Seconds (time), &ActivateError, link_container[nodeId+2].Get (0), state);


  // link_container[]  skal være  Ptr<NetDevice>

  return 1;
}

// int resetContainers()
// {  
//   (&node)->~NodeContainer();
//   new (&node) NodeContainer();
//   (&link_container)->~NetDeviceContainer();
//   new (&link_container) NetDeviceContainer();
//   (&link_interface)->~Ipv4InterfaceContainer();
//   new (&link_interface) Ipv4InterfaceContainer();

//   return 1;
// }
