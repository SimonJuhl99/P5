Address sinkAddress;
ApplicationContainer sinkApp; 
ApplicationContainer sourceApp;

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

Address createSink(int node, string tcp_version, int port = 8080)
{
  Address newSinkAddress ( InetSocketAddress ( link_interface[node].GetAddress(1), port ) );     // setup sink interface on node 7
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress ( Ipv4Address::GetAny (), port ) );
  sinkApp = packetSinkHelper.Install( c.Get (7) );   // Install sink application on node 7
  // ApplicationContainer sinkApp = packetSinkHelper.Install( c.Get (7) );   // Install sink application on node 7

  // sinkApp.Start ( Seconds ( start_time ) );
  sinkApp.Start ( Seconds ( 0 ) );
  sinkApp.Stop ( simulationEndTime );

  return newSinkAddress;
}

BulkSendHelper createSource(int node, string tcp_version, Address sink, string error, int datarate = maxBytes)
{
  BulkSendHelper newSource ("ns3::TcpSocketFactory", sink);
  newSource.SetAttribute ("MaxBytes", UintegerValue (datarate));
  sourceApp = newSource.Install ( c.Get (node) );
  // ApplicationContainer sourceApp = newSource.Install ( c.Get (node) );


  sourceApp.Start ( Seconds ( source_start_time ) );
  // sourceApp.Start ( Seconds ( start_time ) );
  sourceApp.Stop ( simulationEndTime );


  // Setup tracing for RTT, Congestion Window and Transmitted Data
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceCwnd,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceCwnd,
                       tcp_version + error + "-cwnd.data", node);
  // Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
  Simulator::Schedule (Seconds (source_start_time + 0.00001), &TraceRtt,
                       tcp_version + error + "-rtt.data", node);

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

int linkDrops(int node, float time, bool state)
{
    Simulator::Schedule (Seconds (time), &ActivateError, link_container[node+2].Get (0), state);
    Simulator::Schedule (Seconds (time), &ActivateError, link_container[node].Get (1), state);

    return 1;
}