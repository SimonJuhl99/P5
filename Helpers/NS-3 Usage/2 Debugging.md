## Installation
### Dependancies
NS-3 needs a couple of packages to run... 
- *g++*
- *python3*
- *cmake*
- *ninja-build*
- *git*
- *ccache* (Not needed, but recommended)

If on a destribution using *apt* package manager (like Ubuntu), just use the command ```apt install g++ python3 cmake ninja-build git ccache``` 

Packages might be named slightly different on other package managers, for example *ninja-build* is just called *ninja* on *Pamac* package manager (used by Manjaro).  

#### Netanim Dependancies
Several QT5 (GUI framework) packages.

Using *apt* run: ```apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools```

Again... might be called something different elsewhere, but Manjaro already has these installed by default. *I think*... 

## Missing Terminal Output
You've likely gotten NS-3 set to *Optimized Build* configuration.  

Run
```BASH
ns configure --build-profile=debug --out=build/debug --enable-examples --enable-tests
```

This should get your terminal outputs back.

This happend to me while trying to "autofind" a file in the *scratch* directory, but didn't later when I tried the same... not sure what happened, but... just be aware some shit can happen at random.  

##### Don't want terminal output?
Run
```BASH
ns configure --build-profile=optimized --out=build/optimized --enable-examples --enable-tests
```