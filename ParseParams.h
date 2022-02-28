#ifndef _PARSE_PARAMS_H_
#define _PARSE_PARAMS_H_

#include <stdlib.h>

#define MAX_PARAM_DEFS 50

struct ParseParamDef { 
	const char* name;
	void* outPtr;
	int type;
	int maxLen;
	const char* description;
};

struct ParseParamDefs {
	ParseParamDef defs[MAX_PARAM_DEFS];
	int count;
};

void startParamDefs(ParseParamDefs* defs) {
	defs->count = 0;
}

void addParamDef(ParseParamDefs* defs, const char* name, void* ptr, int type, const char* description) {
	int i = defs->count;
	ParseParamDef* def = &defs->defs[i];
	def->name = name;
	def->outPtr = ptr;
	def->type = type;
	def->description = description;
	defs->count++;
}

void addFlagParam(ParseParamDefs* defs, const char* name, int* ptr, const char* description) {
	addParamDef(defs, name, ptr, 0, description);
}

void addStrParam(ParseParamDefs* defs, const char* name, char* ptr, int maxLen, const char* description) {
	defs->defs[defs->count].maxLen = maxLen;
	addParamDef(defs, name, ptr, 1, description);
}

void addIntParam(ParseParamDefs* defs, const char* name, int* ptr, const char* description) {
	addParamDef(defs, name, ptr, 2, description);
}

void addDoubleParam(ParseParamDefs* defs, const char* name, double* ptr, const char* description) {
	addParamDef(defs, name, ptr, 3, description);
}

void printUsage(ParseParamDefs* defs) {
	printf("Valid options:\n");
	
	int tab = 0;
	for(int p=0; p<defs->count; p++) {
		int len = strlen(defs->defs[p].name)+12;
		if(len>tab)
			tab = len;
	}
	
	for(int p=0; p<defs->count; p++) {
		ParseParamDef* def = &defs->defs[p];
		printf("%s", def->name);
		int len = strlen(def->name);
		switch(def->type) {
			case 1:
				printf(" <string>");
				len += 9;
				break;
			case 2:
				printf(" <int>");
				len += 6;
				break;
			case 3:
				printf(" <float>");
				len += 9;
				break;
		}
		if(def->description) {
			for(; len<tab; len++)
				printf(" ");
			printf(": %s", def->description);
		}
		printf("\n");
	}
}

int parseParams(ParseParamDefs* defs, int argc, char** argv) {
	char* end;
	for(int i=1; i<argc; i++) {
		int found = 0;
		for(int p=0; p<defs->count; p++) {
			ParseParamDef* def = &defs->defs[p];
			if(!strcmp(argv[i], def->name)) {
				if(def->type!=0 && i+1>=argc) {
					printf("Expected value for option %s\n\n", argv[i]);
					printUsage(defs);
					return 0;
				}
				switch(def->type) {
					case 0:
						*((int*) def->outPtr) = 1;
						break;
					case 1:
						strncpy((char*) def->outPtr, argv[++i], def->maxLen);
						break;
					case 2:
						*((int*) def->outPtr) = strtol(argv[++i], &end, 10);
						if(end==argv[i]) {
							printf("Expected interger value for option %s\n\n", argv[i-1]);
							printUsage(defs);
							return 0;
						}
						break;
					case 3:
						*((double*) def->outPtr) = strtod(argv[++i], &end);
						if(end==argv[i]) {
							printf("Expected float value for option %s\n\n", argv[i-1]);
							printUsage(defs);
							return 0;
						}
						break;
				}
				found = 1;
				break;
			}
		}
		if(!found) {
			printf("Unknown option %s\n\n", argv[i]);
			printUsage(defs);
			return 0;
		}
	}
	return 1;
}

#endif
