// UACDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//
// �ο����ϣ�http://blog.csdn.net/hiwubihe/article/details/44339125
//
// ����������У�UAC��UASͨ�ţ�source���Լ��ĵ�ַ��Ŀ���ַ����UAS

#include "stdafx.h"
#include "eXosip2/eXosip.h"

#include <iostream>
#include <WinSock2.h>


int _tmain(int argc, _TCHAR* argv[])
{
	const char *identity = "sip:140@127.0.0.1";
	const char *uas = "sip:133@127.0.0.1:15061";
	const char *source_call = "sip:140@127.0.0.1";
	const char *dest_call = "sip:133@127.0.0.1:15061";

	std::cout<<"SIP �ͻ��˲��Գ���"<<std::endl;
	std::cout<<"r - ��SIP����������ע��"<<std::endl;
	std::cout<<"c - ȡ��ע��(ע��?)"<<std::endl;
	std::cout<<"i - �����������"<<std::endl;
	std::cout<<"h - �ҶϺ���"<<std::endl;
	std::cout<<"q - �˳�����"<<std::endl;
	std::cout<<"s - ִ�� INFO ����"<<std::endl;
	std::cout<<"m - ִ�� MESSAGE ����"<<std::endl;

	// ��ʼ��eXosip
	struct eXosip_t *context_eXosip = NULL;
	int errCode = eXosip_init(context_eXosip);

	if (errCode != 0)
		return errCode;

	std::cout<<"ע�� eXosip �ɹ���"<<std::endl;

	return 0;
}

