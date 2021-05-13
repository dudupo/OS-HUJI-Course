#!/usr/bin/env bash
./cmake-build-debug/theTests --gtest_filter=Test1.BasicFunctionality
./cmake-build-debug/theTests --gtest_filter=Test2.ThreadSchedulingWithTermination
./cmake-build-debug/theTests --gtest_filter=Test3.ThreadExecutionOrder
./cmake-build-debug/theTests --gtest_filter=Test4.StressTestAndThreadCreationOrder
./cmake-build-debug/theTests --gtest_filter=Test6.RandomThreadOperations
./cmake-build-debug/theTests --gtest_filter=Test7.MutexTest1
./cmake-build-debug/theTests --gtest_filter=Test8.MutexTest2
./cmake-build-debug/theTests --gtest_filter=Test9.MutexTest3
./cmake-build-debug/theTests --gtest_filter=Test10.MutexTest4
./cmake-build-debug/theTests --gtest_filter=Test11.MutexTest5
./cmake-build-debug/theTests --gtest_filter=Test12.MutexTest6
./cmake-build-debug/theTests --gtest_filter=Test13.MutexTest7
./cmake-build-debug/theTests --gtest_filter=Test14.MutexTest8

