# TCP in Satellites  
## ComTek 5th Semester Project  
#### Autumn Semester 2022  

Clone this into your NS-3 root directory, probably placed inside other NS-3 directory, in a path similar to ```ns3/ns-allinone-3.36.1/ns-3.36.1/```  
Do this by entering that directory in your *terminal* and write ```git clone https://github.com/SimonJuhl99/P5.git```  
This will give you a new directory like ```ns-3.36.1/P5``` where this Git is inside the *P5* directory.  

### Script Names
All scripts here are the currently used version.  

```rtt.cc``` script for *Round-Trip Time*.  
```ood.cc``` script for *Rerouting*.  
```con.cc``` script for *Congestion*.  

```ro.cc``` script for combined *Round-Trip Time* & *Rerouting*.  
```rc.cc``` script for combined *Round-Trip Time* & *Congestion*.  
```oc.cc``` script for combined *Rerouting* & *Congestion*.  
```roc.cc``` script for **all three** combined.  

#### Version Naming Scheme
Older versions/combinations can be found in the *directories* named the same as each script.  
They are all named after the convention:  
 ```<filename>_n<network size>.v<variant>.cc```  

Example of a 10 node network, with testing a third variant of *Round-Trip Time* behaviour.
```rtt_n10.v3.cc```

### Helpers  
Inside the *Helpers* directory, you can find things to help you do everything easier.

