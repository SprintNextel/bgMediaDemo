#include "stdafx.h"

#include "eXosip2/eXosip.h"
#include "eXosip2.h"
#include "UASImp.h"
#include "bgSIPLogical.h"



bgUASImp::bgUASImp()
: sip_context_(NULL)
, is_thread_working_(false)
{

}

bgUASImp::~bgUASImp()
{

}

int bgUASImp::Init(const char *code, const char *ip /* = "0.0.0.0" */, int port /* = 5060 */, const char *name /* = "_bgUAS_" */)
{
	int errCode = 0;

	InitializeCriticalSection(&start_lock_);

	// ��ʼ�� eXosip ��
	sip_context_ = eXosip_malloc();
	if (sip_context_ == NULL)
		return -1;

	errCode = eXosip_init(sip_context_);
	if (errCode != 0)
		return errCode;

	// ���ؿ�������
	errCode = eXosip_listen_addr(sip_context_, IPPROTO_UDP, ip, port, AF_INET, 0);
	if (errCode == 0)
	{
		uas_ip_ = ip;
		uas_port_ = port;
		uas_name_ = name;
		uas_code_ = code;
	}

	return errCode;
}

void bgUASImp::Close()
{
	if (sip_context_)
		eXosip_quit(sip_context_);
	sip_context_ = NULL;
}

int bgUASImp::Start()
{
	EnterCriticalSection(&start_lock_);

	if (is_thread_working_)
		return 0;

	int errCode = 0;

	// �����̣߳���������¼�
	HANDLE thread_handle = CreateThread(NULL, 0, WorkingThread, this, 0, NULL);
	if (thread_handle == NULL)
		errCode = GetLastError();
	else
	{
		is_thread_working_ = true;
		CloseHandle(thread_handle);
	}
	
	LeaveCriticalSection(&start_lock_);
	return errCode;
}

eXosip_t* bgUASImp::get_context_()
{
	return sip_context_;
}

std::string bgUASImp::get_uas_ip_()
{
	return uas_ip_;
}

int bgUASImp::get_uas_port_()
{
	return uas_port_;
}

std::string bgUASImp::get_uas_code_()
{
	return uas_code_;
}

std::string bgUASImp::get_uas_name_()
{
	return uas_name_;
}

DWORD WINAPI bgUASImp::WorkingThread(LPVOID lpParam)
{
	bgUASImp *uas = (bgUASImp*)lpParam;
	eXosip_t *sip_context = uas->get_context_();

	while (true)
	{
		// �ȴ�SIP�¼�
		eXosip_event_t *sip_event = eXosip_event_wait(sip_context, 0, 50);

		eXosip_lock(sip_context);
		eXosip_default_action(sip_context, sip_event);
		eXosip_automatic_refresh(sip_context);
		eXosip_unlock(sip_context);

		if (sip_event == NULL)
			continue;

		switch (sip_event->type)
		{
		//////////////////////////////////////////////////////////////////////////
		// ע������¼�
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_REGISTRATION_NEW:
			// ��ע����Ϣ֪ͨ
			OutputDebugString(_T("����һ��ע������"));
			{
				bgSIPLogical::Register(sip_context, sip_event);
			}
			break;
		case EXOSIP_REGISTRATION_SUCCESS:
			// �û�ע��ɹ�
			OutputDebugString(_T("EXOSIP_REGISTRATION_SUCCESS"));
			break;
		case EXOSIP_REGISTRATION_FAILURE:
			// �û�ע��ʧ��
			OutputDebugString(_T("EXOSIP_REGISTRATION_FAILURE"));
			break;
		case EXOSIP_REGISTRATION_REFRESHED:
			// ע���Ѿ���ˢ��
			OutputDebugString(_T("EXOSIP_REGISTRATION_REFRESHED"));
			break;
		case EXOSIP_REGISTRATION_TERMINATED:
			// UA ����ִ��ע��
			OutputDebugString(_T("EXOSIP_REGISTRATION_TERMINATED"));
			break;

		//////////////////////////////////////////////////////////////////////////
		// ͨ����INVITE����¼�
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_CALL_INVITE:
			// �µ�����ͨ��֪ͨ
			OutputDebugString(_T("EXOSIP_CALL_INVITE"));
			break;
		case EXOSIP_CALL_REINVITE:
			// ����ͨ������������һ���µ�����֪ͨ
			OutputDebugString(_T("EXOSIP_CALL_REINVITE"));
			break;
		case EXOSIP_CALL_NOANSWER:
			// û��Ӧ��ֱ����ʱ
			OutputDebugString(_T("EXOSIP_CALL_NOANSWER"));
			break;
		case EXOSIP_CALL_PROCEEDING:
			// Զ��Ӧ�����ڴ���
			OutputDebugString(_T("EXOSIP_CALL_PROCEEDING"));
			break;
		case EXOSIP_CALL_RINGING:
			// �����л�����
			OutputDebugString(_T("EXOSIP_CALL_RINGING"));
			break;
		case EXOSIP_CALL_ANSWERED:
			// ͨ����ʼ
			OutputDebugString(_T("EXOSIP_CALL_ANSWERED"));
			break;
		case EXOSIP_CALL_REDIRECTED:
			// ����ת��
			OutputDebugString(_T("EXOSIP_CALL_REDIRECTED"));
			break;
		case EXOSIP_CALL_REQUESTFAILURE:
			// ����ʧ��
			OutputDebugString(_T("EXOSIP_CALL_REQUESTFAILURE"));
			break;
		case EXOSIP_CALL_SERVERFAILURE:
			// ������ʧ��
			OutputDebugString(_T("EXOSIP_CALL_SERVERFAILURE"));
			break;
		case EXOSIP_CALL_GLOBALFAILURE:
			// ȫ��ʧ��
			OutputDebugString(_T("EXOSIP_CALL_GLOBALFAILURE"));
			break;
		case EXOSIP_CALL_ACK:
			// INVITE����200���ͻ��յ�һ��ACK
			OutputDebugString(_T("EXOSIP_CALL_ACK"));
			break;
		case EXOSIP_CALL_CANCELLED:
			// ͨ����ȡ��
			OutputDebugString(_T("EXOSIP_CALL_CANCELLED"));
			break;
		case EXOSIP_CALL_TIMEOUT:
			// ͨ����ʱ
			OutputDebugString(_T("EXOSIP_CALL_TIMEOUT"));
			break;

		//////////////////////////////////////////////////////////////////////////
		// ͨ���г���INVITE֮�����������
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_CALL_MESSAGE_NEW:
			// һ���µ���������
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_NEW"));
			break;
		case EXOSIP_CALL_MESSAGE_PROCEEDING:
			// ���󷵻��� 1xx ״̬��
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_PROCEEDING"));
			break;
		case EXOSIP_CALL_MESSAGE_ANSWERED:
			// ���󷵻� 200 ״̬��
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_ANSWERED"));
			break;
		case EXOSIP_CALL_MESSAGE_REDIRECTED:
			// ����
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_REDIRECTED"));
			break;
		case EXOSIP_CALL_MESSAGE_REQUESTFAILURE:
			// ����
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_REQUESTFAILURE"));
			break;
		case EXOSIP_CALL_MESSAGE_SERVERFAILURE:
			// ����
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_SERVERFAILURE"));
			break;
		case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:
			// ����
			OutputDebugString(_T("EXOSIP_CALL_MESSAGE_GLOBALFAILURE"));
			break;
		case EXOSIP_CALL_CLOSED:
			// ����ͨ���յ�һ��BYE��Ϣ
			OutputDebugString(_T("EXOSIP_CALL_CLOSED"));
			break;

		//////////////////////////////////////////////////////////////////////////
		// �����Ϣ UAC �� UAS �������õ�
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_CALL_RELEASED:
			// ͨ�������ı�����
			OutputDebugString(_T("EXOSIP_CALL_RELEASED"));
			break;

		//////////////////////////////////////////////////////////////////////////
		// �ⲿ���õ������յ�����Ӧ����
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_MESSAGE_NEW:
			// һ���µ���������
			OutputDebugString(_T("EXOSIP_MESSAGE_NEW"));
			{
				if (MSG_IS_REGISTER(sip_event->request))
				{
					// ����һ��ע������
					bgSIPLogical::Register(sip_context, sip_event);
				}
				else if (MSG_IS_MESSAGE(sip_event->request))
				{
					bgSIPLogical::RecvSMS(sip_context, sip_event);
				}
			}
			break;
		case EXOSIP_MESSAGE_PROCEEDING:
			// ���󷵻��� 1xx ״̬��
			OutputDebugString(_T("EXOSIP_MESSAGE_PROCEEDING"));
			break;
		case EXOSIP_MESSAGE_ANSWERED:
			// ���󷵻� 200 ״̬��
			OutputDebugString(_T("EXOSIP_MESSAGE_ANSWERED"));
			break;
		case EXOSIP_MESSAGE_REDIRECTED:
			// ����
			OutputDebugString(_T("EXOSIP_MESSAGE_REDIRECTED"));
			break;
		case EXOSIP_MESSAGE_REQUESTFAILURE:
			// ����
			OutputDebugString(_T("EXOSIP_MESSAGE_REQUESTFAILURE"));
			break;
		case EXOSIP_MESSAGE_SERVERFAILURE:
			// ����
			OutputDebugString(_T("EXOSIP_MESSAGE_SERVERFAILURE"));
			break;
		case EXOSIP_MESSAGE_GLOBALFAILURE:
			// ����
			OutputDebugString(_T("EXOSIP_MESSAGE_GLOBALFAILURE"));
			break;

		//////////////////////////////////////////////////////////////////////////
		// ��ʱ��Ϣ
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_SUBSCRIPTION_UPDATE:
			// ����һ�� SUBSCRIBE�����ģ� ��Ϣ
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_UPDATE"));
			break;
		case EXOSIP_SUBSCRIPTION_CLOSED:
			// ���Ľ���
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_CLOSED"));
			break;
		case EXOSIP_SUBSCRIPTION_NOANSWER:
			// ����û��Ӧ��
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_NOANSWER"));
			break;
		case EXOSIP_SUBSCRIPTION_PROCEEDING:
			// ���Ĵ����У����� 1xx ״̬��
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_PROCEEDING"));
			break;
		case EXOSIP_SUBSCRIPTION_ANSWERED:
			// ����Ӧ�𣬷��� 200 ״̬��
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_ANSWERED"));
			break;
		case EXOSIP_SUBSCRIPTION_REDIRECTED:
			// �����ض���
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_REDIRECTED"));
			break;
		case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
			// ��������ʧ��
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_REQUESTFAILURE"));
			break;
		case EXOSIP_SUBSCRIPTION_SERVERFAILURE:
			// ���ķ�����ʧ��
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_SERVERFAILURE"));
			break;
		case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:
			// ����ȫ��ʧ��
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_GLOBALFAILURE"));
			break;
		case EXOSIP_SUBSCRIPTION_NOTIFY:
			// һ���µ���֪ͨ
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_NOTIFY"));
			break;
		case EXOSIP_SUBSCRIPTION_RELEASED:
			// ���������ı��ͷ�
			OutputDebugString(_T("EXOSIP_SUBSCRIPTION_RELEASED"));
			break;
		case EXOSIP_IN_SUBSCRIPTION_NEW:
			// һ���µĶ�������
			OutputDebugString(_T("EXOSIP_IN_SUBSCRIPTION_NEW"));
			break;
		case EXOSIP_IN_SUBSCRIPTION_RELEASED:
			// ���Ľ���
			OutputDebugString(_T("EXOSIP_IN_SUBSCRIPTION_RELEASED"));
			break;
		case EXOSIP_NOTIFICATION_NOANSWER:
			// ֪ͨ����Ӧ��
			OutputDebugString(_T("EXOSIP_NOTIFICATION_NOANSWER"));
			break;
		case EXOSIP_NOTIFICATION_PROCEEDING:
			// ֪ͨ�������У����� 1xx ״̬��
			OutputDebugString(_T("EXOSIP_NOTIFICATION_PROCEEDING"));
			break;
		case EXOSIP_NOTIFICATION_ANSWERED:
			// ֪ͨ����Ӧ�𣬷��� 200 ״̬��
			OutputDebugString(_T("EXOSIP_NOTIFICATION_ANSWERED"));
			break;
		case EXOSIP_NOTIFICATION_REDIRECTED:
			// ֪ͨ�����ض���
			OutputDebugString(_T("EXOSIP_NOTIFICATION_REDIRECTED"));
			break;
		case EXOSIP_NOTIFICATION_REQUESTFAILURE:
			// ֪ͨ������ʧ��
			OutputDebugString(_T("EXOSIP_NOTIFICATION_REQUESTFAILURE"));
			break;
		case EXOSIP_NOTIFICATION_SERVERFAILURE:
			// ֪ͨ��������ʧ��
			OutputDebugString(_T("EXOSIP_NOTIFICATION_SERVERFAILURE"));
			break;
		case EXOSIP_NOTIFICATION_GLOBALFAILURE:
			// ֪ͨ��ȫ��ʧ��
			OutputDebugString(_T("EXOSIP_NOTIFICATION_GLOBALFAILURE"));
			break;
		default:
			break;
		}
	}

	return 0;
}