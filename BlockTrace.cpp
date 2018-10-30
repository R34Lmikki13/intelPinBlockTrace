#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include "pin.h"

ofstream out;

void Image(IMG img, void * v) {
    if (IMG_IsMainExecutable(img)) {
        char temp[1024];
        sprintf(temp, "%s loaded at %p", IMG_Name(img).c_str(), (void *)IMG_StartAddress(img));
        out << temp << endl;
    }
}

set<string> stringSet;

void BBLHit(void *ip) {
    char temp[1024];
    sprintf(temp, "%p", ip);
    string str1(temp);
    if (stringSet.find(str1) == stringSet.end()) {
        stringSet.insert(str1);
        out << temp << endl;
    }
}

void Trace(TRACE trace, void * v) {
	IMG img = IMG_FindByAddress(TRACE_Address(trace));
	if(!IMG_Valid(img) || !IMG_IsMainExecutable(img)){
        return ;
	}

    BBL bbl = TRACE_BblHead(trace); //返回本次Trace的第一个基本块
    for (; BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)BBLHit, IARG_INST_PTR, IARG_END);
    }

}

/*!
 *  Print out help message.
 */
INT32 usage()
{
    out << "usage " << endl;

    return -1;
}
/*!
 * Print out analysis results.
 * This function is called when the application exits.
 * @param[in]   code            exit code of the application
 * @param[in]   v               value specified by the tool in the 
 *                              PIN_AddFiniFunction function call
 */
VOID Fini(INT32 code, VOID *v)
{
    out <<  "Fini" << endl;
}


int main(int argc, char *argv[]) {
    if (PIN_Init(argc, argv))
        return usage();
    out.open("blockTrace.log");
    IMG_AddInstrumentFunction(Image, 0);
    TRACE_AddInstrumentFunction(Trace, 0);
    PIN_AddFiniFunction(Fini, 0);
    PIN_StartProgram();
    return 0;
}