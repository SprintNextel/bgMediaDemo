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
				// ׼�����͵�����
				int request_id = 0;				// ע������ID
				int expires = 0;				// ��ʱʱ��
				const char *code = NULL;		// ������
				const char *ip = NULL;			// UAC-IP
				const char *port = NULL;		// UAC-PORT
				const char *content = NULL;		// ����
				bool is_authen_null = true;		// �Ƿ����ύ��֤����
				const char *user_name = NULL;
				const char *algorithm = NULL;
				const char *realm = NULL;
				const char *nonce = NULL;
				const char *response = NULL;
				const char *uri = NULL;

				// ���ȼ��expires
				osip_header_t* header_expires = NULL;
				osip_message_header_get_byname(sip_event->request, "expires", 0, &header_expires);
				if (NULL != header_expires && NULL != header_expires->hvalue)
					expires = atoi(header_expires->hvalue);

				// ע�᷵�أ��ɷ��ͷ�ά��������ID�����շ����պ�ԭ�����ؼ���
				request_id = sip_event->tid;

				// �鿴contact�ֶ�
				osip_contact_t* contact = NULL;
				osip_message_get_contact(sip_event->request, 0, &contact);
				if (NULL != contact)
				{
					code = contact->url->username;
					ip = contact->url->host;
					port = contact->url->port;
				}

				// ���� ע����Ϣ��
				osip_body_t * body = NULL;
				osip_message_get_body(sip_event->request, 0, &body);
				if (NULL != body)
					content = body->body;

				// ��Ȩ��Ϣ
				osip_authorization_t* authentication = NULL;
				osip_message_get_authorization(sip_event->request, 0, &authentication);
				if (NULL == authentication)
					is_authen_null = true;
				else
				{
					is_authen_null = false;
					user_name = authentication->username;
					algorithm = authentication->algorithm;
					realm = authentication->realm;
					nonce = authentication->nonce;
					response = authentication->response;
					uri = authentication->uri;
				}

				//
				// һ��׼������������Ӧ��
				int status = 500;
				if (is_authen_null)
					status = 401;
				else
					status = 200;

				eXosip_lock(sip_context);
				osip_message_t* answer = NULL;
				int result = ::eXosip_message_build_answer(sip_context, request_id, status, &answer);

				if (401 == status)
				{
					// ��SIP��������֤��������֤�������͸��ͻ���
					const char *random = "9bd055";
					const char *alg = "MD5";
					char stream[4096] = {0};
					sprintf_s(stream, 4096, "Digest realm=\"%s\",nonce=\"%s\",algorithm=%s", ip, random, alg);
					osip_message_set_header(answer, "WWW-Authenticate", stream);
				}
				else if (200 == status)
				{
					char header[4096] = {0};
					sprintf_s(header, 4096, "<sip:%s@%s:%s>;expires=%d", code, ip, port, expires);
					osip_message_set_header(answer, "Contact", header);
				}
				else
				{
					// Do nothing ...
				}

				if (OSIP_SUCCESS != result)
					eXosip_message_send_answer(sip_context, request_id, 400, NULL);
				else
					eXosip_message_send_answer(sip_context, request_id, status, answer);

				if (0 == expires)
					eXosip_register_remove(sip_context, request_id);

				eXosip_unlock(sip_context);
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