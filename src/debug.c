#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug.h"

FILE *logFile;
int logComplexity = DL_VERBOSE;

void SetupDebug(){
	logFile = fopen("../log.txt", "a");
	fprintf(logFile, "\n---------------\nSEPARATOR\n---------------\n\n");
}

void DebugLog(int type, const char *format, ...){
	va_list vaFormat;
	
	//Use var args to create formatted text
	va_start(vaFormat, format);
	int length = vsnprintf(NULL, 0, format, vaFormat);
	char *formattedText = malloc((strlen(format) + length + 8) * sizeof(char));
	vsnprintf(formattedText, sizeof(char) * (length + 1), format, vaFormat);
	va_end(vaFormat);
	
	//Insert timestamp to log
	if(type <= logComplexity){
		time_t rawtime;
		struct tm *currentTime;
		time(&rawtime);
		currentTime = localtime(&rawtime);
		fprintf(logFile, "%d-%d-%d %d:%d:%d: ", 1900 + currentTime->tm_year, currentTime->tm_mon,
		currentTime->tm_mday, currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
	}
	
	//Insert corresponding debug log type depending on defined log complexity
	if(type <= logComplexity){
		switch(type){
			//Brief
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

			//Verbose
			case D_VERBOSE_ACT:
				fprintf(logFile, "[ACTION]: ");
				break;
			case D_VERBOSE_WARN:
				fprintf(logFile, "[WARNING]: ");
				break;
			case D_VERBOSE_ERR:
				fprintf(logFile, "[ERROR]: ");
				break;

			default:
				break;
		}
		fprintf(logFile, "%s\n", formattedText);
	}
	
	//Free the formatted string
	free(formattedText);
	//Flush the file buffer to the file
	fflush(logFile);
}

void QuitDebug(){
	fclose(logFile);
}