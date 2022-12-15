/*  ---------------------------------------
        --  8 Node Network Template --
    ------------------------- 

  ________________________________
  --  Default Network Topology  -- 

    n0 \      Default Datarate:       +-- n4 --+
        \        8 Mbps              /          \
      l0 \    Default Delay:        / l3         \ l5           
          \      5 ms              /              \
           \                      /                \          l7      
             n2 --------------- n3                  n6 --------------- n7
           /          l2          \                /
          /                        \  l4          /
      l1 /             (2Mbps, 5ms) \            / l6
        /                            \          /
    n1 /                              +-- n5 --+



// KEEP AND UPDATE THE TOP COMMENT/EXPLANATION
*/



//    ERSTAT ALLE "i1i2" og andre tal, med "link_interface[1]".... 
//    OBS!!!  Tilsvarende tal er ikke nødvendigvis det første i rækken...


//  ---------------------------------------
//  --  Setup of Network Variables  --
NodeContainer c;
NodeContainer n0n2;
NodeContainer n1n2;
NodeContainer n2n3;
NodeContainer n3n4;
NodeContainer n3n5;
NodeContainer n4n6;
NodeContainer n5n6;
NodeContainer n6n7;

PointToPointHelper p2p;

NetDeviceContainer link_container[8];
Ipv4InterfaceContainer link_interface[8];

Ptr<ConstantPositionMobilityModel> mn0;
Ptr<ConstantPositionMobilityModel> mn1;
Ptr<ConstantPositionMobilityModel> mn2;
Ptr<ConstantPositionMobilityModel> mn3;
Ptr<ConstantPositionMobilityModel> mn4;
Ptr<ConstantPositionMobilityModel> mn5;
Ptr<ConstantPositionMobilityModel> mn6;
Ptr<ConstantPositionMobilityModel> mn7;

//  ----------------------------------------------
//  --  Setup of Source & Sink Variables  --
// Maximum Source Datarate Setup
uint32_t maxBytes = 0;      // 0 means "no limit"

// Address sinkAddress;

// // AsciiTraceHelper ascii;
// FlowMonitorHelper flowmon;
// // Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
// Ptr<FlowMonitor> monitor;


// DataRate defaultDatarate = DataRate (4 * bottleneckBandwidth.GetBitRate ());
// DataRate defaultDatarate("8Mbps");
// Time defaultLinkDelay = MilliSeconds (5);



AnimationInterface    // Need to be the XML tracing object, for it to work.
build_network (string tcp_version, string error)
{
  string transportProtocol = "ns3::Tcp" + tcp_version;


  // Source Setup
  // Config::SetDefault ("ns3::BulkSendApplication::SendSize", UintegerValue (100000));    // Change the packetsize of a source application
  //Config::SetDefault ("ns3::BulkSendApplication::DataRate", StringValue ("448kb/s"));    // Change the datarate of source applications


  // General Setup
  Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));     // Respond to routing table changes
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", TypeIdValue (TypeId::LookupByName (transportProtocol)));    // Tell which TCP version to use
  Config::SetDefault ("ns3::TcpSocket::InitialCwnd", UintegerValue (10));   // Set initial congestion window value

  LogComponentEnable(scenario, LOG_LEVEL_ALL);    // Initiate logging

  ////////////////////////////////////////////
  //  --  Start actual network setup  --
  NS_LOG_INFO ("\nCreating nodes.");
  c.Create (8);


  n0n2 = NodeContainer (c.Get (0), c.Get (2));    // Nye ende noder
  n1n2 = NodeContainer (c.Get (1), c.Get (2));    // Nye ende noder
  n2n3 = NodeContainer (c.Get (2), c.Get (3));    // Gammel n0n1
  n3n4 = NodeContainer (c.Get (3), c.Get (4));    // Gammel n1n5
  n3n5 = NodeContainer (c.Get (3), c.Get (5));    // Gammel n1n2
  n4n6 = NodeContainer (c.Get (4), c.Get (6));    // Gammel n5n3
  n5n6 = NodeContainer (c.Get (5), c.Get (6));    // Gammel n2n3
  n6n7 = NodeContainer (c.Get (6), c.Get (7));    // Gammel n3n4

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (c);

  InternetStackHelper internet;
  internet.Install (c);

  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Creating channels.");

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (defaultDatarate));
  p2p.SetChannelAttribute ("Delay", TimeValue (defaultLinkDelay));

  // Setup the connection between a net-devices in each end
  link_container[0] = p2p.Install (n0n2);
  link_container[1] = p2p.Install (n1n2);
  link_container[2] = p2p.Install (n2n3);   // Gammel d0d1
  link_container[3] = p2p.Install (n3n4);   // Gammel d1d5
  link_container[4] = p2p.Install (n3n5);   // Gammel d1d2
  link_container[5] = p2p.Install (n4n6);   // Gammel d5d3
  link_container[6] = p2p.Install (n5n6);   // Gammel d2d3
  link_container[7] = p2p.Install (n6n7);   // Gammel d3d4

  //      OBS   OBS   OBS   OBS   OBS
  // --  Stadig ikke sikker på at "Bottleneck'en" bliver opdateret fra run()
  //      OBS   OBS   OBS   OBS   OBS

  // p2p.SetDeviceAttribute ("DataRate", DataRateValue (bottleneckBandwidth));
  // p2p.SetChannelAttribute ("Delay", TimeValue (bottleneckDelay));
  // link_container[6] = p2p.Install (n5n6);   // Gammel d2d3



  /*  ----------------------------
  //    --  IP address setup  --
  //  ----------------------------
  //  Setting up Subnets for each (single P2P connection) network
  //  IP's will be assigned from these subnet values.
  */
  Ipv4AddressHelper ipv4;

  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  link_interface[0] = ipv4.Assign (link_container[0]);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  link_interface[1] = ipv4.Assign (link_container[1]);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  link_interface[2] = ipv4.Assign (link_container[2]);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  link_interface[3] = ipv4.Assign (link_container[3]);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  link_interface[4] = ipv4.Assign (link_container[4]);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  link_interface[5] = ipv4.Assign (link_container[5]);

  ipv4.SetBase ("10.1.7.0", "255.255.255.0");
  link_interface[6] = ipv4.Assign (link_container[6]);

  ipv4.SetBase ("10.1.8.0", "255.255.255.0");
  link_interface[7] = ipv4.Assign (link_container[7]);

  NS_LOG_INFO ("IP's and interfaces assigned.");

  // Create router nodes, initialize routing database and set up the routing
  // tables in the nodes.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();


  // XML tracing for NetAnim setup 
  AnimationInterface anim ( tcp_version + error + ".xml");    
  
  mn0 = c.Get (0)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn1 = c.Get (1)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn2 = c.Get (2)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn3 = c.Get (3)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn4 = c.Get (4)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn5 = c.Get (5)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn6 = c.Get (6)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn7 = c.Get (7)  ->  GetObject<ConstantPositionMobilityModel> ();

  mn0->SetPosition ( Vector ( 0.5, 0.7, 0 ) );
  mn1->SetPosition ( Vector ( 0.5, 1.3, 0 ) );
  mn2->SetPosition ( Vector ( 1.0, 1.0, 0 ) );
  mn3->SetPosition ( Vector ( 1.5, 1.0, 0 ) );
  mn4->SetPosition ( Vector ( 2.0, 0.7, 0 ) );
  mn5->SetPosition ( Vector ( 2.0, 1.3, 0 ) );
  mn6->SetPosition ( Vector ( 2.5, 1.0, 0 ) );
  mn7->SetPosition ( Vector ( 3.0, 1.0, 0 ) );

  anim.UpdateNodeSize( c.Get(0)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(1)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(2)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(3)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(4)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(5)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(6)->GetId(), .1, .1 );
  anim.UpdateNodeSize( c.Get(7)->GetId(), .1, .1 );

  uint64_t maxPktsPerFile = 1844674407370955161;   // Max size

  anim.SetMaxPktsPerTraceFile(maxPktsPerFile);    // To trace larger simulations

  // anim.EnableQueueCounters();   // Tracing queues, dequeues and queue drops
  // anim.Ipv4DropTrace();   // Getting info on why packets are dropped... seems to need some parameters though... might be wrong... 

 // Tracing Ascii Data Setup, tracing every single thing happening in the network
  // Ptr<OutputStreamWrapper> stream = ascii.CreateFileStream ( tcp_version + error + ".tr" );
  // p2p.EnableAsciiAll ( stream );
  // p2p.EnablePcapAll ( tcp_version );

  // monitor = flowmon.InstallAll ();


  return anim;
}
