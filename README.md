# Simulated_Process_Manager
Starts a specified sequence of processes and monitors their status in the linux enviorment
## Files
The macD.c file is the main file that contains the logic for the program.\
makefile is a file used to compile the program, see "How To Use"\
the README file contains useful information about the software.
## Description
the exectutable checks for the -i flag followed by a file path.\
the program will then open the file, if the path provided is a valid file,\
and create a new process for each line in the file.\
each new process created will be created to be the process indicated by the line in the file.\
If the line in the file is not a valid process then the created process will terminate.\
every 5 seconds the program will display a "normal report" in which the cpu usage, as a percent,\
and memory usage, in MB, will be displayed.\
if the program receives a SIGINT it will terminate itself and all children.\
if the provided file has 'timelimit' specified as the first line then the program will terminate\
after the specified time. For example the line "timelimit 20" \
will terminate the program and all children after 20 seconds.\
If no time limit is present then the program will run untill all processes exit.
## How To Use
First type the command "make" in order to compile the executable.\
Next call the function with the command "./macD -i <filepath>"\
the program will create the processes specified in the file and then terminate.\
Once you're done run "make clean" to remove the executable file.\
to terminate the program while it is running click ctrl+C.
