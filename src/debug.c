#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug.h"

FILE *logFile;

void SetupDebug(){
	logFile = fopen("../log.txt", "a");
	fprintf(logFile, "\n---------------\nSEPARATOR\n---------------\n\n");
}

void DebugLog(int type, const char *format, ...){
	va_list vaFormat;
	
	//Use var args to create formatted text
	va_start(vaFormat, format);
	char *formattedText = malloc((strlen(format) + 64) * sizeof(char));
	vsprintf(formattedText, format, vaFormat);
	va_end(vaFormat);
	
	//Insert timestamp to log
	time_t rawtime;
	struct tm *currentTime;
	time(&rawtime);
	currentTime = localtime(&rawtime);
	fprintf(logFile, "%d-%d-%d %d:%d:%d: ", 1900 + currentTime->tm_year, currentTime->tm_mon,
	currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
	
	//Insert corresponding debug log type
	switch(type){
		case D_ACT:
			fprintf(logFile, "[ACTION]: ");
			break;
		case D_WARN:
			fprintf(logFile, "[WARNING]: ");
			break;
		case D_ERR:
			fprintf(logFile, "[ERROR]: ");
			break;
		case D_SCRIPT_ERR:
			fprintf(logFile, "[LUA_ERROR]: ");
			break;
		case D_SCRIPT_ACT:
			fprintf(logFile, "[LUA_ACTION]: ");
			break;
		default:
			fprintf(logFile, "[UNKNOWN]: ");
			break;
	}
	
	//Put it all together
	fprintf(logFile, "%s\n", formattedText);
	//Free the formatted string
	free(formattedText);
	//Flush the file buffer to the file
	fflush(logFile);
}

void QuitDebug(){
	fclose(logFile);
}