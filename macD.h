/*
 * get_num_args
 * description:
 *     counts the number of arguments in a given line of text
 *     assuming this line is from the process list file.
 *     counts the number of spaces in the line and uses this
 *     to determin the number of args.
 * parameters:
 *     line: a string of the command to count the args of
 * pre-condition:
 *    line is initialized.
 * returns:
 *     number of args in line.
 */
int get_num_args(char *line);

/*
 * get_args
 * description:
 *     creates a list of strings containing the arguments
 *     of the given line, which represents a process from the process list file.
 * parameters:
 *     line: a string of the command to get the args of
 * pre-condition:
 *     line is initialized.
 * returns:
 *     list of strings each element is an argument.
 */
char **get_args(char *line);

/*
 * create_process
 * description:
 *     creates a new process using the fork function.
 *     changes the process to the process indicated by process_path.
 *     terminates any failed forks.
 * parameters:
 *     process_path: string containing path to the process to create.
 * pre-conditions:
 *     process_path is initialized.
 */
void create_process(char *process_line, int *out_pid);

/*
 * read_next_line
 * description:
 *     reads the next line in the given file object
 * parameters:
 *     fptr: pointer to the FILE object to read from.
 * pre-conditions:
 *     fptr is initialized and is a valid FILE object
 * returns:
 *     char array, string, containing the next line of the file.
 */
char *read_next_line(FILE *fptr);

/*
 * convert_str_to_int
 * description:
 *     converts the given string to its integer form
 * parameters:
 *     str: the string to convert to an integer.
 * pre-condition:
 *     str is initialized.
 * returns:
 *     integer form of str if str is numeric.
 *     -1 otherwise.
 */
int convert_str_to_int(char *str);

/*
 * read_timer
 * description:
 *     checks if the given line indicates a timer.
 *     if the line is in the form "timelimit [integer]" then
 *     [integer] is returned.
 * parameters:
 *     line: line to check if its a timer.
 * returns:
 *     the value of the time limit if the given line is a timer.
 *     -1 otherwise.
 */
int read_timer(char *line);

/*
 * get_month
 * description:
 *     converts the month in current time to
 *     its string representation.
 * parameters:
 *     current_time: tm structure containing the current time.
 * returns:
 *     string representation of the month.
 */
char *get_month(struct tm current_time);

/*
 * get_day_of_week
 * description:
 *     converts the day in current time to
 *     its string representaion.
 * parameters:
 *     current_time: tm structure containing the current time.
 * returns:
 *     string representation of the day.
 */
char *get_day_of_week(struct tm current_time);

/*
 * display_date
 * description:
 *     displays the current time in the following format:
 *     [day_of_week], [month] [day], [year] [hour]:[min]:[sec] [AM/PM]
 */
void display_date(void);

/*
 * read_file
 * description:
 *     reads all lines in the given file.
 *     creates a process for each line in the file where the line
 *     indicates what process to create.
 * parameters:
 *     file_path: string of the path to the file to read.
 * pre-conditions:
 *     file_path is initialized.
 */
int *read_file(char *file_path);

/*
 * get_num_digits
 * description:
 *     counts the number of digits in i.
 * parameters:
 *    i: integer to count the digits of.
 * returns:
 *     the number of digits in i.
 */
int get_num_digits(int i);

/*
 * convert_int_to_string
 * description:
 *     converts the given integer to a string.
 * parameters:
 *     i: integer to convert to string.
 * pre-condition:
 *     i>=0
 * returns:
 *     string representaion of i.
 */
char *convert_int_to_string(int i);

/*
 * read_until_space
 * description:
 *     reads through the given file until it reaches a space
 *     or the end of a file.
 * parameters:
 *     fptr: FILE pointer to be read through.
 * pre-condition:
 *     fptr is initalized.
 * post-condition:
 *     the read from point in fptr is moved such that the next character read
 *     will be the character immediately after the space, or EOF if no space was found.
 * returns:
 *     1 if a space was found, meaning fptr not at EOF.
 *     0 if no space was found, meaning fptr is at EOF.
 */
int read_until_space(FILE *fptr);

/*
 * get_next_segment
 * description:
 *     reads fptr until the next space character or EOF.
 *     returns all characters between read before finding a space.
 * parameters:
 *     fptr: FILE pointer to be read through.
 * pre-condition:
 *     fptr is initalized.
 * post-condition:
 *     the read from point in fptr is moved such that the next character read
 *     will be the character immediately after the space, or EOF if no space was found.
 * returns:
 *     char array of all characters read before finding a space or EOF.
 */
char *get_next_segment(FILE *fptr);

/*
 * get_cpu_usage
 * description:
 *     computes the total amount of time the process has spent
 *     on the cpu, measured in clock ticks by
 *     reading /proc/[pid]/stat to find the user time and kernal time.
 * parameters:
 *     pid: the process id of the process to compute the cpu usage for.
 * returns:
 *     the number of ticks the process has been on the cpu for
 *     or -1 if no such process with pid exists.
 */
int get_cpu_usage(int pid);

/*
 * get_mem_usage
 * description:
 *     computes the amount of memory used by the process
 * parameters:
 *     pid: id of the process to compute memory usage of.
 * returns:
 *     the memory usage, in MB, of the given process
 *     or -1 if no such process exists.
 */
int get_mem_usage(int pid);

/*
 * initialize_cpu_counters
 * description:
 *     creates an array with a size equal to the amount of processes.
 *     this array will contain the cpu usage of each process from the
 *     previous reporting cycle.
 * parameters:
 *     pids: list of process ids
 *     num_processes: the length of pids, the number of processes.
 * pre-conditions:
 *     pids is initialized.
 *     num_processes is the length of pids.
 * returns:
 *     list of integers containing the number of ticks each process
 *     has run on the cpu for. This list is in the same order as pids.
 *     so the processes at pids[i] will have run for counters[i] ticks.
 */
int *initialize_cpu_counters(int *pids, int num_processes);

/*
 * len_pids
 * description:
 *     counts the number of elements in the pids list.
 *     this is calculated by searching the list until -1 is found.
 * parameters:
 *     pids: list of pids to calculate length of.
 * pre-conditions:
 *     pids is initialized.
 *     pids is terminated by a -1 element.
 * returns:
 *     the length of pids.
 */
int len_pids(int *pids);

/*
 * terminate_program
 * description:
 *     terminates this process and all children processes.
 *     It then displays the final status for all children
 *     and the total runtime of the process.
 * parameters:
 *     pids: list of all children process ids
 *     elapsed_time: the time the program has been running for.
 * pre-conditions:
 *     pids is initalized.
 * post-conditions:
 *     all processes with pid in pids list will be killed.
 *     this program will terminate.
 */
void terminate_program(int *pids, double elapsed_time);

/*
 * check_timer
 * description:
 *     checks if the program has run longer than the TARGET_TIME
 *     or if KILL_STATE == 1.
 *     if either of these condtions are true returns 1.
 * parameters:
 *     current_time: the current time. Created by calling time(NULL)
 * returns:
 *     1 if program has run for TARGET_TIME or KILL_STATE = 1.
 *     0 otherwise.
 */
int check_timer(double current_time);

/*
 * display_proc_state
 * description:
 *     displays the cpu usage and mem usage of a process.
 * parameters:
 *     index: the index of the process in the pids array.
 *     cpu: the percentage of time this process has spent on the cpu.
 *     mem: the amount of memory, in MB, used by the process.
 */
void display_proc_state(int index, int cpu, int mem);

/*
 * periodic_reports
 * description:
 *     displays the status of all processes every 5 seconds.
 * parameters:
 *     pids: list of process ids
 * pre-conditions:
 *     pids is initialized.
 *     pids is terminated by a -1 element.
 */
void periodic_reports(int *pids);

/*
 * sig_handler
 * description:
 *     called when SIGINT is passed to the process
 *     sets KILL_STATE to 1 which tells the process to terminate
 *     itself and its children.
 * parameters:
 *     sig: the id of the signal passed to the process
 */
void sig_handler(int sig);

/*
 * register_handler
 * description:
 *     registers this program to react to the SIGINT signal
 */
void register_handler(void);
