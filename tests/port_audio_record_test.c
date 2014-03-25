/*
 * File:   port_audio_record_test.c
 * Author: harprobey
 *
 * Created on 26/03/2014, 8:50:08 AM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Simple C Test Suite
 */

void test1() {
    printf("port_audio_record_test test 1\n");
}

void test2() {
    printf("port_audio_record_test test 2\n");
    printf("%%TEST_FAILED%% time=0 testname=test2 (port_audio_record_test) message=error message sample\n");
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% port_audio_record_test\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% test1 (port_audio_record_test)\n");
    test1();
    printf("%%TEST_FINISHED%% time=0 test1 (port_audio_record_test) \n");

    printf("%%TEST_STARTED%% test2 (port_audio_record_test)\n");
    test2();
    printf("%%TEST_FINISHED%% time=0 test2 (port_audio_record_test) \n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}
