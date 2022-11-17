# Readme for *bash_aliases*  
This file contains shortcuts (aliases and functions) for your terminal, to make NS-3 easier to interact with.  

All functionality here is on *absolute paths*, so you'll never have to think about the "placement" of your terminal.  

## Installation
1. Copy the file to your Linux users *Home* folder.  
2. In your *Home* directory, open the *.bashrc* file (it's hidden), and make it include the *bash_aliases* file, by inserting the code below at the bottom of the *.bashrc* file.  
If you already have a similar file with other functions or aliases in, just copy the *bash_aliases* stuff to that file, instead. 
 ``` 
if [ -f ~/bash_aliases ]; then  
    . ~/bash_aliases  
fi
```  

3. Edit the `explorer` variable to your personal explorer program.  

4. Edit the `ns3_root` variable in the beginning of the script, to your personal local path of NS-3's *allinone* directory.    



Now you can interact with NS-3 easier by using the *ns* and *p5* commands in your terminal.


## Usage  

### NS-3 Helper
Bound to the command *ns*.

You can now run NS-3 scripts, without being in the folder, and don't have to call the *run* argument, unless you need more arguements than *run* and the path of the script to run.  

Valid arguments are:  
```<script name>```  
```run <script name>``` - same as the one above, just more keystrokes.
```dir```  
```exp```  
```help```  
```<Normal NS-3 arguments>``` - as many as wanted  

If you want arguments in the script to run, NS-3 already wants those arguments in quotation marks, the same applies here.  

#### Argument Explanations
```ns <path/script>``` or just ```ns <script>```  
Runs the script at the path, and throws the output files into an *output_files* folder created at the NS-3 root folder.  

```ns "<path/script> --<argument_name>=<arguement_value>"```  
 If arguments are needed for the script, call the script and arguements using quotation-marks. Otherwise same as above.

 ```ns dir```  
 Directly enters your NS-3 directory.  

 ```ns exp```  
 Directly enters your NS-3 directory in your explorer.  

 ```ns help```  
 Calls the NS-3 help function.  

If other NS-3 functionality is needed, just write the the full arguments, as you normally would using NS-3... Still don't need to be in the NS-3 directory though.  

Example ```ns configure <argument1> <argument2>```  

### P5 Helper
Bound to the command *p5*.

If ```p5``` is called without further arguments, it will `cd` to your P5 directory.  

Valid arguments are:  
```git <optional arguments>```  
```exp```

#### Argument Explanations
```exp```  
Will open your P5 directory in your explorer.  

```git```  
If called without further arguments, will go through all the motions of a `git commit/push`, and ask for a message for your commit.  

If entered with more arguments than just `git`, it will just do your normal git commands that gets inserted.  
*Example:* `p5 git commit -m "Deleted the entire repository"` will make a *commit* with that message, as it normally would.