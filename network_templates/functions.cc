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
  // Ptr<PointToPointNetDevice> d0 = link_devices[link_id][0];
  // Ptr<PointToPointNetDevice> d1 = link_devices[link_id][1];
  // Ptr<PointToPointChannel> c = link_channel[link_id];


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

  // NS_LOG_INFO("Før Israel Data");
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

  // NS_LOG_INFO("Efter Israel Data");

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

  // NS_LOG_INFO("Filling arrays");

  int int_converted_dr_vals;
  double prop_delay;

  // std::cout << std::stoi(parallel_dr_arr[1268]) << " yo" << "\n";
  for(int i = 0; i < arr_len ; i++) {
    // NS_LOG_INFO("Function done.");
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
    double scaled_datarate = megabits_per_sec / 1000.0;
    // Simulator::Schedule (Seconds (i), &ChangeDataRate, d0, d1, c, scaled_datarate);
    Simulator::Schedule (Seconds (i), &ChangeDataRate, link_devices[link_id][0], link_devices[link_id][1], link_channel[link_id], scaled_datarate);
    // Simulator::Schedule (Seconds (i), &ChangePropDelay, c, prop_delay);
    Simulator::Schedule (Seconds (i), &ChangePropDelay, link_channel[link_id], prop_delay);
  }


  // NS_LOG_INFO("Function done.");

}


static void
ActivateError(Ptr<NetDevice> d, bool activate)
{
  Ptr<RateErrorModel> errorFree = CreateObject<RateErrorModel> ();
  Ptr<RateErrorModel> error = CreateObject<RateErrorModel> ();
  // an error rate of 1 means that error percentage is 100%
  errorFree->SetAttribute ("ErrorRate", DoubleValue (0));
  error->SetAttribute ("ErrorRate", DoubleValue (1));
  // if activate is true set error percentage to 100%
  if (activate) {
    d->SetAttribute ("ReceiveErrorModel", PointerValue (error));
  } else {
    d->SetAttribute ("ReceiveErrorModel", PointerValue (errorFree));
  }
}

Address createSink(int nodeId, string tcp_version, int device_id = 1, int port = 8080)
// Address createSink(int nodeId, string tcp_version, int port = 8080)
{
  Address newSinkAddress ( InetSocketAddress ( link_interface[nodeId].GetAddress(device_id), port ) );     // setup sink interface on node 7
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
                       tcp_version + node_file + error + "-cwnd.data", nodeId);
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceRtt,
                       tcp_version + node_file + error + "-rtt.data", nodeId);

  return newSource;
}

int setupThroughputTracing(string tcp_version, string error)
{
  monitor = flowmon.InstallAll ();
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceTxThroughput,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceTxThroughput,
                       tcp_version + error + "-tx-throughput.data", monitor);
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRxThroughput,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceRxThroughput,
                       tcp_version + error + "-rx-throughput.data", monitor);

  return 1;
}





auto setupDefaultNodeTraffic(string tcp_version, string error)
// int setupDefaultNodeTraffic(string tcp_version)
{
  emptyTraceFiles(tcp_version, error);
  Address sinkAddress = createSink(7, tcp_version);
  BulkSendHelper source = createSource(0, tcp_version, sinkAddress, error);
  setupThroughputTracing(tcp_version, error);

  struct retVals {        // Declare a local structure 
    BulkSendHelper src;
    Address add;
  };

  // return 1;
  return retVals {source, sinkAddress};
  // return {source, sinkAddress};
}

int linkDrops(int nodeId, float time, bool state)
{
    Simulator::Schedule (Seconds (time), &ActivateError, link_container[nodeId+2].Get (0), state);
    Simulator::Schedule (Seconds (time), &ActivateError, link_container[nodeId].Get (1), state);

    return 1;
}