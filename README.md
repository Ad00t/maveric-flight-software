# La Jument Flight Software

## Description
This repository contains the flight software for the La Jument cubesat project. Two separate applications are provided within the source because the cubesat requires two separate processors or PPMs - the upper and lower PPMs. The codebase is configured, compiled, installed, and ran as a standalone, embedded application.

## System Architecture
The target hardware is the [PIC24FJ256GA110](https://www.microchip.com/wwwproducts/en/PIC24FJ256GA110) General Purpose 256 KB Flash Microcontroller, built by Microchip.

## Dependencies
The application is written in C
### Build Dependencies
Development environment setup instructions are provided [here](https://uscisi.atlassian.net/wiki/spaces/DOD/pages/2120712220/Development+Environment). There is a [Microchip Developer Help](http://microchip.wikidot.com/) page with plenty of good info on the tools we are using.
- MPLAB IDE v8.80 - This is a legacy version of the MPLAB IDE.
- [CCS PCD C Compiler v4.124](http://www.ccsinfo.com/content.php?page=compilers) - Compiler manual provided [here](https://www.ccsinfo.com/downloads/ccs_c_manual.pdf).
- [CCS MPLAB plugin](https://www.ccsinfo.com/faq.php?page=ccs_mplab)
- Windows 10 Machine
### Install Dependencies
- MPLAB v8.80
- [MPLAB ICD 3 In-Circuit Debugger](https://www.microchip.com/developmenttools/ProductDetails/dv164035)
### Unit Test Dependencies
Unit testing is available for platform independent code. <br />
Requirements:
- Visual Studio 2019
- [Google Test](https://google.github.io/googletest/) Packege (part of VS installation)

## Instructions
### MPLAB IDE v8.80
#### Build
1. Open **./projects/Dodona/Dodona.mcw**
2. Verify the project is configured the way you want. There is a config.h file with some configuration settings. There are other configuration parameters peppered throughout the source.
333. Within the Dodona.mcw project window, right click **Dodona.mcp->Compile**
#### Install
1. Power on the processor
2. The hardware setup used to program the processors looks similar to [this](http://microchip.wikidot.com/icd3:test-interface)
    1. Connect the USB cable from your computer, to the ICD 3.
    2. Connect the grey modular cable to the ICD 3 and Test Interface Board.
    3. There is also a flat ribbon cable that goes from the Test Interface Board to the programming port on the PPM. **Note: The upper and lower PPMs have different flat ribbon cables and test interface boards. The upper has 6 pin connector while the lower has 8 pin connector ribbon cables/test boards**
3. Within MPLAB, make sure the correct programmer is selected. **Programmer->SelectProgrammer->MPLAB ICD 3**
4. Once the programmer is selected and the device is connected, a new tool bar will appear
5. You can now use it to erase the device, check it, and load the porgram onto the device
#### Execute
The program will run automatically once it is loaded onto the device and the device is powered on.

## User Guide
A user guide on how to interact with the programs is provided [here](./docs/UserGuide.md)

## Windows Support
A Visual Studio solution is included in the repository, thus allowing developers to more efficiently look through the project when MPLAB is not available. **It is not for building and running the entire project.** 
### Unit Testing
The Visual Studio environment can be used to unit test certain pieces of the code. The DodonaOnWindows project was created to build platform independent code, on Windows, in preparation for unit testing. That project creates a static libarary that is then linked into the unit test application, before testing. Support for unit testing started early in the project, but was not maintained as the project progressed. Some unit tests may fail at this point, though the code is operational.
### Unit Testing Instructions
1) Write a platform independent object. 
2) Add that object's .c file to the **DodonaOnWindows** project.
3) Create a .cpp test file dedicated to that object and place it in the **./tests** directory
4) Add that same .cpp test file to the **TestDodona* project, under the **Tests** filter
5) Compile the **TestDodona** project
6) Use the **Test Explorer** view to run the tests
