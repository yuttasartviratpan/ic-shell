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
 * Trims all the white characters that comes before the first char
 * Ex: ____echo hello -> echo hello
 *     ____echo hi__ -> echo hi__
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

//Mode: 0 = remember the prev command, 1 = !0
void command_executor(char* command, char* argument, char* prev_input[], int mode){
    if(strcmp(command, command_name[ECHO]) == 0){
        printf("This is echo\n");
        if(mode == 0){
            prev_input[0] = realloc(prev_input[0], strlen(command) * sizeof(char));
            strcpy(prev_input[0], command);
            prev_input[1] = realloc(prev_input[1], strlen(argument) * sizeof(char));
            strcpy(prev_input[1], argument);
        }
    }
    else if(strcmp(command, command_name[DOUBLE_BANG]) == 0){
        printf("Double banged\n");
        if(*prev_input[0] == '\0'){
            return;
        }
        command_executor(prev_input[0], prev_input[1], NULL, 1);
    }
    else if(strcmp(command, command_name[EXIT]) == 0){
        printf("Try to exit, but not now\n");
        if(mode == 0){
            prev_input[0] = realloc(prev_input[0], strlen(command) * sizeof(char));
            strcpy(prev_input[0], command);
            prev_input[1] = realloc(prev_input[1], strlen(argument) * sizeof(char));
            strcpy(prev_input[1], argument);
        }
    }
    else{
        printf("Don't know this command\n");
    }
}

// Performance improvement suggestion: do space trimming while going through the string
//  in command_process_unit. So you don't have to go through buffer twice.
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
        arguments = realloc(arguments, (argument_pos + 1) * sizeof(char));
        command[command_pos] = '\0';
        arguments[argument_pos] = '\0';
    }
    else{
        command = realloc(command, (command_pos + 1) * sizeof(char));
        command[command_pos] = '\0';
        arguments[0] = '\0';
    }

    command_executor(command, arguments,  prev_input, 0);
    free(command);
    free(arguments);
    free(trim_input);

}

int main() {
    char buffer[MAX_CMD_BUFFER];
    char* prev_input[2];
    prev_input[0] = (char*) malloc(sizeof(char));
    prev_input[1] = (char*) malloc(sizeof(char));
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
        command_process_unit(buffer, prev_input);
    }
    free(prev_input[0]);
    free(prev_input[1]);
}


