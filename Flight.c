#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Flight.h"
#include "fileHelper.h"

void	initFlight(Flight* pFlight, const AirportManager* pManager)
{
	Airport* pPortOr = setAiportToFlight(pManager, "Enter name of origin airport:");
	pFlight->nameSource = _strdup(pPortOr->name);
	int same;
	Airport* pPortDes;
	do {
		pPortDes = setAiportToFlight(pManager, "Enter name of destination airport:");
		same = isSameAirport(pPortOr, pPortDes);
		if (same)
			printf("Same origin and destination airport\n");
	} while (same);
	pFlight->nameDest = _strdup(pPortDes->name);
	initPlane(&pFlight->thePlane);
	getCorrectDate(&pFlight->date);
}

int		isFlightFromSourceName(const Flight* pFlight, const char* nameSource)
{
	if (strcmp(pFlight->nameSource, nameSource) == 0)
		return 1;
		
	return 0;
}


int		isFlightToDestName(const Flight* pFlight, const char* nameDest)
{
	if (strcmp(pFlight->nameDest, nameDest) == 0)
		return 1;

	return 0;


}

int		isPlaneCodeInFlight(const Flight* pFlight, const char*  code)
{
	if (strcmp(pFlight->thePlane.code, code) == 0)
		return 1;
	return 0;
}

int		isPlaneTypeInFlight(const Flight* pFlight, ePlaneType type)
{
	if (pFlight->thePlane.type == type)
		return 1;
	return 0;
}


void	printFlight(const Flight* pFlight)
{
	printf("Flight From %s To %s\t",pFlight->nameSource, pFlight->nameDest);
	printDate(&pFlight->date);
	printPlane(&pFlight->thePlane);
}

void	printFlightV(const void* val)
{
	const Flight* pFlight = *(const Flight**)val;
	printFlight(pFlight);
}


Airport* setAiportToFlight(const AirportManager* pManager, const char* msg)
{
	char name[MAX_STR_LEN];
	Airport* port;
	do
	{
		printf("%s\t", msg);
		myGets(name, MAX_STR_LEN,stdin);
		port = findAirportByName(pManager, name);
		if (port == NULL)
			printf("No airport with this name - try again\n");
	} while(port == NULL);

	return port;
}

void	freeFlight(Flight* pFlight)
{
	free(pFlight->nameSource);
	free(pFlight->nameDest);
	free(pFlight);
}


int saveFlightToFile(const Flight* pF, FILE* fp)
{
	if (!writeStringToFile(pF->nameSource, fp, "Error write flight source name\n"))
		return 0;

	if (!writeStringToFile(pF->nameDest, fp, "Error write flight destination name\n"))
		return 0;

	if (!savePlaneToFile(&pF->thePlane,fp))
		return 0;

	if (!saveDateToFile(&pF->date,fp))
		return 0;

	return 1;
}
int saveFlightToCompressBFile(const Flight* pF, FILE* fp)
{
    BYTE data[6] = {0};

    int lenSrc = (int)strlen(pF->nameSource);
    int lenDest = (int)strlen(pF->nameDest);

    data[0] = lenSrc << 3;
    data[0] = data[0] | ((lenDest >>2)&0x7);
    data[1] = (lenDest &0x3)<<6;
    data[1] = data[1]| pF->thePlane.type << 4;
    data[1] = data[1]| pF->date.month;
    data[2] = (pF->thePlane.code[0] - 0x41) <<3;
    data[2] = data[2] | ((pF->thePlane.code[1] - 0x41)>>2)&0x7;
    data[3] = ((pF->thePlane.code[1] - 0x41)&0x3)<<6;
    data[3] = data[3]| (pF->thePlane.code[2]-0x41)<<1;
    data[3] = data[3] | ((pF->thePlane.code[3]-0x41)>>4)&0x1;
    data[4] = ((pF->thePlane.code[3]-0x41)&0xF)<<4;
    data[4] = data[4] | (pF->date.year -0x7E5);
    data[5] = pF->date.day;

    if (fwrite(&data, sizeof(BYTE), 6, fp) != 6)
        return 0;
    if (fwrite(pF->nameSource, sizeof(char), lenSrc, fp) != lenSrc)
        return 0;
    if (fwrite(pF->nameDest, sizeof(char), lenDest, fp) != lenDest)
        return 0;

    return 1;

}
int loadFlightFromCompressBFile(Flight* pF,  FILE* fp)
{
    BYTE data[6];
    if(fread(&data,sizeof(BYTE),6,fp)!=6)
        return 0;
    int lenSrc = ((data[0] >> 3) &0x1F);
    int lenDest = ((data[0] &0x7)<<2) |((data[1] >> 6)&0x3);
    pF->thePlane.type = (ePlaneType)((data[1] >> 4)&0x3);
    pF->date.month = data[1] &0xF;
    pF->thePlane.code[0] = 0x41+((data[2]>>3) &0x1F);
    pF->thePlane.code[1] = 0x41+(((data[2]&0x7)<<2)|((data[3]>>6)&0x3));
    pF->thePlane.code[2] = 0x41+((data[3]>>1)&0x1F);
    pF->thePlane.code[3] = 0x41+(((data[3]&0x1)<<4)|((data[4]>>4)&0xF));
    pF->thePlane.code[4] = '\0';
    pF->date.year = 0x7E5 + ((data[4])&0xF);
    pF->date.day = data[5] & 0x1F;
    pF->nameSource = (char*) calloc(lenSrc+1,sizeof(char ));
    if(!pF->nameSource) {
        fclose(fp);
        return 0;
    }
    if(fread(pF->nameSource,sizeof(char),lenSrc,fp)!= lenSrc)
    {
        fclose(fp);
        free(pF->nameSource);
        return 0;
    }
    pF->nameDest = (char*) calloc(lenDest+1,sizeof(char ));
    if(!pF->nameDest) {
        fclose(fp);
        free(pF->nameSource);
        return 0;
    }
    if(fread(pF->nameDest,sizeof(char),lenDest,fp)!= lenDest)
    {
        fclose(fp);
        free(pF->nameSource);
        free(pF->nameDest);
        return 0;
    }
    return 1;
}

int loadFlightFromFile(Flight* pF, const AirportManager* pManager, FILE* fp)
{

	pF->nameSource = readStringFromFile(fp, "Error reading source name\n");
	if (!pF->nameSource)
		return 0;

	pF->nameDest = readStringFromFile(fp, "Error reading destination name\n");
	if (!pF->nameDest)
		return 0;

	if (!loadPlaneFromFile(&pF->thePlane, fp))
		return 0;

	if (!loadDateFromFile(&pF->date, fp))
		return 0;

	return 1;
}



int	compareFlightBySourceName(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;
	return strcmp(pFlight1->nameSource, pFlight2->nameSource);
}

int	compareFlightByDestName(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;
	return strcmp(pFlight1->nameDest, pFlight2->nameDest);
}

int	compareFlightByPlaneCode(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;
	return strcmp(pFlight1->thePlane.code, pFlight2->thePlane.code);
}

int		compareFlightByDate(const void* flight1, const void* flight2)
{
	const Flight* pFlight1 = *(const Flight**)flight1;
	const Flight* pFlight2 = *(const Flight**)flight2;


	return compareDate(&pFlight1->date, &pFlight2->date);
	

	return 0;
}

