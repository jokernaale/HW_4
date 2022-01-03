#pragma once
#include "AirportManager.h"
#include "Airline.h"
#define COMPRESS
#define AIRLINE_COMPRESS_FILE_NAME "C:\\Users\\joker\\CLionProjects\\HW_4\\airline_compress.bin"

typedef enum
{
	eAddFlight, eAddAirport, ePrintCompany, ePrintAirports,eSortFlights, eSearchFlight,
	ePrintNumFlightsOrig, ePrintFlightsPlaneCode, ePrintFlightsPlaneType, eNofOptions
} eMenuOptions;

const char* str[eNofOptions];

int initManagerAndAirline(AirportManager* pManager, Airline* pCompany,const char* airlineFileName, const char* airlineCompressFileName, int ifCompress);
int menu();


#define EXIT -1
#define MANAGER_FILE_NAME "C:\\Users\\joker\\CLionProjects\\HW_4\\airport_authority.txt"
#define AIRLINE_FILE_NAME "C:\\Users\\joker\\CLionProjects\\HW_4\\airline.bin"


