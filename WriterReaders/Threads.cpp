#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "Threads.h"

//-----------------------------------------    BaseClass begin     -------------------------------------------//
CThread::CThread( lpStartFunc func, unsigned int CreateSuspended ):Thread(INVALID_HANDLE_VALUE){
	this->Thread = CreateThread( NULL, 0, func, this, CreateSuspended, NULL );
	suspend = CreateSuspended == CREATE_SUSPENDED;
};
CThread::~CThread(){
	if( this->Thread != INVALID_HANDLE_VALUE ){
		Suspend();
		CloseHandle( this->Thread );
		this->Thread = INVALID_HANDLE_VALUE;
	};
};
DWORD CThread::Suspend(){
	DWORD res = SuspendThread( this->Thread );
	if ( res >= 1 ) {
		suspend = true;
	}
	return res;
};
DWORD CThread::Resume(){
	DWORD res = ResumeThread( this->Thread );
	if ( res < 1 ) {
		suspend = false;
	}
	return res;
};
DWORD CThread::WaitFor( DWORD ms ){
	return WaitForSingleObject( this->Thread, ms );
};
//-----------------------------------------    BaseClass end     -------------------------------------------//

//-----------------------------------------    CReaderThread begin     -------------------------------------------//
CReaderThread::CReaderThread( lpStartFunc func, unsigned int CreateSuspended ):CThread( func, CreateSuspended){
	readerSyncPrimitive = CreateEvent(NULL, TRUE, TRUE, NULL);
	tmpData = new char[80];
	strcpy(tmpData, "");
}
CReaderThread::~CReaderThread(){
	if(tmpData != NULL){
		delete[] tmpData;
		tmpData = NULL;
	}
	if(number != NULL){
		delete[] this->number;
		number = NULL;
	}
	if( this->readerSyncPrimitive != INVALID_HANDLE_VALUE ){
		CloseHandle(readerSyncPrimitive);
		this->readerSyncPrimitive = INVALID_HANDLE_VALUE;
	}
};
void CReaderThread::SetNumber(int n){
	this->number = new char[11];
	memset(this->number,32,10);
	this->number[10] = 0;
	char* tmp = new char[11];
	sprintf(tmp,"%d",n);
	memcpy(this->number,tmp,strlen(tmp));
	delete[] tmp;
};
bool CReaderThread::IsChildren(const CWriterThread* const writer) const{
	std::list<CReaderThread*>::const_iterator it = std::find(writer->readers.begin(), writer->readers.end(), this);
	return it != writer->readers.end();
};
void CReaderThread::RefreshData(const CWriterThread* writer){
	writer->CopyDataTo(this->tmpData, this->readerSyncPrimitive);
};
//-----------------------------------------    CReaderThread end     -------------------------------------------//
//-----------------------------------------    CWriterThread begin     -------------------------------------------//
CWriterThread::CWriterThread( lpStartFunc func ):CThread( func ){
	writerSyncPrimitive = CreateEvent(NULL, TRUE, TRUE, NULL);
	data = new char[13];
	strcpy(data,"werwer");
};
CWriterThread::~CWriterThread(){
	if(data != NULL){
		delete[] data;
		data = NULL;
	}
	if( this->writerSyncPrimitive != INVALID_HANDLE_VALUE ){
		CloseHandle(writerSyncPrimitive);
		this->writerSyncPrimitive = INVALID_HANDLE_VALUE;
	}
};
int CWriterThread::GetReaderWaitObjects(HANDLE* handles){
	if(handles == NULL){
		return readers.size();
	}
	HANDLE* handles_tmp = handles;
	std::list<CReaderThread*>::iterator it;
	int i = 0;
	for(it = readers.begin(); it != readers.end(); it++, i++){
		handles[i] = (*it)->readerSyncPrimitive;
	}
	return readers.size();
};
int CWriterThread::CopyDataTo(char* dst, HANDLE syncObject) const{
	//CRITICAL_SECTION cSection;
	//InitializeCriticalSection(&cSection);
	//EnterCriticalSection(&cSection);
	//закоментарив следующую инструкцию мы увидим испорченые данные
	WaitForSingleObject(this->writerSyncPrimitive,INFINITE);
	//мы можем получить испорченые данные, в том случае, когда
	//какой-либо из потоков-читателей прервёт своё выполнение, 
	//отдав квант времени, между указанными инструкциями.
	// В таком случае, для исправления этой ситуации,
	//необходимо раскоментировать использование критической секции
	ResetEvent(syncObject);
	//LeaveCriticalSection(&cSection);

	if((data == NULL)||(dst == NULL)){
		return -1;
	}
	for(int i = 0; i < 12; i+=2){
		strncpy(dst+i,data+i,2);
		//Sleep(200);
	}
	int result = strlen(data);
	dst[result] = 0;
	SetEvent(syncObject);
	return result;
};
int CWriterThread::AddReader( CReaderThread* reader){
	bool b = reader->IsChildren(this);
	if(!b){
		this->readers.push_back(reader);
	}
	return 0;
};
//void CWriterThread::RemoveReader( CReaderThread* reader){
//	if(reader->IsChildren(this)){
//		readers.remove(reader);
//	}
//};
void CWriterThread::SetData(const char* str){
	///*
	if(data != NULL){
		if(strlen(data) < strlen(str)){
			delete[] data;
			data = new char[strlen(str) + 1];
		}
	}
	else{
		data = new char[strlen(str) + 1];
	}
	//*/
	strcpy(data, str);
};
void CWriterThread::AddData(const char* str){
	///*
	int len = strlen(str) + 1;
	if(data != NULL){
		len+= strlen(data);
	}
	char* tmp = new char[len];
	if(data != NULL){
		strcpy(tmp, data);
	}
	strcat(tmp, str);
	if(data != NULL){
		delete[] data;
	}
	data = tmp;
	//*/
	//strcat(data, str);//
};
//-----------------------------------------    CWriterThread end     -------------------------------------------//