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
      l1 /                          \            / l6
        /                            \          /
    n1 /                              +-- n5 --+



// KEEP AND UPDATE THE TOP COMMENT/EXPLANATION
*/



//    ERSTAT ALLE "i1i2" og andre tal, med "link_interface[1]".... 
//    OBS!!!  Tilsvarende tal er ikke nødvendigvis det første i rækken...


//  ---------------------------------------
//  --  Setup of Network Variables  --
NodeContainer node;
// NodeContainer n0n2;
// NodeContainer n1n2;
// NodeContainer n2n3;
// NodeContainer n3n4;
// NodeContainer n3n5;
// NodeContainer n4n6;
// NodeContainer n5n6;
// NodeContainer n6n7;

PointToPointHelper p2p;


Ptr<PointToPointNetDevice> link_devices[8][2];
// Ptr<PointToPointNetDevice> d0_link_0_2;
// Ptr<PointToPointNetDevice> d1_link_0_2;
// Ptr<PointToPointNetDevice> d0_link_1_2;
// Ptr<PointToPointNetDevice> d1_link_1_2;
// Ptr<PointToPointNetDevice> d0_link_2_3;
// Ptr<PointToPointNetDevice> d1_link_2_3;
// Ptr<PointToPointNetDevice> d0_link_3_4;
// Ptr<PointToPointNetDevice> d1_link_3_4;
// Ptr<PointToPointNetDevice> d0_link_3_5;
// Ptr<PointToPointNetDevice> d1_link_3_5;
// Ptr<PointToPointNetDevice> d0_link_4_6;
// Ptr<PointToPointNetDevice> d1_link_4_6;
// Ptr<PointToPointNetDevice> d0_link_5_6;
// Ptr<PointToPointNetDevice> d1_link_5_6;
// Ptr<PointToPointNetDevice> d0_link_6_7;
// Ptr<PointToPointNetDevice> d1_link_6_7;

Ptr<PointToPointChannel> link_channel[8];
// Ptr<PointToPointChannel> link_0_2;
// Ptr<PointToPointChannel> link_1_2;
// Ptr<PointToPointChannel> link_2_3;
// Ptr<PointToPointChannel> link_3_4;
// Ptr<PointToPointChannel> link_3_5;
// Ptr<PointToPointChannel> link_4_6;
// Ptr<PointToPointChannel> link_5_6;
// Ptr<PointToPointChannel> link_6_7;





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









// Needed for change of delay on links
PointToPointHelper::PointToPointHelper ()
{
  m_queueFactory.SetTypeId ("ns3::DropTailQueue<Packet>");
  m_deviceFactory.SetTypeId ("ns3::PointToPointNetDevice");
  m_channelFactory.SetTypeId ("ns3::PointToPointChannel");
  m_enableFlowControl = true;
}

// Needed for change of delay on links
auto
PointToPointHelper::InstallWithoutContainer(Ptr<Node> a, Ptr<Node> b) {

  Ptr<PointToPointNetDevice> devA = m_deviceFactory.Create<PointToPointNetDevice> ();
  devA->SetAddress (Mac48Address::Allocate ());
  a->AddDevice (devA);
  Ptr<Queue<Packet> > queueA = m_queueFactory.Create<Queue<Packet> > ();
  devA->SetQueue (queueA);
  Ptr<PointToPointNetDevice> devB = m_deviceFactory.Create<PointToPointNetDevice> ();
  devB->SetAddress (Mac48Address::Allocate ());
  b->AddDevice (devB);
  Ptr<Queue<Packet> > queueB = m_queueFactory.Create<Queue<Packet> > ();
  devB->SetQueue (queueB);
  if (m_enableFlowControl)
    {
      // Aggregate NetDeviceQueueInterface objects
      Ptr<NetDeviceQueueInterface> ndqiA = CreateObject<NetDeviceQueueInterface> ();
      ndqiA->GetTxQueue (0)->ConnectQueueTraces (queueA);
      devA->AggregateObject (ndqiA);
      Ptr<NetDeviceQueueInterface> ndqiB = CreateObject<NetDeviceQueueInterface> ();
      ndqiB->GetTxQueue (0)->ConnectQueueTraces (queueB);
      devB->AggregateObject (ndqiB);
    }

  Ptr<PointToPointChannel> channel = 0;
  channel = m_channelFactory.Create<PointToPointChannel> ();

  devA->Attach (channel);
  devB->Attach (channel);

  struct retVals {
    Ptr<PointToPointNetDevice> i1, i2;
    Ptr<PointToPointChannel> i3;
  };

  return retVals {devA, devB, channel};
  // return netDevVector;
}










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
  node.Create (8);




  // n0n2 = NodeContainer (node.Get (0), node.Get (2));    // Nye ende noder
  // n1n2 = NodeContainer (node.Get (1), node.Get (2));    // Nye ende noder
  // n2n3 = NodeContainer (node.Get (2), node.Get (3));    // Gammel n0n1
  // n3n4 = NodeContainer (node.Get (3), node.Get (4));    // Gammel n1n5
  // n3n5 = NodeContainer (node.Get (3), node.Get (5));    // Gammel n1n2
  // n4n6 = NodeContainer (node.Get (4), node.Get (6));    // Gammel n5n3
  // n5n6 = NodeContainer (node.Get (5), node.Get (6));    // Gammel n2n3
  // n6n7 = NodeContainer (node.Get (6), node.Get (7));    // Gammel n3n4


  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (node);

  InternetStackHelper internet;
  internet.Install (node);

  //////////////////////////////
  //  --  Nyt Setup  --

  p2p.SetDeviceAttribute ("DataRate", DataRateValue (defaultDatarate));
  p2p.SetChannelAttribute ("Delay", TimeValue (defaultLinkDelay));


  auto [temp_d0_link_0_2, temp_d1_link_0_2, temp_link_0_2] = p2p.InstallWithoutContainer(node.Get(0), node.Get(2));

  auto [temp_d0_link_1_2, temp_d1_link_1_2, temp_link_1_2] = p2p.InstallWithoutContainer(node.Get(1), node.Get(2));

  auto [temp_d0_link_2_3, temp_d1_link_2_3, temp_link_2_3] = p2p.InstallWithoutContainer(node.Get(2), node.Get(3));

  auto [temp_d0_link_3_4, temp_d1_link_3_4, temp_link_3_4] = p2p.InstallWithoutContainer(node.Get(3), node.Get(4));

  auto [temp_d0_link_3_5, temp_d1_link_3_5, temp_link_3_5] = p2p.InstallWithoutContainer(node.Get(3), node.Get(5));

  auto [temp_d0_link_4_6, temp_d1_link_4_6, temp_link_4_6] = p2p.InstallWithoutContainer(node.Get(4), node.Get(6));

  auto [temp_d0_link_5_6, temp_d1_link_5_6, temp_link_5_6] = p2p.InstallWithoutContainer(node.Get(5), node.Get(6));

  auto [temp_d0_link_6_7, temp_d1_link_6_7, temp_link_6_7] = p2p.InstallWithoutContainer(node.Get(6), node.Get(7));





  // link_0_2 = temp_link_0_2;
  // link_1_2 = temp_link_1_2;
  // link_2_3 = temp_link_2_3;
  // link_3_4 = temp_link_3_4;
  // link_3_5 = temp_link_3_5;
  // link_4_6 = temp_link_4_6;
  // link_5_6 = temp_link_5_6;
  // link_6_7 = temp_link_6_7;


  link_channel[0] = temp_link_0_2;
  link_channel[1] = temp_link_1_2;
  link_channel[2] = temp_link_2_3;
  link_channel[3] = temp_link_3_4;
  link_channel[4] = temp_link_3_5;
  link_channel[5] = temp_link_4_6;
  link_channel[6] = temp_link_5_6;
  link_channel[7] = temp_link_6_7;


  // d0_link_0_2 = temp_d0_link_0_2;
  // d1_link_0_2 = temp_d1_link_0_2;
  // d0_link_1_2 = temp_d0_link_1_2;
  // d1_link_1_2 = temp_d1_link_1_2;
  // d0_link_2_3 = temp_d0_link_2_3;
  // d1_link_2_3 = temp_d1_link_2_3;
  // d0_link_3_4 = temp_d0_link_3_4;
  // d1_link_3_4 = temp_d1_link_3_4;
  // d0_link_3_5 = temp_d0_link_3_5;
  // d1_link_3_5 = temp_d1_link_3_5;
  // d0_link_4_6 = temp_d0_link_4_6;
  // d1_link_4_6 = temp_d1_link_4_6;
  // d0_link_5_6 = temp_d0_link_5_6;
  // d1_link_5_6 = temp_d1_link_5_6;
  // d0_link_6_7 = temp_d0_link_6_7;
  // d1_link_6_7 = temp_d1_link_6_7;


  link_devices[0][0] = temp_d0_link_0_2;
  link_devices[0][1] = temp_d1_link_0_2;
  link_devices[1][0] = temp_d0_link_1_2;
  link_devices[1][1] = temp_d1_link_1_2;
  link_devices[2][0] = temp_d0_link_2_3;
  link_devices[2][1] = temp_d1_link_2_3;
  link_devices[3][0] = temp_d0_link_3_4;
  link_devices[3][1] = temp_d1_link_3_4;
  link_devices[4][0] = temp_d0_link_3_5;
  link_devices[4][1] = temp_d1_link_3_5;
  link_devices[5][0] = temp_d0_link_4_6;
  link_devices[5][1] = temp_d1_link_4_6;
  link_devices[6][0] = temp_d0_link_5_6;
  link_devices[6][1] = temp_d1_link_5_6;
  link_devices[7][0] = temp_d0_link_6_7;
  link_devices[7][1] = temp_d1_link_6_7;


  link_channel[0]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[1]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[2]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[3]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[4]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[5]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[6]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[7]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...



  // link_0_2->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_1_2->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_2_3->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_3_4->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_3_5->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_4_6->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_5_6->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // link_6_7->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...



  // Setup the connection between a net-devices in each end
  // link_container[0].Add(d0_link_0_2);
  // link_container[0].Add(d1_link_0_2);
  // link_container[1].Add(d0_link_1_2);
  // link_container[1].Add(d1_link_1_2);
  // link_container[2].Add(d0_link_2_3);
  // link_container[2].Add(d1_link_2_3);
  // link_container[3].Add(d0_link_3_4);
  // link_container[3].Add(d1_link_3_4);
  // link_container[4].Add(d0_link_3_5);
  // link_container[4].Add(d1_link_3_5);
  // link_container[5].Add(d0_link_4_6);
  // link_container[5].Add(d1_link_4_6);
  // link_container[6].Add(d0_link_5_6);
  // link_container[6].Add(d1_link_5_6);
  // link_container[7].Add(d0_link_6_7);
  // link_container[7].Add(d1_link_6_7);


  link_container[0].Add(link_devices[0][0]);
  link_container[0].Add(link_devices[0][1]);
  link_container[1].Add(link_devices[1][0]);
  link_container[1].Add(link_devices[1][1]);
  link_container[2].Add(link_devices[2][0]);
  link_container[2].Add(link_devices[2][1]);
  link_container[3].Add(link_devices[3][0]);
  link_container[3].Add(link_devices[3][1]);
  link_container[4].Add(link_devices[4][0]);
  link_container[4].Add(link_devices[4][1]);
  link_container[5].Add(link_devices[5][0]);
  link_container[5].Add(link_devices[5][1]);
  link_container[6].Add(link_devices[6][0]);
  link_container[6].Add(link_devices[6][1]);
  link_container[7].Add(link_devices[7][0]);
  link_container[7].Add(link_devices[7][1]);



  //  --  Nyt Setup  --
  //////////////////////////////







  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Creating channels.");

  // p2p.SetDeviceAttribute ("DataRate", DataRateValue (defaultDatarate));
  // p2p.SetChannelAttribute ("Delay", TimeValue (defaultLinkDelay));

  // // Setup the connection between a net-devices in each end
  // link_container[0] = p2p.Install (n0n2);
  // link_container[1] = p2p.Install (n1n2);
  // link_container[2] = p2p.Install (n2n3);   // Gammel d0d1
  // link_container[3] = p2p.Install (n3n4);   // Gammel d1d5
  // link_container[4] = p2p.Install (n3n5);   // Gammel d1d2
  // link_container[5] = p2p.Install (n4n6);   // Gammel d5d3
  // link_container[6] = p2p.Install (n5n6);   // Gammel d2d3
  // link_container[7] = p2p.Install (n6n7);   // Gammel d3d4

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
  
  mn0 = node.Get (0)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn1 = node.Get (1)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn2 = node.Get (2)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn3 = node.Get (3)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn4 = node.Get (4)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn5 = node.Get (5)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn6 = node.Get (6)  ->  GetObject<ConstantPositionMobilityModel> ();
  mn7 = node.Get (7)  ->  GetObject<ConstantPositionMobilityModel> ();

  mn0->SetPosition ( Vector ( 0.5, 0.7, 0 ) );
  mn1->SetPosition ( Vector ( 0.5, 1.3, 0 ) );
  mn2->SetPosition ( Vector ( 1.0, 1.0, 0 ) );
  mn3->SetPosition ( Vector ( 1.5, 1.0, 0 ) );
  mn4->SetPosition ( Vector ( 2.0, 0.7, 0 ) );
  mn5->SetPosition ( Vector ( 2.0, 1.3, 0 ) );
  mn6->SetPosition ( Vector ( 2.5, 1.0, 0 ) );
  mn7->SetPosition ( Vector ( 3.0, 1.0, 0 ) );

  anim.UpdateNodeSize( node.Get(0)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(1)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(2)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(3)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(4)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(5)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(6)->GetId(), .1, .1 );
  anim.UpdateNodeSize( node.Get(7)->GetId(), .1, .1 );

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



