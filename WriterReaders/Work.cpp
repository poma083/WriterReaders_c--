#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "Threads.h"
#include "Work.h"

extern CWriterThread writer;

DWORD __stdcall StartFuncReader( void * params ){
	CReaderThread * r = (CReaderThread *)params;
	while( true ){
		//Sleep(50);
		r->RefreshData(&writer);
		printf("%s N %s - data=%s%s", "Reader", r->GetNumber(), r->GetData(), "\n");
	}
	return 0;
};

DWORD __stdcall StartFuncWriter( void * params ){
	CWriterThread * w = (CWriterThread *)params;
	while( true ){
		//������������ ��������� ���������� �� ������ ���������� ������
		// ������� ���� ��� �������� �������� �� �����
		HANDLE* handles = NULL;
		int count = w->GetReaderWaitObjects(handles);
		handles = new HANDLE[count];
		w->GetReaderWaitObjects(handles);
		WaitForMultipleObjects(count, handles, TRUE, INFINITE);
		// C������ ���� ��� �� ���������� ������ � ����� 
		//������ �� ����� ������ ���� �� �� ��������.
		ResetEvent(w->writerSyncPrimitive);	
		delete[] handles;
		
		// ����� ������ ��-������� � ������� ���������� �������
		// �������� �� ������� ���������� ���������� ������� Sleep
		std::cout << "\n";
		std::cout << "Write data char \"";
		w->SetData("");
		w->SetData("-");
		for(int i = 0; i < 10; i++){
			char* sss = new char[10]; 
			sprintf( sss, "%d", i );
			std::cout << sss;
			std::cout << "\"";
			w->AddData(sss);
			Sleep(10);
			std::cout << "\b\b";
			delete[] sss;
		}
		w->AddData("-");
		std::cout << "Writed\"\n";
		std::cout << "\n";

		// ������� ��� �� ��������� ������ � ������ ����� ���� ���������
		SetEvent(w->writerSyncPrimitive);		
	}
	return 0;
};