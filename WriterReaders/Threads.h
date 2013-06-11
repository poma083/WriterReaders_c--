#ifndef __SERVICE_THREADS__
#define __SERVICE_THREADS__

#include <windows.h>
#include <list>
#include "Threads.h"

typedef DWORD (__stdcall *lpStartFunc)( void * );
class CWriterThread;

class CThread{
  public:
	CThread( lpStartFunc func, unsigned int CreateSuspended = CREATE_SUSPENDED );
	virtual ~CThread();
	DWORD Suspend();
	bool IsSuspend() const{ return suspend; };
	DWORD Resume();
	DWORD WaitFor( DWORD ms );
  private:
	bool suspend;
	HANDLE Thread;
};

class CReaderThread : public CThread{
  public:
	CReaderThread( lpStartFunc func, unsigned int CreateSuspended = CREATE_SUSPENDED  );
	~CReaderThread();
	//��� ��� ��������� ����� ��� ����� ������ 
	//������ ���������� �� ���������� ������ ������ CWriterThread
	//�� ������� ��� ��� "������"
	//������ ������� �� �������������� �������� � �� ������ ��� ���������
	bool IsChildren(const CWriterThread* const writer) const;
	void SetNumber(int n);
	char* GetNumber() const {return this->number;};
	//�������� ���������� ��������� �� ���������� ���������� ��� ���������������� ������ ������ �� ������� �� ������� ������, 
	//������ ������� ����� �� ������������ ���� ��������� ��� ��������� ������ �� ������� �� ���������
	char* GetData() const{return this->tmpData;};
	void RefreshData(const CWriterThread* writer);

	HANDLE readerSyncPrimitive;
private:
	char* number;
	char* tmpData;
};

class CWriterThread : public CThread{
  public:
	CWriterThread( lpStartFunc func );
	~CWriterThread();
	int AddReader(CReaderThread* reader);
	//void RemoveReader(CReaderThread* reader);
	const CReaderThread* const FirstReader() const;
	const CReaderThread* const NextReader() const;
	void SetData(const char* str);
	void AddData(const char* str);
	int GetReaderWaitObjects(HANDLE* handles);
	int CopyDataTo(char* dst, HANDLE syncObject) const;
	
	friend bool CReaderThread::IsChildren(const CWriterThread* const writer) const;

	HANDLE writerSyncPrimitive;
  private:
	std::list<CReaderThread*> readers;
	char* data;
};

#endif