/* 
 * File:   port_audio_record_test.cpp
 * Author: harprobey
 *
 * Created on 26/03/2014, 8:50:38 AM
 */

#include <stdlib.h>
#include <iostream>

/*
 * Simple C++ Test Suite
 */

void test1() {
    std::cout << "port_audio_record_test test 1" << std::endl;
}

void test2() {
    std::cout << "port_audio_record_test test 2" << std::endl;
    std::cout << "%TEST_FAILED% time=0 testname=test2 (port_audio_record_test) message=error message sample" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "%SUITE_STARTING% port_audio_record_test" << std::endl;
    std::cout << "%SUITE_STARTED%" << std::endl;

    std::cout << "%TEST_STARTED% test1 (port_audio_record_test)" << std::endl;
    test1();
    std::cout << "%TEST_FINISHED% time=0 test1 (port_audio_record_test)" << std::endl;

    std::cout << "%TEST_STARTED% test2 (port_audio_record_test)\n" << std::endl;
    test2();
    std::cout << "%TEST_FINISHED% time=0 test2 (port_audio_record_test)" << std::endl;

    std::cout << "%SUITE_FINISHED% time=0" << std::endl;

    return (EXIT_SUCCESS);
}

