/* ICCS227: Project 1: icsh
 * Name: Yuttasart Viratpan
 * StudentID: 6281370
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "ctype.h"
#define MAX_CMD_BUFFER 255

/*
 * Command enums
 * Mapping the command string to the command enum for functions usage
 */
enum command{
    ECHO=0,
    DOUBLE_BANG,
    EXIT,
};

static const char * const command_name[] = {
        [ECHO] = "echo",
        [DOUBLE_BANG] = "!!",
        [EXIT] = "exit",
};


//TODO: write "string_process" such that it trims (ignores) all the space in the front (and the last if possible). //Done, but still skeptical of will it break
// Try to also optimize the time complexity.


/* string_process()
 * Input: input[] = the whole string from the input buffer.
 *
 * Functionality: Trims all the white characters that comes before the first char.
 *      Ex: ____echo hello -> echo hello
 *          ____echo hi__ -> echo hi__
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

    for (int pos = 0; (pos < strlen(input)-1); pos++){
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
 *      - command = the command part of the user inputted buffer.
 *      - argument = the argument part of the user inputted buffer.
 *      - prev_input[] = array of string, that stores previous command and argument.
 *              prev_input[0] = previous command.
 *              prev_input[1] = previous argument.
 *      - mode = Determinant to update prev_input[], used in double-bang (!!)
 *              0 = allowed updating of prev_input[], 1 otherwise.
 *
 * Functionality:
 *      - Checks "command", whether the command matches any recorded known command (in enum above).
 *      - If match recorded command, does the functionality it supposed to, otherwise output unknown command.
 *      - If the command requires argument, then it uses the argument as well.
 *
 */
void command_executor(char* command, char* argument, char* prev_input[], int mode){
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
        if(*prev_input[0] == '\0'){
            return;
        }
        printf("%s %s\n", prev_input[0], prev_input[1]);
        command_executor(prev_input[0], prev_input[1], NULL, 1);
    }
    else if(strcmp(command, command_name[EXIT]) == 0){
        int status = atoi(argument);
        int truncation = status / 256;
        if (truncation > 0 ){
            status -= truncation * 256;
        }
        printf("Good bye\n");
        exit(status);
    }
    else{
        printf("Bad command\n");
    }
}



/* command_process_unit()
 * Input:
 *      - input = the whole string from the input buffer.
 *      - prev_input[] = array of string, that stores previous command and argument.
 *              prev_input[0] = previous command.
 *              prev_input[1] = previous argument.
 *
 * Functionality:
 *      - Trims the inputted string by calling string_process()
 *      - Extracts the command string and argument(s) string from the inputted string buffer.
 *      - It then runs command_executor(), passing in the extracted command, arguments and prev_input[] in.
 *      - command_executor() ran in this way is set to mode = 0.
 *      - If "input" is null-terminated after trimming the string, the whole function is returned.
 *
 *
 */
void command_process_unit(char input[], char* prev_input[]){
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

    command_executor(command, arguments,  prev_input, 0);
    free(command);
    free(arguments);
    free(trim_input);

}


/* main()
 * Just a placeholder where everything comes together, nothing exciting for now.
 *
 * prev_input[0] and prev_input[1] are malloc-ed with intent of remember the string after other
 *      function(s) edit them.
 *
 */
int main() {
    char buffer[MAX_CMD_BUFFER];
    char* prev_input[2];
    prev_input[0] = (char*) malloc(sizeof(char));
    prev_input[1] = (char*) malloc(sizeof(char));
    printf("Starting IC shell\n");
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
        command_process_unit(buffer, prev_input);
    }
    free(prev_input[0]);
    free(prev_input[1]);
}


