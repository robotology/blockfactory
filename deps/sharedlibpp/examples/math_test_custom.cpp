/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <stdio.h>
#include "MyMathCustom.h"

#include <shlibpp/SharedLibraryClass.h>
#include <shlibpp/SharedLibrary.h>


int main(int argc, char *argv[])
{

    if(argc < 2) {
        printf("Usage: %s <library file name>\n", argv[0]);
        printf("for example: %s libmymath.so\n", argv[0]);
        return 0;
    }

    // create an instance of shared library class factory to load the library
    printf("Loading the shared library... \n");
    shlibpp::SharedLibraryClassFactory<MyMathCustom> myMathFactory(argv[1],
                                                                   CUSTOM_START_CHECK,
                                                                   CUSTOM_END_CHECK,
                                                                   CUSTOM_SYSTEM_VERSION,
                                                                   "my_math_custom");
    if (!myMathFactory.isValid()) {
        printf("error (%d) : %s\n", static_cast<std::uint32_t>(myMathFactory.getStatus()),
                                    myMathFactory.getError().c_str());
        return 1;
    }

    // create an instance of the class and call its functions
    shlibpp::SharedLibraryClass<MyMathCustom> myMath(myMathFactory);
    printf("Calling some of its functions... \n");
    printf("15 + 12 = %d\n", myMath->add(15, 12));
    printf("15 - 12 = %d\n", myMath->sub(15, 12));

    return 0;
}
