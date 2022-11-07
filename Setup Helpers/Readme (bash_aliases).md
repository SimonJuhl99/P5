## Readme for *bash_aliases*  
This file contains shortcuts (aliases and functions) for your terminal, to make NS-3 easier to interact with.

#### Installation
1. Copy the file to your Linux users *Home* folder.  
2. In your *Home* directory, open the *.bashrc* file (it's hidden), and make it include the *bash_aliases* file, by inserting the code below at the bottom of the *.bashrc* file.  
If you already have a similar file with other functions or aliases in, just copy the *bash_aliases* stuff to that file, instead. 
 ``` 
if [ -f ~/bash_aliases ]; then  
    . ~/bash_aliases  
fi
```  
 3. Edit the *ns3path* variable in the beginning of the *ns* function, to your personal local path of NS-3  


 Now you can interact with NS-3 easier by just writing *ns* in your terminal.


#### Usage  
You can now run NS-3 scripts, without being in the folder, and don't have to call the *run* argument, unless you need more arguements than *run* and the path of the script to run.  
```ns <path/script>```: runs the script at the path, and throws the output files into an *output_files* folder created at the NS-3 root folder.  

 ```ns dir```: directly enters your NS-3 directory, no matter where "you are".  

 ```ns help```: calls the NS-3 help function.  

If other NS-3 functionality is needed, just write the the full arguements, as you normally would... Still don't need to be in the NS-3 directory though.  
Like ```ns run <path/script> <argument1> <argument2>```
