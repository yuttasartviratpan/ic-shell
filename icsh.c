/* ICCS227: Project 1: icsh
 * Name:
 * StudentID:
 */

#include "stdio.h"

#define MAX_CMD_BUFFER 255

int main() {
    char buffer[MAX_CMD_BUFFER];
    while (1) {
        printf("icsh $ ");
        fgets(buffer, 255, stdin);
        printf("you said: %s\n", buffer);
    }
}
