# MAVERIC Flight Software

Flight software for **MAVERIC** (Magnetic Vector and Remote Imaging Communication), a 3U CubeSat designed, built, and operated by students at the USC Space Engineering Research Center (SERC). MAVERIC launched July 7, 2026 aboard a SpaceX Falcon 9 rideshare mission and is currently operational in low Earth orbit.

## Mission Overview

MAVERIC is a science and technology demonstration mission carrying:

- **Magnetometry payload** — over a dozen internal and deployable magnetometers that measure Earth's magnetic field along the spacecraft's orbit, cross-referenced against existing geomagnetic models.
- **Imaging payload** — dual COTS multispectral cameras for remote imaging.
- **Technology demonstrators** — Holonav (a holographic 3D display for visualizing rendezvous/proximity operations) and Astroboard (a 2D LCD display technology demo).
- **Magnetorquer-only ADCS** — a custom B-dot control algorithm for de-spin and attitude control, with solar sensor input for Sun tracking, serving as an in-flight testbed for magnetorquer-only pointing.

MAVERIC's bus and heritage subsystems build on earlier SERC CubeSat missions (Aeneas, Dodona, Mayflower-Caerus), with this codebase representing a ground-up modular redesign of that legacy flight software to support MAVERIC's new hardware and mission requirements.

## System Architecture

The spacecraft runs on two [PIC24FJ256GA110](https://www.microchip.com/wwwproducts/en/PIC24FJ256GA110) microcontrollers (256 KB flash), split across an **upper** and **lower** payload processor module (PPM). Flight software is built and deployed as two separate standalone embedded applications, one per PPM, communicating over an internal packet-based link.

Onboard responsibilities span:

- **Avionics** — core bus management, power, and health monitoring
- **GNC / ADCS** — custom attitude determination and B-dot magnetorquer control
- **Magnetorquers** — actuation drivers for de-spin and pointing
- **Flash memory** — persistent onboard storage for telemetry, logs, and scheduled data
- **Communications** — radio interface and ground link handling
- **Payloads** — magnetometer array, camera, Holonav, and Astroboard interfacing

## Flight Software Capabilities

This codebase is a modular rewrite of the legacy PIC24 CubeSat flight software, built to be portable across missions and extensible to new hardware. Key components include:

- **Peripheral driver layer** — five drivers spanning I2C, SPI, and UART for subsystem and payload interfacing
- **Custom GNC/ADCS stack** — attitude determination and B-dot magnetorquer control
- **Unified system time** — a single time reference shared across all subsystems
- **Packet-based networking** — internal messaging protocol for upper/lower PPM and subsystem integration
- **Custom TT&C protocol** — telemetry, tracking, and command handling for ground communications
- **Persistent scheduler** — survives resets/power cycles to maintain mission timelines
- **Autonomous LEOPS** — self-directed launch and early orbit operations sequencing, no ground intervention required
- **Failsafe and redundancy modes** — fault detection and recovery to protect the mission in off-nominal conditions

## Repository Structure

| Path | Contents |
| --- | --- |
| `common/maveric` | Shared flight software common to both PPMs on the current MAVERIC mission |
| `projects` | Per-processor (upper/lower PPM) project builds |
| `archive/aeneas` | Archived flight software from the legacy Aeneas mission, retained for heritage reference |
| `tests` | Unit tests for platform-independent code |
| `misc` | Supporting scripts and utilities |

## Dependencies

The application is written in C/C++.

### Build Dependencies

- MPLAB IDE v8.80 (legacy version)
- [CCS PCD C Compiler v4.124](http://www.ccsinfo.com/content.php?page=compilers) ([manual](https://www.ccsinfo.com/downloads/ccs_c_manual.pdf))
- [CCS MPLAB plugin](https://www.ccsinfo.com/faq.php?page=ccs_mplab)
- Windows 10 machine

Development environment setup instructions are available [here](https://uscisi.atlassian.net/wiki/spaces/DOD/pages/2120712220/Development+Environment). The [Microchip Developer Help](http://microchip.wikidot.com/) wiki has additional tooling reference.

### Install Dependencies

- MPLAB v8.80
- [MPLAB ICD 3 In-Circuit Debugger](https://www.microchip.com/developmenttools/ProductDetails/dv164035)

### Unit Test Dependencies

- Visual Studio 2019
- [Google Test](https://google.github.io/googletest/) (included with the VS installation)

Unit testing is available for platform-independent code.

## Build & Install

### Build

1. Open the relevant project workspace under `./projects`.
2. Review `config.h` and any other configuration parameters for your target build.
3. Right-click the project file within MPLAB and select **Compile**.

### Install

1. Power on the processor.
2. Connect the USB cable from your computer to the ICD 3, the grey modular cable from the ICD 3 to the Test Interface Board, and the flat ribbon cable from the Test Interface Board to the PPM's programming port.
   - **Note:** The upper and lower PPMs use different ribbon cables and test interface boards — the upper PPM uses a 6-pin connector, the lower uses an 8-pin connector.
3. In MPLAB, select **Programmer → Select Programmer → MPLAB ICD 3**.
4. Once the programmer and device are connected, use the resulting toolbar to erase, verify, and load the program onto the device.

### Execute

The program runs automatically once loaded and the device is powered on.

## Windows Support & Unit Testing

A Visual Studio solution is included for browsing the codebase when MPLAB isn't available (it is not used for building/running the full flight software). It also supports unit testing of platform-independent modules:

1. Write a platform-independent object.
2. Add its `.c` file to the Windows-target static library project.
3. Create a dedicated `.cpp` test file and place it in `./tests`.
4. Add the test file to the test project's **Tests** filter.
5. Compile the test project.
6. Run tests via the **Test Explorer** view.

Support for unit testing started early in the project and has not been maintained consistently since — some tests may fail even though the underlying code is flight-operational.

## Team

MAVERIC flight software has been developed and maintained by a cross-disciplinary team of 20+ engineers across mechanical, aerospace, electrical, systems, astronautical, and computer engineering disciplines at USC SERC, through iterative testing, simulation, and hardware-software integration campaigns.
