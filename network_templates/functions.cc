Address sinkAddress;
// BulkSendHelper source;
// FlowMonitorHelper flowmon;
// Ptr<FlowMonitor> monitor;



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
  Address newSinkAddress ( InetSocketAddress ( node_interface[node].GetAddress(1), port ) );     // setup sink interface on node 7
  PacketSinkHelper packetSinkHelper ("ns3::TcpSocketFactory", InetSocketAddress ( Ipv4Address::GetAny (), port ) );
  ApplicationContainer sinkApp = packetSinkHelper.Install( c.Get (7) );   // Install sink application on node 7

  // sinkApp.Start ( Seconds ( start_time ) );
  sinkApp.Start ( Seconds ( 0 ) );
  sinkApp.Stop ( simulationEndTime );

  // // Setup tracing for Received Data
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRxThroughput,
                       tcp_version + "-rx-throughput.data", monitor);

  return newSinkAddress;
}

BulkSendHelper createSource(int node, string tcp_version, Address sink = sinkAddress)
{
  BulkSendHelper newSource ("ns3::TcpSocketFactory", sinkAddress);
  newSource.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApp = newSource.Install ( c.Get (node) );


  sourceApp.Start ( Seconds ( start_time ) );
  sourceApp.Stop ( simulationEndTime );


  // Setup tracing for RTT, Congestion Window and Transmitted Data
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceCwnd,
                       tcp_version + "-cwnd.data", node);
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRtt,
                       tcp_version + "-rtt.data", node);
  Simulator::Schedule (Seconds (start_time + 0.00001), &TraceTxThroughput,
                       tcp_version + "-tx-throughput.data", monitor);

  return newSource;
}



// int setupThroughputTracing(string tcp_version)
// {
//   monitor = flowmon.InstallAll ();
//   // Setup tracing for Transmitted Data
//   Simulator::Schedule (Seconds (start_time + 0.00001), &TraceTxThroughput,
//                        tcp_version + "-tx-throughput.data", monitor);
//   // Setup tracing for Received Data
//   Simulator::Schedule (Seconds (start_time + 0.00001), &TraceRxThroughput,
//                        tcp_version + "-rx-throughput.data", monitor);

//   return 1;
// }

int setupDefaultNodeTraffic(string tcp_version)
{


  sinkAddress = createSink(7, tcp_version);
  BulkSendHelper source = createSource(0, tcp_version);
  // setupThroughputTracing(tcp_version);

  return 1;
}