#ifdef __FreeBSD__
#else
#include <windows.h>
#define uint64_t unsigned long long int
#endif
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "Threads.h"
#include "Work.h"

CWriterThread writer = CWriterThread( StartFuncWriter );

int main(int argc, char** argv){
	writer.Resume();
	std::list<CReaderThread*> localListReaders;
	for(int i = 0; i < 15; i++){
		CReaderThread* reader = new CReaderThread( StartFuncReader, CREATE_SUSPENDED);
		reader->SetNumber(i);
		reader->Resume();
		writer.AddReader(reader);
		localListReaders.push_back(reader);
		//Sleep(100);
		writer.AddReader(reader);
	}
	writer.WaitFor( INFINITE );
	
	std::list<CReaderThread*>::iterator it;
	for(it = localListReaders.begin(); it != localListReaders.end(); it++){
		delete *it;
	}
	return 0;
}