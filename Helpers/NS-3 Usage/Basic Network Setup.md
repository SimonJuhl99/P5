### Basic Network Setup

##### NodeContainer (*Network Module*)
Used to create and hold each *node* needed in a simulation.  
```C
NodeContainer nodes;
nodes.Create (2);
```
This creates 2 nodes to build a connection between.  

Nodes represent the physical nodes, and can contain several "network adapters", still being just one node.  

##### PointToPointHelper (*PointToPoint Module*)
To set the connection capabilities, like datarate and delays.
```C
PointToPointHelper pointToPoint;
pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
```
Sets the *datarate* of the devices to 5Mbps, and the *delay* between them to 2ms.

##### NetDeviceContainer (*Network Module*)
A container to contain the devices (Netcards) and "know" their available connections.

```C
NetDeviceContainer devices;
devices = pointToPoint.Install (nodes);
```
Creates the network adapters, with a point-to-point connection between them, in each of the two nodes created.  

**Now you have a basic "physical" network, containing 2 nodes, with a network adapter each, and a connection between them**

### Internet Protocols & Tracing
To enable the *Internet* stack functionalities, 

##### InternetStackHelper (*Internet Module*)
Used to add *Internet Stack* functionalities to your setup, adding *IP*, *TCP* and *UDP* functionality.  

Also the possibility of tracing the traffic, using *pcap* and *ascii*.

```C
InternetStackHelper stack;
stack.Install (nodes);
```
Don't need to do more than this, as it just enables more functionalities for the *nodes*


##### Ipv4AddressHelper (*Internet Module*)
To set IPv4 address parameters.

```C
Ipv4AddressHelper address;
address.SetBase ("10.1.1.0", "255.255.255.0");
```
Tells NS-3 to create IP addresses from the starting point of 10.1.1.1 onwards, using the subnet mask 255.255.255.0.

##### Ipv4InterfaceContainer (*Internet Module*)
Adds the IPv4 interface to the network devices already created, so they can communicate using internet protocols.

```C
Ipv4InterfaceContainer interfaces = address.Assign (devices);
```

**Now you have a functioning connection between the 2 nodes, using internet protocols.**  
From here you can start using the functionalities of the *Application Module* to set up scenarios to simulate and get data on.


##### *example/tutorial/second.cc* adds a second cluster (using CSMA) and connects them to a Point-to-point cluster like this one... 