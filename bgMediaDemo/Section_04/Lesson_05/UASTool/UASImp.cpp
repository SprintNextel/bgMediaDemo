#include "stdafx.h"

#include "eXosip2/eXosip.h"
#include "eXosip2.h"
#include "UASImp.h"




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
			break;
		case EXOSIP_REGISTRATION_SUCCESS:
			// �û�ע��ɹ�
			break;
		case EXOSIP_REGISTRATION_FAILURE:
			// �û�ע��ʧ��
			break;
		case EXOSIP_REGISTRATION_REFRESHED:
			// ע���Ѿ���ˢ��
			break;
		case EXOSIP_REGISTRATION_TERMINATED:
			// UA ����ִ��ע��
			break;

		//////////////////////////////////////////////////////////////////////////
		// ͨ����INVITE����¼�
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_CALL_INVITE:
			// �µ�����ͨ��֪ͨ
			break;
		case EXOSIP_CALL_REINVITE:
			// ����ͨ������������һ���µ�����֪ͨ
			break;
		case EXOSIP_CALL_NOANSWER:
			// û��Ӧ��ֱ����ʱ
			break;
		case EXOSIP_CALL_PROCEEDING:
			// Զ��Ӧ�����ڴ���
			break;
		case EXOSIP_CALL_RINGING:
			// �����л�����
			break;
		case EXOSIP_CALL_ANSWERED:
			// ͨ����ʼ
			break;
		case EXOSIP_CALL_REDIRECTED:
			// ����ת��
			break;
		case EXOSIP_CALL_REQUESTFAILURE:
			// ����ʧ��
			break;
		case EXOSIP_CALL_SERVERFAILURE:
			// ������ʧ��
			break;
		case EXOSIP_CALL_GLOBALFAILURE:
			// ȫ��ʧ��
			break;
		case EXOSIP_CALL_ACK:
			// INVITE����200���ͻ��յ�һ��ACK
			break;
		case EXOSIP_CALL_CANCELLED:
			// ͨ����ȡ��
			break;
		case EXOSIP_CALL_TIMEOUT:
			// ͨ����ʱ
			break;

		//////////////////////////////////////////////////////////////////////////
		// ͨ���г���INVITE֮�����������
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_CALL_MESSAGE_NEW:
			// һ���µ���������
			break;
		case EXOSIP_CALL_MESSAGE_PROCEEDING:
			// ���󷵻��� 1xx ״̬��
			break;
		case EXOSIP_CALL_MESSAGE_ANSWERED:
			// ���󷵻� 200 ״̬��
			break;
		case EXOSIP_CALL_MESSAGE_REDIRECTED:
			// ����
			break;
		case EXOSIP_CALL_MESSAGE_REQUESTFAILURE:
			// ����
			break;
		case EXOSIP_CALL_MESSAGE_SERVERFAILURE:
			// ����
			break;
		case EXOSIP_CALL_MESSAGE_GLOBALFAILURE:
			// ����
			break;
		case EXOSIP_CALL_CLOSED:
			// ����ͨ���յ�һ��BYE��Ϣ
			break;

		//////////////////////////////////////////////////////////////////////////
		// �����Ϣ UAC �� UAS �������õ�
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_CALL_RELEASED:
			// ͨ�������ı�����
			break;

		//////////////////////////////////////////////////////////////////////////
		// �ⲿ���õ������յ�����Ӧ����
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_MESSAGE_NEW:
			// һ���µ���������
			break;
		case EXOSIP_MESSAGE_PROCEEDING:
			// ���󷵻��� 1xx ״̬��
			break;
		case EXOSIP_MESSAGE_ANSWERED:
			// ���󷵻� 200 ״̬��
			break;
		case EXOSIP_MESSAGE_REDIRECTED:
			// ����
			break;
		case EXOSIP_MESSAGE_REQUESTFAILURE:
			// ����
			break;
		case EXOSIP_MESSAGE_SERVERFAILURE:
			// ����
			break;
		case EXOSIP_MESSAGE_GLOBALFAILURE:
			// ����
			break;

		//////////////////////////////////////////////////////////////////////////
		// ��ʱ��Ϣ
		//////////////////////////////////////////////////////////////////////////
		case EXOSIP_SUBSCRIPTION_UPDATE:
			// ����һ�� SUBSCRIBE�����ģ� ��Ϣ
			break;
		case EXOSIP_SUBSCRIPTION_CLOSED:
			// ���Ľ���
			break;
		case EXOSIP_SUBSCRIPTION_NOANSWER:
			// ����û��Ӧ��
			break;
		case EXOSIP_SUBSCRIPTION_PROCEEDING:
			// ���Ĵ����У����� 1xx ״̬��
			break;
		case EXOSIP_SUBSCRIPTION_ANSWERED:
			// ����Ӧ�𣬷��� 200 ״̬��
			break;
		case EXOSIP_SUBSCRIPTION_REDIRECTED:
			// �����ض���
			break;
		case EXOSIP_SUBSCRIPTION_REQUESTFAILURE:
			// ��������ʧ��
			break;
		case EXOSIP_SUBSCRIPTION_SERVERFAILURE:
			// ���ķ�����ʧ��
			break;
		case EXOSIP_SUBSCRIPTION_GLOBALFAILURE:
			// ����ȫ��ʧ��
			break;
		case EXOSIP_SUBSCRIPTION_NOTIFY:
			// һ���µ���֪ͨ
			break;
		case EXOSIP_SUBSCRIPTION_RELEASED:
			// ���������ı��ͷ�
			break;
		case EXOSIP_IN_SUBSCRIPTION_NEW:
			// һ���µĶ�������
			break;
		case EXOSIP_IN_SUBSCRIPTION_RELEASED:
			// ���Ľ���
			break;
		case EXOSIP_NOTIFICATION_NOANSWER:
			// ֪ͨ����Ӧ��
			break;
		case EXOSIP_NOTIFICATION_PROCEEDING:
			// ֪ͨ�������У����� 1xx ״̬��
			break;
		case EXOSIP_NOTIFICATION_ANSWERED:
			// ֪ͨ����Ӧ�𣬷��� 200 ״̬��
			break;
		case EXOSIP_NOTIFICATION_REDIRECTED:
			// ֪ͨ�����ض���
			break;
		case EXOSIP_NOTIFICATION_REQUESTFAILURE:
			// ֪ͨ������ʧ��
			break;
		case EXOSIP_NOTIFICATION_SERVERFAILURE:
			// ֪ͨ��������ʧ��
			break;
		case EXOSIP_NOTIFICATION_GLOBALFAILURE:
			// ֪ͨ��ȫ��ʧ��
			break;
		default:
			break;
		}
	}

	return 0;
}