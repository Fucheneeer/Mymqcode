////////////////////////////////////////////////////////////////////////////////
//
// DEPMain.h
//
// Copyright (c) 2020 CFETSIT.
// Building 3, 491 YinDong Road, Shanghai, China.
// All rights reserved.
//
// LightMQ is the copyrighted,
// proprietary property of CFETSIT which retain all right, title and interest therein.
//
// Description: Define the main function for DEP IApplication
//
////////////////////////////////////////////////////////////////////////////////

#ifndef DEPMAIN_H_
#define DEPMAIN_H_

#include <cstdint>
#include <memory>

/**
 * @file
 * @brief Define the main function for DEP IApplication
 */

/**
 * DEP_APP_MAIN is a macro that generates the main(argc, argv) function.
 * It takes an application class name as arguments and in turns pass the class pointer
 * to dep::main(argc, argv, IDEPMain).
 * The application must inherits from IDEPMain in order to use this macro.
 */
#define DEP_APP_MAIN(ApplicationClass) \
    int main(int argc, const char* argv[]) \
    { \
        return static_cast<int16_t>(dep::main(argc, argv, std::unique_ptr<ApplicationClass>(new ApplicationClass))); \
    }

/**
 * @namespace dep
 * @brief Namespace for DEP project
 */
namespace dep {

class IDEPMain;
enum class ExitCode : int16_t;

/**
 * Entry point for IDEPMain application.
 *
 * This function initialize the application and inject the requires object dependency to it.
 * It is aim to generalize the application startup behaviors and help to maintain its lifecycle.
 *
 * This function operates only on IDEPMain and its derived class.
 * In current implementation, it would provides the following services to the application by calling
 * the corresponding IDEPMain interface
 * 1. Command Line Service
 * 2. Signal Handling Service
 * 3. Logger Service
 * 4. Selector and Timer Service (only for SelectorApplication)
 *
 * It should be noted that the caller thread will be blocked until the application is end.
 *
 * @param argc          Non-negative value representing the number of arguments
 * @param argv          Pointer to the first element of an array of pointers that represent the program arguments
 * @param pApplication  Pointer to the application program
 * @return exit code. 0 indicate no errors
 */
ExitCode main(int32_t argc, const char* argv[], std::unique_ptr<IDEPMain> pApplication);

} /* namespace dep */

#endif /* DEPMAIN_H_ */
