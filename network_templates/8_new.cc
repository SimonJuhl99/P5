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

PointToPointHelper p2p;


PointToPointNetDevice link_devices[8][2];
PointToPointChannel link_channel[8];


// Ptr<PointToPointNetDevice> link_devices[8][2];
// Ptr<PointToPointChannel> link_channel[8];
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

Ipv4AddressHelper ipv4;


//  ----------------------------------------------
//  --  Setup of Source & Sink Variables  --
// Maximum Source Datarate Setup
uint32_t maxBytes = 0;      // 0 means "no limit"

int current_link = 0;
int current_node_a = 0;
int current_node_b = 0;

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
int PointToPointHelper::InstallWithoutContainer(int node_a, int node_b)
// auto PointToPointHelper::InstallWithoutContainer(Ptr<Node> a, Ptr<Node> b) 
{

  Ptr<PointToPointNetDevice> devA = m_deviceFactory.Create<PointToPointNetDevice> ();
  devA->SetAddress (Mac48Address::Allocate ());



  /////////////////////////////
  // New version....

  NodeContainer a = node.Get(node_a);
  NodeContainer b = node.Get(node_b);
  // Ptr<Node> a = node.Get(node_a);
  // Ptr<Node> b = node.Get(node_b);

  // New version....
  /////////////////////////////

  // Device 1 Setup
  a->AddDevice (devA);
  Ptr<Queue<Packet> > queueA = m_queueFactory.Create<Queue<Packet> > ();
  devA->SetQueue (queueA);
  // Device 2 Setup
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




  /////////////////////////////
  // New version....

  NS_LOG_INFO("Current Link Channel is " + std::to_string(current_link));
  NS_LOG_INFO("Current Link Nodes are " + std::to_string(current_node_a) + " and " + std::to_string(current_node_b));

  link_devices[current_link][current_node_a] = devA;
  link_devices[current_link][current_node_b] = devB;
  link_channel[current_link] = channel;

  current_link++;

  return 1;

  // New version....
  /////////////////////////////


  /////////////////////////////
  // Old version....

  // struct retVals {
  //   Ptr<PointToPointNetDevice> i1, i2;
  //   Ptr<PointToPointChannel> i3;
  // };

  // return retVals {devA, devB, channel};
}




int createLink(int node_a, int node_b)
{
  current_node_a = node_a;
  current_node_b = node_b;

  auto [temp_d0, temp_d1, temp_link] = p2p.InstallWithoutContainer(node.Get(0), node.Get(2));

  current_link++;
  // link_channel[0] = temp_link;
  // link_devices[0][0] = temp_d0;
  // link_devices[0][1] = temp_d1;

  return 1;
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


  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (node);

  InternetStackHelper internet;
  internet.Install (node);



  p2p.SetDeviceAttribute ("DataRate", DataRateValue (defaultDatarate));
  // p2p.SetChannelAttribute ("Delay", TimeValue (defaultLinkDelay));

  /////////////////////////////
  // New version....


  // createLink(0, 2, 0);
  // createLink(1, 2, 1);
  // createLink(2, 3, 2);
  // createLink(3, 4, 3);
  // createLink(3, 5, 4);
  // createLink(4, 6, 5);
  // createLink(5, 6, 6);
  // createLink(6, 7, 7);

  p2p.InstallWithoutContainer(0, 2);
  p2p.InstallWithoutContainer(1, 2);
  p2p.InstallWithoutContainer(2, 3);
  p2p.InstallWithoutContainer(3, 4);
  p2p.InstallWithoutContainer(3, 5);
  p2p.InstallWithoutContainer(4, 6);
  p2p.InstallWithoutContainer(5, 6);
  p2p.InstallWithoutContainer(6, 7);


  // New version....
  /////////////////////////////


  /////////////////////////////
  // Old version....

  // auto [temp_d0_link_0_2, temp_d1_link_0_2, temp_link_0_2] = p2p.InstallWithoutContainer(node.Get(0), node.Get(2));

  // auto [temp_d0_link_1_2, temp_d1_link_1_2, temp_link_1_2] = p2p.InstallWithoutContainer(node.Get(1), node.Get(2));

  // auto [temp_d0_link_2_3, temp_d1_link_2_3, temp_link_2_3] = p2p.InstallWithoutContainer(node.Get(2), node.Get(3));

  // auto [temp_d0_link_3_4, temp_d1_link_3_4, temp_link_3_4] = p2p.InstallWithoutContainer(node.Get(3), node.Get(4));

  // auto [temp_d0_link_3_5, temp_d1_link_3_5, temp_link_3_5] = p2p.InstallWithoutContainer(node.Get(3), node.Get(5));

  // auto [temp_d0_link_4_6, temp_d1_link_4_6, temp_link_4_6] = p2p.InstallWithoutContainer(node.Get(4), node.Get(6));

  // auto [temp_d0_link_5_6, temp_d1_link_5_6, temp_link_5_6] = p2p.InstallWithoutContainer(node.Get(5), node.Get(6));

  // auto [temp_d0_link_6_7, temp_d1_link_6_7, temp_link_6_7] = p2p.InstallWithoutContainer(node.Get(6), node.Get(7));


  // link_channel[0] = temp_link_0_2;
  // link_channel[1] = temp_link_1_2;
  // link_channel[2] = temp_link_2_3;
  // link_channel[3] = temp_link_3_4;
  // link_channel[4] = temp_link_3_5;
  // link_channel[5] = temp_link_4_6;
  // link_channel[6] = temp_link_5_6;
  // link_channel[7] = temp_link_6_7;


  // link_devices[0][0] = temp_d0_link_0_2;
  // link_devices[0][1] = temp_d1_link_0_2;
  // link_devices[1][0] = temp_d0_link_1_2;
  // link_devices[1][1] = temp_d1_link_1_2;
  // link_devices[2][0] = temp_d0_link_2_3;
  // link_devices[2][1] = temp_d1_link_2_3;
  // link_devices[3][0] = temp_d0_link_3_4;
  // link_devices[3][1] = temp_d1_link_3_4;
  // link_devices[4][0] = temp_d0_link_3_5;
  // link_devices[4][1] = temp_d1_link_3_5;
  // link_devices[5][0] = temp_d0_link_4_6;
  // link_devices[5][1] = temp_d1_link_4_6;
  // link_devices[6][0] = temp_d0_link_5_6;
  // link_devices[6][1] = temp_d1_link_5_6;
  // link_devices[7][0] = temp_d0_link_6_7;
  // link_devices[7][1] = temp_d1_link_6_7;


  // Old version....
  /////////////////////////////

  // NS_LOG_INFO("Setting Delays:");

  link_channel[0]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // NS_LOG_INFO("After first channel");
  link_channel[1]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  // NS_LOG_INFO("After second channel");
  link_channel[2]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[3]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[4]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[5]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[6]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...
  link_channel[7]->SetAttribute("Delay", StringValue(std::to_string(delay) + "s"));   // Delay is weird and static here...






  // Setup the connection between a net-devices in each end
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




  // We create the channels first without any IP addressing information
  NS_LOG_INFO ("Creating channels.");


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



