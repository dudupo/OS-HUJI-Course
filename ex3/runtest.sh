#!/usr/bin/env bash
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.outputTest
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.outputTestFromTwoDiffrentThreads
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.outputTestWaitFromTwoThreads
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.outputTest3Clients
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.outputTestOneThreadOnly
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.progressTest
./cmake-build-debug/mattanTester --gtest_filter=MattanTests.deadlockTest
#./cmake-build-debug/theTests --gtest_filter=Test9.MutexTest3
#./cmake-build-debug/theTests --gtest_filter=Test10.MutexTest4
#./cmake-build-debug/theTests --gtest_filter=Test11.MutexTest5
#./cmake-build-debug/theTests --gtest_filter=Test12.MutexTest6
#./cmake-build-debug/theTests --gtest_filter=Test13.MutexTest7
#./cmake-build-debug/theTests --gtest_filter=Test14.MutexTest8
#./cmake-build-debug/theTests --gtest_filter=Test15.MutexTest9
#./cmake-build-debug/theTests --gtest_filter=Test16.MutexTest10