//
// Created by davidponar on 24/05/2021.
//


#include "uthreads.h"
#include <iostream>
int main( ) {

    int ret = uthread_init(2);
    std::cout <<"hi " << ret << "\n";
}

