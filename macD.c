/*
 * macD
 * written by: Nathan Koop
 *
 * description:
 *     the executable detects a -i flag to find the file to open.
 *     for each line in the file the executable will create a new process
 *     where the process created is specified by the file.
 *     The program will then send a normal report every 5 seconds
 *     in which the cpu usage, as a percent, and memory usage, in MB
 *     is displayed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <limits.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include "macD.h"

int MAX_ARG_LENGTH = 1000;
int MAX_PROCESSES = 10;
int MAX_SEGMENT_LENGTH = 100;
double TARGET_TIME = -1;
int KILL_STATE = -1;
double START_TIME = -1;

/*
 * main
 * description:
 *     called when the program is executed.
 *     checks for the -i flag and opens the following file.
 * parameters:
 *     argc: number of command line arguments
 *     argv: array of strings containing the command line arguments
 */
int main(int argc, char *argv[])
{
	int opt;

	START_TIME = 0;
	while ((opt = getopt(argc, argv, "i:")) != -1) {
		if (opt == 'i') {
			if (optarg == NULL) {
				printf("option requires an argument --i");
			} else {
				int *pids = read_file(optarg);

				if (pids == NULL)
					return 1;
				START_TIME = time(NULL);
				register_handler();
				periodic_reports(pids);
			}
		}
	}
}

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
int get_num_args(char *line)
{
	int len = strlen(line);
	int count = 0;

	for (int i = 0; i < len; i++) {
		if (line[i] == ' ')
			count++;
	}
	return count+1;
}

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
char **get_args(char *line)
{
	int args = get_num_args(line);
	char **return_array = malloc(sizeof(char *)*args);
	char *token = strtok(line, " ");
	int index = 0;

	while (token != NULL) {
		return_array[index] = token;
		token = strtok(NULL, " ");
		index++;
	}
	return return_array;
}

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
void create_process(char *process_line, int *out_pid)
{
	int pid = fork();

	if (pid == -1)
		exit(1);
	if (pid == 0) {
		char **args = get_args(process_line);

		if (args[0] == NULL)
			exit(1);
		execvp(args[0], args);
		*out_pid = -1;
		exit(errno);
	} else {
		usleep(100000);
		int result = waitpid(pid, NULL, WNOHANG);

		if (*out_pid != -1 && result == 0)
			*out_pid = pid;
	}
}

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
char *read_next_line(FILE *fptr)
{
	char *scanner = malloc(1);
	char *line = malloc(PATH_MAX+MAX_ARG_LENGTH);
	int index = 0;
	int extensions = 0;
	int read_items = fread(scanner, 1, 1, fptr);

	if (read_items != 1) {
		free(scanner);
		free(line);
		return NULL;
	}
	while (*scanner != '\n' && read_items == 1) {
		line[index] = *scanner;
		index++;
		if (index >= PATH_MAX + (MAX_ARG_LENGTH * (extensions+1))) {
			//grow the array.
			extensions++;
			char *temp = line;

			line = malloc(PATH_MAX + (MAX_ARG_LENGTH*(extensions+1)));
			strncpy(line, temp, PATH_MAX+MAX_ARG_LENGTH*(extensions)-1);
			free(temp);
		}
		read_items = fread(scanner, 1, 1, fptr);
	}
	free(scanner);
	line[index] = '\0';
	return line;
}

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
int convert_str_to_int(char *str)
{
	char c = str[0];
	int index = 0;
	int total = 0;

	while (c != '\0') {
		int add = -1;

		switch (c) {
		case '0':
			add = 0;
			break;
		case '1':
			add = 1;
			break;
		case '2':
			add = 2;
			break;
		case '3':
			add = 3;
			break;
		case '4':
			add = 4;
			break;
		case '5':
			add = 5;
			break;
		case '6':
			add = 6;
			break;
		case '7':
			add = 7;
			break;
		case '8':
			add = 8;
			break;
		case '9':
			add = 9;
			break;
		}
		if (add == -1)
			return -1; //str is not numeric
		total *= 10;
		total += add;
		index++;
		c = str[index];
	}
	return total;
}

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
int read_timer(char *line)
{
	if (line == NULL)
		return -1;
	if (line[0] == '\0')
		return -1;
	char *token = strtok(line, " ");

	if (strcmp(token, "timelimit") == 0) {
		token = strtok(NULL, " ");
		if (token == NULL)
			return -1;
		int timer = convert_str_to_int(token);

		return timer;
	}
	return -1;
}

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
char *get_month(struct tm current_time)
{
	int month_num = current_time.tm_mon;
	char *month = "";

	switch (month_num) {
	case 0:
		month = "Jan";
		break;
	case 1:
		month = "Feb";
		break;
	case 2:
		month = "Mar";
		break;
	case 3:
		month = "Apr";
		break;
	case 4:
		month = "May";
		break;
	case 5:
		month = "June";
		break;
	case 6:
		month = "July";
		break;
	case 7:
		month = "Aug";
		break;
	case 8:
		month = "Sept";
		break;
	case 9:
		month = "Oct";
		break;
	case 10:
		month = "Nov";
		break;
	case 11:
		month = "Dec";
		break;
	}
	return month;
}

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
char *get_day_of_week(struct tm current_time)
{
	int day = current_time.tm_wday;
	char *day_of_week = "";

	switch (day) {
	case 0:
		day_of_week = "Sun";
		break;
	case 1:
		day_of_week = "Mon";
		break;
	case 2:
		day_of_week = "Tue";
		break;
	case 3:
		day_of_week = "Wed";
		break;
	case 4:
		day_of_week = "Thu";
		break;
	case 5:
		day_of_week = "Fri";
		break;
	case 6:
		day_of_week = "Sat";
		break;
	}
	return day_of_week;
}

/*
 * display_date
 * description:
 *     displays the current time in the following format:
 *     [day_of_week], [month] [day], [year] [hour]:[min]:[sec] [AM/PM]
 */
void display_date(void)
{
	time_t t = time(NULL);
	struct tm current_time = *localtime(&t);
	char *wkday = get_day_of_week(current_time);
	char *month = get_month(current_time);
	int date = current_time.tm_mday;
	int year = 1900 + current_time.tm_year;
	int hour = current_time.tm_hour;
	char *xm = "AM";

	if (hour >= 12) {
		xm = "PM";
		hour -= 12;
	}
	if (hour == 0)
		hour = 12;
	int min = current_time.tm_min;
	int sec = current_time.tm_sec;

	printf("%s, %s %d, %d %d:%d:%d %s\n", wkday, month, date, year, hour, min, sec, xm);
}

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
int *read_file(char *file_path)
{
	FILE *fptr = fopen(file_path, "r");

	if (fptr == NULL) {
		fprintf(stderr, "macD: %s not found", file_path);
		return NULL;
	}
	//read file for processes
	printf("%s", "Starting report, ");
	display_date();
	char *line = read_next_line(fptr);

	TARGET_TIME = read_timer(line);
	if (TARGET_TIME != -1) {
		free(line);
		line = read_next_line(fptr);
	}
	int *pids = malloc(sizeof(int)*MAX_PROCESSES);
	int index = 0;
	int line_number = 0;
	int len_pids = MAX_PROCESSES;
	int *out_pid = malloc(sizeof(int *));

	while (line != NULL) {
		*out_pid = -2;
		if (line[0] != '\0')
			create_process(line, out_pid);
		if (*out_pid >= 0) {
			pids[index] = *out_pid;
			char *path = strtok(line, " ");

			printf("[%d] %s, started successfully (pid: %d)\n", line_number, path, *out_pid);
			index++;
		} else {
			if (line[0] == '\0') {
				printf("[%d] badprogram , failed to start\n", line_number);
			} else {
				char *path = strtok(line, " ");

				printf("[%d] badprogram %s, failed to start\n", line_number, path);
			}
		}
		if (index >= len_pids - 1) {
			//increase size of pids
			len_pids = len_pids + MAX_PROCESSES;
			int *temp = pids;

			pids = malloc(sizeof(int)*len_pids);
			for (int i = 0; i < index; i++)
				pids[i] = temp[i];
			free(temp);
		}
		line_number++;
		free(line);
		line = read_next_line(fptr);
	}
	free(line);
	free(out_pid);
	fclose(fptr);
	pids[index] = -1;//to indicate end of array
	return pids;
}

/*
 * get_num_digits
 * description:
 *     counts the number of digits in i.
 * parameters:
 *    i: integer to count the digits of.
 * returns:
 *     the number of digits in i.
 */
int get_num_digits(int i)
{
	int digits = 0;

	if (i < 0)
		i = i*-1;
	if (i == 0)
		return 1;
	while (i >= 1) {
		digits += 1;
		i = i / 10;
	}
	return digits;
}

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
char *convert_int_to_string(int i)
{
	if (i == 0)
		return "0";
	char *str = malloc(get_num_digits(i));
	int index = 0;

	while (i > 0) {
		int d = i % 10;

		str[index] = '0' + d;
		i -= d;
		i = i / 10;
		index++;
	}
	//reverse string
	int lptr = 0;
	int rptr = index-1;

	while (lptr < rptr) {
		char temp = str[rptr];

		str[rptr] = str[lptr];
		str[lptr] = temp;
		lptr++;
		rptr--;
	}
	return str;
}

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
int read_until_space(FILE *fptr)
{
	char *c = malloc(1);
	int read = fread(c, 1, 1, fptr);

	while (*c != ' ' && read == 1)
		read = fread(c, 1, 1, fptr);
	free(c);
	return read;
}

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
char *get_next_segment(FILE *fptr)
{
	char *segment = malloc(MAX_SEGMENT_LENGTH);
	int seg_size = MAX_SEGMENT_LENGTH;
	int index = 0;
	char *c = malloc(1);
	int read = fread(c, 1, 1, fptr);

	while (*c != ' ' && read == 1) {
		segment[index] = *c;
		index++;
		if (index >= seg_size) {
			//adjust size
			char *temp = segment;

			segment = malloc(seg_size+MAX_SEGMENT_LENGTH);
			seg_size += MAX_SEGMENT_LENGTH;
			for (int i = 0; i < index; i++)
				segment[i] = temp[i];
			free(temp);
		}
		read = fread(c, 1, 1, fptr);
	}
	free(c);
	segment[index] = '\0';
	return segment;
}

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
int get_cpu_usage(int pid)
{
	char *path = malloc(12+get_num_digits(pid));

	path[0] = '\0';
	char *part1 = "/proc/";
	char *part2 = "/stat";

	strncat(path, part1, 6);
	char *str_pid = convert_int_to_string(pid);

	strncat(path, str_pid, get_num_digits(pid));
	free(str_pid);
	strncat(path, part2, 5);
	FILE *fptr = fopen(path, "r");

	free(path);
	if (fptr == NULL) {
		fclose(fptr);
		return -1;
	}
	//Read until the usertime section of the file
	// which is the 14th segment (after 13 spaces).
	for (int i = 0; i < 13; i++) {
		int r = read_until_space(fptr);

		if (r == 0) {
			fclose(fptr);
			return -1; //file not long enough
		}
	}
	//get user time
	char *s_user = get_next_segment(fptr);
	int user_time = atoi(s_user);
	//get kernal time
	char *s_kernal = get_next_segment(fptr);
	int kernal_time = atoi(s_kernal);

	free(s_user);
	free(s_kernal);
	fclose(fptr);
	return user_time + kernal_time;
}

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
int get_mem_usage(int pid)
{
	char *path = malloc(13+get_num_digits(pid));

	path[0] = '\0';
	char *part1 = "/proc/";
	char *part2 = "/statm";

	strncat(path, part1, 6);
	char *str_pid = convert_int_to_string(pid);

	strncat(path, str_pid, get_num_digits(pid));
	free(str_pid);
	strncat(path, part2, 6);
	FILE *fptr = fopen(path, "r");

	free(path);
	if (fptr == NULL) {
		fclose(fptr);
		return -1;
	}
	//read contents of file, summing up all numbers as they are read.
	int sum = 0;
	char *segment = get_next_segment(fptr);

	while (strlen(segment) != 0) {
		sum += atoi(segment);
		free(segment);
		segment = get_next_segment(fptr);
	}
	free(segment);
	fclose(fptr);
	return sum/1024;
}

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
int *initialize_cpu_counters(int *pids, int num_processes)
{
	int *counters = malloc(sizeof(int)*num_processes);

	for (int i = 0; i < num_processes; i++) {
		//get initial cpu usage
		counters[i] = get_cpu_usage(pids[i]);
		if (counters[i] < 0)
			counters[i] = 0;
	}
	return counters;
}

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
int len_pids(int *pids)
{
	if (pids == NULL)
		return 0;
	int len = 0;

	while (pids[len] != -1)
		len++;
	return len;
}

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
void terminate_program(int *pids, double elapsed_time)
{
	printf("%s", "Terminating, ");
	display_date();
	int pid = pids[0];
	int index = 0;

	while (pid != -1) {
		//check if process is still active
		pid_t result = waitpid(pid, NULL, WNOHANG);

		if (result == 0) {
			printf("[%d] %s\n", index, "Terminated");
			kill(pid, SIGKILL);
		} else {
			printf("[%d] %s\n", index, "Exited");
		}
		index++;
		pid = pids[index];
	}
	free(pids);
	printf("Exiting (total time: %d seconds)\n", (int)(elapsed_time/1));
	exit(0);
}

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
int check_timer(double current_time)
{
	if (current_time - START_TIME >= TARGET_TIME && TARGET_TIME != -1)
		return 1;
	if (KILL_STATE == 1)
		return 1;
	return 0;
}

/*
 * display_proc_state
 * description:
 *     displays the cpu usage and mem usage of a process.
 * parameters:
 *     index: the index of the process in the pids array.
 *     cpu: the percentage of time this process has spent on the cpu.
 *     mem: the amount of memory, in MB, used by the process.
 */
void display_proc_state(int index, int cpu, int mem)
{
	char percent = '%';

	printf("[%d] Running, cpu usage: %d%c,", index, cpu, percent);
	printf(" mem usage: %d MB\n", mem);
}

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
void periodic_reports(int *pids)
{
	int *counters = initialize_cpu_counters(pids, len_pids(pids));
	int full_cpu_increase = 5*sysconf(_SC_CLK_TCK);

	while (1) {
		int done = 1;
		int index = 0;

		printf("%s\n", "...");
		printf("%s", "Normal report, ");
		display_date();
		while (pids[index] != -1) {
			pid_t result = waitpid(pids[index], NULL, WNOHANG);

			if (result == 0) {
				int cpu = get_cpu_usage(pids[index]);
				int cpu_percent = ((cpu - counters[index])*100);
				int mem = get_mem_usage(pids[index]);

				cpu_percent = cpu_percent/full_cpu_increase;
				counters[index] = cpu;
				done = 0;
				display_proc_state(index, cpu_percent, mem);
			} else {
				printf("[%d] Exited\n", index);
			}
			index++;
		}
		if (done == 1) {
			double current_time = time(NULL);
			int total_time = (int)(current_time - START_TIME);

			printf("Exiting (total time: %d seconds)\n...\n", total_time);
			exit(0);
		}
		printf("%s\n", "...");
		double t = time(NULL);
		double current_time = t;

		while (current_time - t < 5) {
			current_time = time(NULL);
			if (check_timer(current_time) == 1)
				terminate_program(pids, current_time - START_TIME);
		}
	}
}

/*
 * sig_handler
 * description:
 *     called when SIGINT is passed to the process
 *     sets KILL_STATE to 1 which tells the process to terminate
 *     itself and its children.
 * parameters:
 *     sig: the id of the signal passed to the process
 */
void sig_handler(int sig)
{
	printf("Signal Received - ");
	KILL_STATE = 1;
}

/*
 * register_handler
 * description:
 *     registers this program to react to the SIGINT signal
 */
void register_handler(void)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = 0;
	sa.sa_handler = sig_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		err(1, "sigaction error");
}
