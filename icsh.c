/* ICCS227: Project 1: icsh
 * Name: Yuttasart Viratpan
 * StudentID: 6281370
 */

#include <sys/wait.h>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#include "unistd.h"
#define MAX_CMD_BUFFER 255

/*
 * Command enums
 * Mapping the command string to the command enum for functions usage
 */
enum command{
    ECHO=0,
    DOUBLE_BANG,
    LS,
    EXIT,
};

static const char * const command_name[] = {
        [ECHO] = "echo",
        [DOUBLE_BANG] = "!!",
        [LS] = "ls",
        [EXIT] = "exit",
};


//TODO: write "string_process" such that it trims (ignores) all the space in the front (and the last if possible). //Done, but still skeptical of will it break
// Try to also optimize the time complexity.


/* string_process()
 * Input: input[] = The whole string from the input buffer.
 *
 * Functionality: Trims all the white characters that comes before the first char.
 *      Ex: ____echo hello -> echo hello
 *          ____echo hi__ -> echo hi__
 *
 * Return/Output:
 *      - A character pointer (string) that points to the trimmed string.
 */
char* string_process(char input[]){
    int actual_string_length = 0;
    int met_first_char = 0;
    char* string_arr = (char*) malloc(sizeof(char));
    if (string_arr == NULL){
        printf("Memory allocation failed");
        exit(1);
    }
    int string_arr_size = 1;
    int string_arr_pos = 0;
    int extra_newline = 0;
    if (input[strlen(input)-1] != '\n'){ // In script, if End-Of-String doesn't contain newline, add it.
        extra_newline = 1;
    }

    for (int pos = 0; (pos < strlen(input)-1 + extra_newline); pos++){
        if (!isspace(input[pos]) && input[pos] != '\0' && met_first_char == 0){
            met_first_char = 1;
            actual_string_length++;

            if (actual_string_length <= string_arr_size){
                string_arr[string_arr_pos] = input[pos];
                string_arr_pos++;
            }
            else{
                string_arr_size = string_arr_size * 2;
                string_arr = realloc(string_arr, string_arr_size * sizeof(char));
                string_arr[string_arr_pos] = input[pos];
                string_arr_pos++;
            }

        }
        else if (!isspace(input[pos]) && input[pos] != '\0' && met_first_char == 1){
            actual_string_length++;

            if (actual_string_length <= string_arr_size){
                string_arr[string_arr_pos] = input[pos];
                string_arr_pos++;
            }
            else{
                string_arr_size = string_arr_size * 2;
                string_arr = realloc(string_arr, string_arr_size * sizeof(char));
                string_arr[string_arr_pos] = input[pos];
                string_arr_pos++;
            }

        }
        else if (isspace(input[pos]) && input[pos] != '\0' && met_first_char == 1){
            actual_string_length++;

            if (actual_string_length <= string_arr_size){
                string_arr[string_arr_pos] = input[pos];
                string_arr_pos++;
            }
            else{
                string_arr_size = string_arr_size * 2;
                string_arr = realloc(string_arr, string_arr_size * sizeof(char));
                string_arr[string_arr_pos] = input[pos];
                string_arr_pos++;
            }

        }
    }

    if (actual_string_length == 0){
        free(string_arr);
        char* newline = (char*) malloc(sizeof(char));
        newline[0] = '\0';
        return newline;
    }
    else{
        string_arr = realloc(string_arr, (actual_string_length + 1) * sizeof(char));
        string_arr[actual_string_length] = '\0';
        return string_arr;
    }
}



/* command_executor()
 * Input:
 *      - command = The command part of the user inputted buffer.
 *      - argument = The argument part of the user inputted buffer.
 *      - prev_input[] = Array of string, that stores previous command and argument.
 *              prev_input[0] = previous command.
 *              prev_input[1] = previous argument.
 *      - oristr (Original String) = A pointer that holds the address of the trimmed string.
 *              Used to free the memory allocation when exit system calls is called.
 *      - mode = Determinant to update prev_input[], used in double-bang (!!)
 *              0 = allowed updating of prev_input[], 1 otherwise.
 *
 * Functionality:
 *      - Checks "command", whether the command matches any recorded known command (in enum above).
 *      - If match recorded command, does the functionality it supposed to, otherwise output unknown command.
 *      - If the command requires argument, then it uses the argument as well.
 *      - If exit ever reaches, free all the malloc-ed pointers.
 *
 * Return/Output:
 *      - Nothing, it's void
 */
void command_executor(char* command, char* argument, char* prev_input[], char* oristr, FILE* fileptr, int mode){

    if(strcmp(command, command_name[ECHO]) == 0){
        if(mode == 0){
            prev_input[0] = realloc(prev_input[0], strlen(command) * sizeof(char));
            if (prev_input[0] == NULL){
                printf("command memory allocation failed");
                exit(1);
            }
            strcpy(prev_input[0], command);

            if(argument[0] != '\0'){
                prev_input[1] = realloc(prev_input[1], strlen(argument) * sizeof(char));
                if (prev_input[1] == NULL){
                    printf("arguments memory allocation failed");
                    exit(1);
                }
                strcpy(prev_input[1], argument);
            }
            else{
                strcpy(prev_input[1], argument);
            }
        }
        printf("%s\n", argument);
    }

    else if(strcmp(command, command_name[DOUBLE_BANG]) == 0){
        if(*prev_input[0] == '\0'){ // If there is no command, just skip.
            return;
        }
        printf("%s %s %s\n", prev_input[0], prev_input[1], argument);
        if(strlen(argument) != 0){
            prev_input[1] = realloc( prev_input[1], (strlen(prev_input[1]) + strlen(argument) + 1) * sizeof(char));
            strcat(prev_input[1], " ");
            strcat(prev_input[1], argument);
        }
        command_executor(prev_input[0], prev_input[1], NULL, oristr, fileptr, 1);
    }


    else if(strcmp(command, command_name[EXIT]) == 0){
        int status = atoi(argument);
        int truncation = status / 256;
        if (truncation > 0 ){
            status -= truncation * 256;
        }
        printf("Good bye\n");
        free(command);
        free(argument);
        free(oristr);
        free(prev_input[0]);
        free(prev_input[1]);
        if(fileptr != NULL){
            fclose(fileptr);
        }
        exit(status);
    }

    else{
        int status;

        char** argument_list = (char**) malloc(sizeof(char*));

        char* command_file_path = (char*) malloc((10 + strlen(command))*sizeof(char));
        strcat(command_file_path, "/usr/bin/");
        strcat(command_file_path, command);
        strcat(command_file_path, "\0");
        argument_list[0] = command_file_path;

        int argument_pos = 1;

        if(strcmp(argument, "\0") != 0){
            argument_list = realloc(argument_list, 2*sizeof(char*));
            char* each_argument = strtok(argument, " ");
            char* first_argument = (char*) malloc(strlen(each_argument) * sizeof(char));
            strcpy(first_argument, each_argument);
            argument_list[argument_pos] = first_argument;
            argument_pos++;
            each_argument = strtok(NULL, " ");


            while(each_argument != NULL){
                char* next_argument = (char*) malloc(strlen(each_argument) * sizeof(char));
                argument_list = realloc(argument_list, (argument_pos+1) * sizeof(char*));
                strcpy(next_argument, each_argument);
                argument_list[argument_pos] = next_argument;
                argument_pos++;
                each_argument = strtok(NULL, " ");
            }
        }
        else{
            argument_pos = -1;
        }

        pid_t pid = fork();

        if (pid < 0){
            printf("fork() failed\n");
            exit(1);
        }
        else if (pid == 0){
            // child process

            if(execvp(argument_list[0], argument_list) < 0){
                printf("Bad Command\n");
            }
            free(command_file_path);
            for(int i = 0; i <= argument_pos-1 && argument_pos >= 0; i++){
                free(argument_list[i]);
            }
            free(argument_list);
        }
        else{
            waitpid(pid, NULL, 0);
        }

    }
}



/* command_process_unit()
 * Input:
 *      - input = The whole string from the input buffer.
 *      - prev_input[] = Array of string, that stores previous command and argument.
 *              prev_input[0] = previous command.
 *              prev_input[1] = previous argument.
 *      - fileptr = A file pointer, used to close the file to free the memory allocation
 *              when exit system calls is called.
 *
 * Functionality:
 *      - Trims the inputted string by calling string_process()
 *      - Extracts the command string and argument(s) string from the inputted string buffer.
 *      - It then runs command_executor(), passing in the extracted command, arguments and prev_input[] in.
 *      - command_executor() ran in this way is set to mode = 0.
 *      - If "input" is null-terminated after trimming the string, the whole function is returned.
 *      - Free the related pointers after a command is done executing, unless <exit> is called.
 *
 * Return/Output:
 *      - Nothing, it's void
 */
void command_process_unit(char input[], char* prev_input[], FILE* fileptr){
    char* trim_input = string_process(input);
    if (trim_input[0] == '\0'){
        free(trim_input);
        return;
    }

    char* command = (char*) malloc(sizeof(char));
    char* arguments = (char*) malloc(sizeof(char));
    int command_length = 1;
    int argument_length = 1;
    int command_pos = 0;
    int argument_pos = 0;
    int pos = 0;
    int mode = 0; // 0 = command fetching, 1 = argument fetching
    int found_first_space = 0;

    while(trim_input[pos] != '\0'){
        if(trim_input[pos] == ' ' && !found_first_space){
            mode = 1;
            found_first_space = 1;
            pos++;
            continue;
        }

        if(!mode){ // mode == 0
            if(pos == command_length){
                command_length = command_length * 2;
                command = realloc(command, command_length * sizeof(char));
                if (command == NULL){ //In case it failed
                    printf("command memory allocation failed");
                    exit(1);
                }
            }
            command[command_pos] = trim_input[pos];
            pos++;
            command_pos++;
        }
        else{
            if(pos-(command_pos+1) == argument_length){
                argument_length = argument_length * 2;
                arguments = realloc(arguments, argument_length * sizeof(char));
                if (arguments == NULL){ //In case it failed
                    printf("arguments memory allocation failed");
                    exit(1);
                }
            }
            arguments[argument_pos] = trim_input[pos];
            pos++;
            argument_pos++;
        }
    }

    if(argument_pos != 0){
        command = realloc(command, (command_pos + 1) * sizeof(char));
        if (command == NULL){
            printf("command memory allocation failed");
            exit(1);
        }

        arguments = realloc(arguments, (argument_pos + 1) * sizeof(char));
        if (arguments == NULL){
            printf("argument memory allocation failed");
            exit(1);
        }
        command[command_pos] = '\0';
        arguments[argument_pos] = '\0';
    }
    else{
        command = realloc(command, (command_pos + 1) * sizeof(char));
        if (command == NULL){
            printf("command memory allocation failed");
            exit(1);
        }
        command[command_pos] = '\0';
        arguments[0] = '\0';
    }

    command_executor(command, arguments,  prev_input, trim_input, fileptr, 0);
    free(command);
    free(arguments);
    free(trim_input);

}



/* main()
 * Input:
 *      - argc (aka. Argument Count) = Counting the number of arguments when executing program on the terminal
 *      - argv[] (aka. Argument Vector) = The stored string of argument when executing program with inputted
 *              argument on the terminal
 *
 * Functionality:
 *      - Be the main (protagonist) where all functions come together
 *      - If there is no file input, wait for user to input a command and an argument in.
 *      - If there is an argument given in a file script/input, attempt to read the file line-by-line, exit if can't
 *          - Continue to wait for stdin intake, just like when there is no file input
 *      - Process the input by running command_process_unit()
 *      - Keep repeating until exit command is called.
 *
 * Return/Output:
 *      - An integer that supposedly has no use (for now?)
 *
 * File reading: Rob. (2016, August 30). C read file line by line. StackOverFlow.
 *      https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
 *
 * (Build-up) forking and waiting: xor007. (2015, January 11). How to get the return value of a program ran
 *      via calling a member of the exec family of functions?. StackOverFlow.
 *      https://stackoverflow.com/questions/2667095/how-to-get-the-return-value-of-a-program-ran-via-calling-a-member-of-the-exec-fa
 *
 *
 */
int main(int argc, char* argv[]) {
    char buffer[MAX_CMD_BUFFER];
    char* prev_input[2];
    prev_input[0] = (char*) malloc(sizeof(char));
    prev_input[1] = (char*) malloc(sizeof(char));
    printf("Starting IC shell\n");
    if (argc == 2){
        int status;
        pid_t pid = fork();

        if (pid < 0){
            printf("fork() failed\n");
        }
        else if (pid == 0){
            // child process
            FILE *fptr;
            char file_name[3] = {'.', '/', '\0'};
            strcat(file_name, argv[1]);
            fptr = fopen(file_name,"r");
            if(fptr == NULL){
                printf("File not found\n");
                exit(1);
            }
            while(fgets(buffer, MAX_CMD_BUFFER, fptr)) {
                command_process_unit(buffer, prev_input, fptr);
            }
        }
        else{
            wait(&status);
            if(WIFEXITED(status))
                printf("Child's exit code is: %d\n", WEXITSTATUS(status));
            else
                printf("Child did not terminate with exit\n");
        }
    }
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
        command_process_unit(buffer, prev_input, NULL);
    }

    return 0;
}

