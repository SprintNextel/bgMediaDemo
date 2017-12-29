// UASDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "eXosip2/eXosip.h"

#include <iostream>
#include <WinSock2.h>


int _tmain(int argc, _TCHAR* argv[])
{
	const char *sour_call = "sip:140@127.0.0.1";
	const char *dest_call = "sip:133@127.0.0.1:15060";  //client ip

	// ��ʼ��eXosip
	struct eXosip_t *context_eXosip = NULL;
	int errCode = eXosip_init(context_eXosip);

	if (errCode != 0)
	{
		std::cout<<"ע�� eXosip ʧ�ܣ�"<<std::endl;
		return errCode;
	}
	else
		std::cout<<"ע�� eXosip �ɹ���"<<std::endl;


	// ��UAC�Լ��Ķ˿�15060��������
	errCode = eXosip_listen_addr(context_eXosip, IPPROTO_UDP, NULL, 15061, AF_INET, 0);
	if (errCode != 0)
	{
		std::cout<<"��ʼ�������ʧ�ܣ�"<<std::endl;
		eXosip_quit(context_eXosip);
		return errCode;
	}
	else
		std::cout<<"��ʼ�������ɹ���"<<std::endl;


	while (true)
	{
		// �����Ƿ�����Ϣ����
		eXosip_event_t *event = eXosip_event_wait(context_eXosip, 0, 50);

		// �����⼸�������Э��ջ��Ҫ�õģ�Ŀǰ����֪��������
		eXosip_lock(context_eXosip);
		eXosip_default_action(context_eXosip, event);
		eXosip_automatic_refresh(context_eXosip);
		eXosip_unlock(context_eXosip);

		if (event == NULL)
			continue;

		switch (event->type)
		{
		case EXOSIP_MESSAGE_NEW:
			{
				std::cout<<"EXOSIP_MESSAGE_NEW �յ��µ���Ϣ��"<<std::endl;

				if (MSG_IS_MESSAGE(event->request))
				{
					// ���յ�����Ϣ�� MESSAGE
					osip_body_t *body = NULL;
					osip_message_get_body(event->request, 0, &body);
					std::cout<<"�յ�����Ϣ�� MESSAGE ��Ϣ������Ϊ��"<<body.body<<std::endl;
				}

				// ����Э��Ҫ������Ҫ����OK��Ϣ
				osip_message_t *answer = NULL;
				eXosip_message_build_answer(context_eXosip, event->tid, 200, &answer);
				eXosip_message_send_answer(context_eXosip, event->tid, 200, answer);
			}
			break;
		case EXOSIP_CALL_INVITE:
			{
				const char *host = event->request->req_uri->host;
				const char *port = event->request->req_uri->port;
				const char *username = event->request->req_uri->username;
				const char *password = event->request->req_uri->password;
				std::cout<<"�յ���һ�� INVITE ��Ϣ�����ͣ�"<<host<<":"<<port<<", �û�����"<<username<<", ���룺"<<password<<std::endl;

				// ȡ����Ϣ�壬����Ĭ����Ϊ����sdp��ʽ
				sdp_message_t *remote_sdp = eXosip_get_remote_sdp(context_eXosip, event->did);

				int call_id = event->cid;
				int dialog_id = event->did;

				eXosip_lock(context_eXosip);
				errCode = eXosip_call_send_answer(context_eXosip, event->tid, 180, NULL);
				if (errCode != 0)
				{
				}
				eXosip_unlock(context_eXosip);
			}
			break;
		case EXOSIP_CALL_ACK:
			std::cout<<""<<std::endl;
			break;
		case EXOSIP_CALL_CLOSED:
			std::cout<<""<<std::endl;
			break;
		case EXOSIP_CALL_MESSAGE_NEW:
			std::cout<<""<<std::endl;
			break;
		default:
			break;
		}
	}

	return 0;
}

