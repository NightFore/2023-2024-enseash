#include <stdlib.h>
#include <time.h>
#include <stdio.h>

long commandExecutionDuration(char *input){
    struct timespec start, end;
    // Get start time
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }

    //executeCommand(input);
    // Get end time
    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        perror("clock_gettime");
        return EXIT_FAILURE;
    }
    // Calculate duration in milliseconds
    long seconds = end.tv_sec - start.tv_sec;
    long nanoseconds = end.tv_nsec - start.tv_nsec;
    long duration_ms = seconds * 1000 + nanoseconds / 1000000;

    return duration_ms;
}