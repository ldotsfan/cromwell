#include "boot.h"

CONFIGENTRY *ParseConfig(char *szBuffer, unsigned int fileLen, char *szPath) {
	char *linePtr;
	char *currentPos = szBuffer;
	CONFIGENTRY *rootEntry = (CONFIGENTRY*)malloc(sizeof(CONFIGENTRY));
	CONFIGENTRY *currentEntry = rootEntry;
	
	memset(rootEntry,0x00,sizeof(CONFIGENTRY));
	//Null terminate the input data
	*(szBuffer+fileLen)=0;
	
	for (linePtr = (char *)strsep(&currentPos, "\n"); linePtr != NULL; linePtr = (char *)strsep(&currentPos, "\n")) {
		char *param, *paramdata;
		char *p;
		
		param = (char *)strsep(&linePtr," \t");
	
		if (linePtr==NULL) {
			continue;
		}
		p=param+strlen(param)+1;
		//Strip off leading whitespace
		while (isspace(*p)) p++;
		
		paramdata = p;
	
		//Strip off trailing whitespace
		p=paramdata+strlen(paramdata)-1;
		while (p!=paramdata && isspace(*p)) p--;
		*(p+1)=0;
		
		if (!strncmp(param,"title",5)) {
			//Do we already have a title in this entry?
			//If so, this is the start of a new 'bootitem'.  
			//Otherwise, it begins a whole new entry
			if (strlen(currentEntry->title)==0) {
				strncpy(currentEntry->title, paramdata, strlen(paramdata));
			}
			else {	
				currentEntry->nextConfigEntry = (struct CONFIGENTRY*)malloc(sizeof(CONFIGENTRY));
				memset(currentEntry->nextConfigEntry, 0x00, sizeof(CONFIGENTRY));
				currentEntry = (CONFIGENTRY*)currentEntry->nextConfigEntry;
				strncpy(currentEntry->title, paramdata, strlen(paramdata));
			}
			
		}
		else if (!strncmp(param,"kernel",6)) {
			//Handle 'xbox-os' naming conventions
			chrreplace(paramdata, '\\', '/');

			if (szPath!=NULL) { 
				sprintf(currentEntry->szKernel,"%s/%s",szPath, paramdata);
			}
			else {
				//Add a leading slash, if the path name does not already contain one
				if (*paramdata=='/') sprintf(currentEntry->szKernel,"%s", paramdata);
				else sprintf(currentEntry->szKernel,"/%s", paramdata);
			}
		}
		else if (!strncmp(param,"initrd",6)) {
			//Handle 'xbox-os' naming conventions
			chrreplace(paramdata, '\\', '/');

			if (szPath!=NULL) { 
				sprintf(currentEntry->szInitrd,"%s/%s",szPath, paramdata);
			}
			else {
				//Add a leading slash, if the path name does not already contain one
				if (*paramdata=='/') sprintf(currentEntry->szInitrd,"%s", paramdata);
				else sprintf(currentEntry->szInitrd,"/%s", paramdata);
			}
		}
		else if (!strncmp(param,"append",6)) {
			strncpy(currentEntry->szAppend, paramdata, strlen(paramdata));
		}
	}
	return rootEntry;
}
