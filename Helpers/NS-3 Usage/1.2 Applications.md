### UDP Echo-relay Connection
From ```example/tutorial/first.cc```.  
Sets up a server, to just repeat and return, what is being sent to it, and a client sending it some data.  

##### UdpEchoServerHelper
To set up an *Echo Server* that just repeats what is being sent to it, back to the source.

```C
UdpEchoServerHelper echoServer (9);

ApplicationContainer serverApps = echoServer.Install (nodes.Get (1));
serverApps.Start (Seconds (1.0));
serverApps.Stop (Seconds (10.0));
```
This creates an *Echo Server*, using *port* 9.  
Installs it on an *ApplicationContainer* on the second node (using ```nodes.Get(1)```)  
And tells it to listen from 1 "simulation second" after the simulation starts, until 10 "simulation seconds" after it has been started.

##### UdpEchoClientHelper
To make a *client* sending some data, to the server.
```C
UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9);
echoClient.SetAttribute ("MaxPackets", UintegerValue (3));
echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
clientApps.Start (Seconds (2.0));
clientApps.Stop (Seconds (10.0));
```
This creates an *Client*, using *port* 9 (same as server). 

Tells it to send 3 packets, with an interval of 1 second and a packetsize of 1024.  

Installs it on a different *ApplicationContainer* than the server container, on the first node (using ```nodes.Get(0)```)  
And tells it to start it's packet transfer from 2 "simulation seconds" after the simulation starts, until a max of 10 "simulation seconds" after it has been started.
