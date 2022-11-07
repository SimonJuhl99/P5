## Good examples for understanding **shit**
**Don't waste time on the *Python* files...** you need to understand the *C++* versions to be able to **almost** be able to write scripts using *Python*.  
*Python* is likely a quicker tool, when you already know NS-3, but as a rookie, you'll need to learn how it works in 1,5 languages to utilize it. 

### General Startup
##### From *example/tutorial* folder

#### first.cc
Creates a simple connecten between 2 nodes, using a Point-to-point connection, and sends UDP trafic to a server that repeats it back to the source.

#### second.cc
Builds on *First* and attaches a cluster of several *CSMA* nodes, to the second Point-to-point node of "the original" network.


### TCP Specific
##### From *examples/tcp* folder

#### tcp-pacing.cc
Good examples of tracing TCP issues

#### tcp-bbr-example.cc
The BBR TCP-version, we got told was probably a good version to look at


### NetAnim Examples
##### From *src/netanim/examples* folder
