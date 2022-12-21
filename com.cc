//  --  Script Scenario Name  --
static char const *scenario = "Rerouting";

#include "includes/general_setup.cc"
#include "includes/tracing.cc"
#include "network_templates/8.cc"
#include "network_templates/functions.cc"



int
run (string tcp_version, conf config = default_config)
{
  /* --------------------------------------------------------
  //  --    General Setup   --
  //  --  DON'T TOUCH THIS!  --
  */

  cout << "\n________________________________________ \n________________________________________ \nPreparing for Simulation Using TCP " << tcp_version << std::endl;
  // NS_LOG_INFO ("\n________________________________________ \n________________________________________ \nPreparing for Simulation Using TCP " + tcp_version + ".\n" );

  config.insert(default_config.begin(), default_config.end());    // config takes presidence here.

  current_config = config;
  current_tcp_version = tcp_version;


  if (config["moving"] == true) {
    if (config["rerouting"] == true) {
      int leftoverPass = fullPass - shift_second;

      if (argChange > 0) {
        first_dynamic_reroute = argChange;
      }
      if (argChange2 > 0) {
        cout << "Only use this outside \"moving\" scenario" << std::endl;
      }

      end_time = start_time + leftoverPass + (first_dynamic_reroute + shift_first);

      // simulationEndTime = Time(Seconds (end_time));     // Set simulation time
      float newDatarate = scaledDatarate * 10;
      defaultDatarate = DataRate( std::to_string(newDatarate) + "Mbps");
      // defaultDatarate = DataRate("5Mbps");
      
      if (argDelay <= 0) {
        delay = (delayMs * 0.001) / 5;
      }
    }
    else {
      end_time = start_time + fullPass;
    }
      simulationEndTime = Time(Seconds (end_time));     // Set simulation time
  }

  if (argEndTime > 0) {
    end_time = argEndTime;
    simulationEndTime = Time(Seconds (argEndTime));     // Set simulation time
  }



  printIt(config);

  string error_str = "";

  if ( (config["link_error"] == true)  && (config["rerouting"] == true) ) {   // If packetdrop on rerouting is enabled
    error_str = "-error";
  }

  AnimationInterface anim = build_network(tcp_version, error_str);   // Network Setup
  auto [source, sinkAddress] = setupDefaultNodeTraffic(tcp_version, error_str);   // Creating Default Sink & Source



  /* --------------------------------------------------------
  //  --  Write Specific Scenario Script Under This  --
  //
  //      !!--!!  START FROM HERE!  !!--!!
  //
  //        ||     ||     ||     ||
  //        ||     ||     ||     ||
  //       \  /   \  /   \  /   \  /
  //        \/     \/     \/     \/
  */


  /////////////////////////////////////
  //  --  Active script part  --
  NS_LOG_INFO ("Creating Applications.");



  //  ---------------------------------------------
  //  --  Dynamic Link Scenario Setup  --
  //  ------------

  if ( config["moving"] == true) {   // If moving satellites is enabled

    if (config["rerouting"] == true) {

      // int shift_first = 30;
      // int shift_second = 340;
      // int shift_second = 420;
      // int shift_second = 500;
      // if (argShift > 0) {
      //   shift_second = argShift;
      // }

      ScheduleDataRateAndDelay(4, shift_first);
      ScheduleDataRateAndDelay(6, shift_first);

      ScheduleDataRateAndDelay(3, shift_second);
      ScheduleDataRateAndDelay(5, shift_second);
    }
    else {
      ScheduleDataRateAndDelay(0);
      ScheduleDataRateAndDelay(1);
      ScheduleDataRateAndDelay(2);
      ScheduleDataRateAndDelay(3);
      ScheduleDataRateAndDelay(4);
      ScheduleDataRateAndDelay(5);
      ScheduleDataRateAndDelay(6);
      ScheduleDataRateAndDelay(7);
    }
  }


  //  ---------------------------------------------
  //  --  Rerouting Scenario Setup  --
  //  ------------

  if ( config["rerouting"] == true) {   // If moving satellites is enabled

    if (config["moving"] == true && argChange <= 0 && argChange2 <= 0) {
      first_reroute_time = first_dynamic_reroute;
      second_reroute_time = second_dynamic_reroute; // After the simulation... set here because we only need one
    }


    // Get node 6 and its ipv4, to prepare changing route
    Ptr<Node> n6 = node.Get (6);   // Grap third node (before forking)
    Ptr<Ipv4> n6ipv4 = n6->GetObject<Ipv4> ();
    // The first interfaceIndex is 0 for loopback, then the first p2p connection is numbered 1, numbered by order of creation.
    uint32_t n4_to_n6_connection = 1;    // Connection index between node 3 & 4
    uint32_t n5_to_n6_connection = 2;    // Connection index between node 3 & 5



    //  ---------------------------------------
    //  --  Simulation Rerouting Scheduling  --

    // SetDown & SetUp opens and closes that specific connection.
    Simulator::Schedule (Seconds (start_time + 0.00001), &Ipv4::SetDown, n6ipv4, n4_to_n6_connection);
    
    Simulator::Schedule (Seconds (first_reroute_time), &Ipv4::SetUp, n6ipv4, n4_to_n6_connection);
    Simulator::Schedule (Seconds (first_reroute_time), &Ipv4::SetDown, n6ipv4, n5_to_n6_connection);

    if ( config["link_error"] == true ) {   // If packetdrop on rerouting is enabled
      linkDrops(6, first_reroute_time, true);
    }

    // Rorouting again
    Simulator::Schedule (Seconds (second_reroute_time), &Ipv4::SetUp, n6ipv4, n5_to_n6_connection);
    Simulator::Schedule (Seconds (second_reroute_time), &Ipv4::SetDown, n6ipv4, n4_to_n6_connection);

    if ( config["link_error"] == true ) {   // If packetdrop on rerouting is enabled
      linkDrops(5, second_reroute_time, true);
      linkDrops(6, second_reroute_time, false);
    }



    //  --  For usage in "prints"  --
    if ( config["link_error"] == true ) {   // If packetdrop on rerouting is enabled
      NS_LOG_INFO ("Rerouting has dropped packets on route change.");
    }
    else {
      NS_LOG_INFO ("Rerouting don't drop packets on route change.");
    }

  }


  //  ---------------------------------------------
  //  --  Congestion Scenario Setup  --
  //  ------------

  if ( config["congestion"] == true) {   // If moving satellites is enabled
    // int new_sink_node = 1;   // Link, not node... see network template scematic
    // int new_sink_link = 1;   // Link, not node... see network template scematic
    // int new_source = 5;   // Link, not node... see network template scematic


    int new_sink_node = 5;   // Link, not node... see network template scematic
    int new_sink_link = 4;   // Link, not node... see network template scematic
    int new_source = 1;   // Link, not node... see network template scematic


    Address sink2 = createSink(new_sink_node, new_sink_link, tcp_version, 1);
    // Address sink2 = createSink(new_sink_node, new_sink_link, tcp_version, 0);
    BulkSendHelper source2 = createSource(new_source, tcp_version, sink2, error_str);

    sourceApp.Start ( Seconds ( source_start_time ) );
    sourceApp.Stop ( simulationEndTime );

  }


  

  /*
  //        /\     /\     /\     /\
  //       /  \   /  \   /  \   /  \
  //        ||     ||     ||     ||
  //        ||     ||     ||     ||
  //
  //      !!--!!  STOP HERE!  !!--!!
  //
  //  --  Write Specific Scenario Script Above This  --
   --------------------------------------------------------*/


  NS_LOG_INFO ("\n_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\nStarting Simulation Using TCP " + tcp_version + ".\n" );
  Simulator::Stop(simulationEndTime);
  Simulator::Run ();  
  Simulator::Destroy ();
  NS_LOG_INFO ("Simulation Done.");


  /////////////////////////////////////////////////
  //  --  Prepare variables for another run  --
  resetContainers();
  resetTracingVars(tcp_version, error_str);
  current_config = default_config;

  return 1;
}


int
main (int argc, char *argv[])
{

  default_config["moving"] = false; 
  default_config["rerouting"] = false; 
  default_config["link_error"] = false; 
  default_config["congestion"] = false;

  ///////////////////////////
  // --  Argument Area


  CommandLine cmd (__FILE__);

  // Setup changes
  cmd.AddValue ("data", "Default Data Rate for Links, Multiplied by 10 if Dynamic Links and Rerouting is Used", argDataRate);
  cmd.AddValue ("scale", "Change the Downscaling Factor of Data Rate", argScale);
  cmd.AddValue ("delay", "Default Delay for Links", argDelay);
  cmd.AddValue ("end", "Change Simulation Runtime", argEndTime);
  cmd.AddValue ("shift", "Change Shift of Second Satellite for Dynamic Links & Rerouting Usage", argShift);
  cmd.AddValue ("change", "Change Timing for Rerouting in the Network", argChange);
  cmd.AddValue ("change2", "Change Timing for Second Rerouting in the Network", argChange2);
  // Scenarios
  cmd.AddValue ("move", "Enable/Disable Moving Satellites Scenario", argMove);
  cmd.AddValue ("con", "Enable/Disable Congestion Scenario", argCon);
  cmd.AddValue ("reroute", "Enable/Disable Rerouting Scenario", argReroute);
  cmd.AddValue ("error", "Enable/Disable Packet Drops on Rerouting Scenario", argError);
  cmd.AddValue ("all", "Enable Complete Combined Scenarios", argAll);
  // cmd.AddValue ("runAll", "Run All Scenarios in One Go", argRunAll);

  cmd.Parse (argc,argv);


  if (argc > 1) {

    if (argDataRate > 0) {  // If Data Rate Argument is Used
      scaledDatarate = argDataRate;
      defaultDatarate = DataRate( std::to_string(argDataRate) + "Mbps");
    }

    if (argScale > 0) {  // If Data Rate Scale Argument is Used
      datarateScale = argScale;
      scaledDatarate = (float)realDataRate / datarateScale;
      defaultDatarate = DataRate( std::to_string(scaledDatarate) + "Mbps");
    }

    if (argDelay > 0) {  // If Delay Argument is Used
      delayMs = argDelay;     // Milliseconds
      delay = delayMs * 0.001;    // Converted to seconds
    }
 
    if (argEndTime > 0) {  // If Simulation Endtime Argument is Used
      end_time = argEndTime;
    }

    if (argShift > 0) {  // If Satellite Shift Argument is Used
      shift_second = argShift;
    }

    if (argChange > 0) {  // If Satellite Shift Argument is Used
      first_reroute_time = argChange;
    }

    if (argChange2 > 0) {  // If Satellite Shift Argument is Used
      second_reroute_time = argChange2;
    }

  }


  //  ----------------------------------
  //  Actual Script Running Section

  conf conf { 
              {"link_error", argError},
              {"rerouting", argReroute},
              {"congestion", argCon}, 
              {"moving", argMove}
            };
  // if (argAll == true) { // Not ready, as they don't have separate filenames yet
  //   conf { 
  //           {"link_error", true},
  //           {"rerouting", true},
  //           {"congestion", true}, 
  //           {"moving", true}
  //         };
  // }


  run("Bbr", conf);
  run("NewReno", conf);
  run("Vegas", conf);

  if (conf["link_error"] == true ) {  // Turn if off, and run without

    conf["link_error"] = false;

    run("Bbr", conf);
    run("NewReno", conf);
    run("Vegas", conf);
  }

}
