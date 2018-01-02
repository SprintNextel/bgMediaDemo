// UASDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "eXosip2/eXosip.h"
#include "eXosip2.h"

#include <iostream>
#include <WinSock2.h>


int _tmain(int argc, _TCHAR* argv[])
{
	const char *sour_call = "sip:140@127.0.0.1";
	const char *dest_call = "sip:133@127.0.0.1:15060";  //client ip

	// ��ʼ��eXosip
	struct eXosip_t context_eXosip;
	int errCode = eXosip_init(&context_eXosip);

	if (errCode != 0)
	{
		std::cout<<"ע�� eXosip ʧ�ܣ�"<<std::endl;
		return errCode;
	}
	else
		std::cout<<"ע�� eXosip �ɹ���"<<std::endl;


	// ��UAC�Լ��Ķ˿�15060��������
	errCode = eXosip_listen_addr(&context_eXosip, IPPROTO_UDP, NULL, 15061, AF_INET, 0);
	if (errCode != 0)
	{
		std::cout<<"��ʼ�������ʧ�ܣ�"<<std::endl;
		eXosip_quit(&context_eXosip);
		return errCode;
	}
	else
		std::cout<<"��ʼ�������ɹ���"<<std::endl;


	while (true)
	{
		// �����Ƿ�����Ϣ����
		eXosip_event_t *event = eXosip_event_wait(&context_eXosip, 0, 50);

		// �����⼸�������Э��ջ��Ҫ�õģ�Ŀǰ����֪��������
		eXosip_lock(&context_eXosip);
		eXosip_default_action(&context_eXosip, event);
		eXosip_automatic_refresh(&context_eXosip);
		eXosip_unlock(&context_eXosip);

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
					std::cout<<"�յ�����Ϣ�� MESSAGE ��Ϣ������Ϊ��"<<body->body<<std::endl;
				}

				// ����Э��Ҫ������Ҫ����OK��Ϣ
				osip_message_t *answer = NULL;
				eXosip_message_build_answer(&context_eXosip, event->tid, 200, &answer);
				eXosip_message_send_answer(&context_eXosip, event->tid, 200, answer);
			}
			break;
		case EXOSIP_CALL_INVITE:
			{
				const char *host = event->request->req_uri->host;
				const char *port = event->request->req_uri->port;
				const char *username = event->request->req_uri->username;
				const char *password = event->request->req_uri->password;
				std::cout<<"�յ���һ�� INVITE ��Ϣ�����ͣ�"<<host<<":"<<port<<", �û�����"<<username<<", ���룺"<<(password == NULL ? "��" : password)<<std::endl;

				// ȡ����Ϣ�壬����Ĭ����Ϊ����sdp��ʽ
				sdp_message_t *remote_sdp = eXosip_get_remote_sdp(&context_eXosip, event->did);

				int call_id = event->cid;
				int dialog_id = event->did;

				eXosip_lock(&context_eXosip);
				errCode = eXosip_call_send_answer(&context_eXosip, event->tid, 180, NULL);

				osip_message_t *answer = NULL;
				errCode = eXosip_call_build_answer(&context_eXosip, event->tid, 200, &answer);
				if (errCode != 0)
				{
					std::cout<<"����������Ϣ�����ã�����Ӧ��"<<std::endl;
					eXosip_call_send_answer(&context_eXosip, event->tid, 400, NULL);
				}
				else
				{
					// 
					char msg[4096] = {0};
					sprintf_s(msg, 4096,
						"v=0\r\n"  
						"o=anonymous 0 0 IN IP4 0.0.0.0\r\n"  
						"t=1 10\r\n"  
						"a=username:rainfish\r\n"  
						"a=password:123\r\n");

					// ���ûظ���SDP��Ϣ��
					osip_message_set_body(answer, msg, strlen(msg));
					osip_message_set_content_type(answer, "application/sdp");

					eXosip_call_send_answer(&context_eXosip, event->tid, 200, answer);
					std::cout<<"�ɹ�����200Ӧ��"<<std::endl;
				}
				eXosip_unlock(&context_eXosip);

				// ��ʾ����sdp��Ϣ���ص�attribute�����ݣ�����ƻ�������ǵ���Ϣ
				std::cout<<"INFO ����Ϊ��"<<std::endl;
				int pos = 0;
				while (!osip_list_eol(&(remote_sdp->a_attributes), pos))
				{
					sdp_attribute_t *at = (sdp_attribute_t *)osip_list_get(&remote_sdp->a_attributes, pos);
					std::cout<<"FIELD : "<<at->a_att_field<<" ,VALUE : "<<at->a_att_value<<std::endl;

					++pos;
				}
			}
			break;
		case EXOSIP_CALL_ACK:
			std::cout<<"�յ� ACK ..."<<std::endl;
			break;
		case EXOSIP_CALL_CLOSED:
			{
				std::cout<<"�յ� Closed ..."<<std::endl;
				//
				osip_message_t *answer = NULL;
				errCode = eXosip_call_build_answer(&context_eXosip, event->tid, 200, &answer);
				if (errCode != 0)
				{
					std::cout<<"����������Ϣ�����ã�����Ӧ��"<<std::endl;
					eXosip_call_send_answer(&context_eXosip, event->tid, 400, NULL);
				}
				else
				{
					eXosip_call_send_answer(&context_eXosip, event->tid, 200, answer);
					std::cout<<"�ɹ�����200Ӧ��"<<std::endl;
				}
			}
			break;
		case EXOSIP_CALL_MESSAGE_NEW:
			std::cout<<"EXOSIP_CALL_MESSAGE_NEW"<<std::endl;
			{
				if (MSG_IS_INFO(event->request))
				{
					eXosip_lock(&context_eXosip);
					osip_message_t *answer = NULL;
					errCode = eXosip_call_build_answer(&context_eXosip, event->tid, 200, &answer);
					if (errCode == 0)
						eXosip_call_send_answer(&context_eXosip, event->tid, 200, answer);
					eXosip_unlock(&context_eXosip);
				}

				osip_body_t *body = NULL;
				osip_message_get_body(event->request, 0, &body);
				std::cout<<"MESSAGE ���ɣ�"<<body->body<<std::endl;
			}
			break;
		default:
			break;
		}
	}

	return 0;
}

