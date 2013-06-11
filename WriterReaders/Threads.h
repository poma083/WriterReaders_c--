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
	//так как следующий метод для своей работы 
	//должен обращаться ко внутренним данным класса CWriterThread
	//то объявем его там "другом"
	//однако клянёмся не злоупотреблять доверием и не менять его состояния
	bool IsChildren(const CWriterThread* const writer) const;
	void SetNumber(int n);
	char* GetNumber() const {return this->number;};
	//пришлось возвращать указатель на внутреннюю переменную для демонстративного вывода данных на консоль из функции потока, 
	//однако кленёмся никак не использовать этот указатель для изменения данных на которые он указывает
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