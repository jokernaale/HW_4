cmake_minimum_required(VERSION 3.20)
project(HW_4 C)

set(CMAKE_C_STANDARD 99)

add_executable(HW_4 main.c Address.c Address.h Airline.c Airline.h Airport.c Airport.h AirportManager.h AirportManager.c Date.c Date.h fileHelper.c fileHelper.h Flight.h Flight.c General.c General.h GeneralList.c GeneralList.h Plane.c Plane.h main.h)
