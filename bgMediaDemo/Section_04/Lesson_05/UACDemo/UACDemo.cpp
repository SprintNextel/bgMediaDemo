// UACDemo.cpp : �������̨Ӧ�ó������ڵ㡣
//
// �ο����ϣ�http://blog.csdn.net/hiwubihe/article/details/44339125
//
// ����������У�UAC��UASͨ�ţ�source���Լ��ĵ�ַ��Ŀ���ַ����UAS

#include "stdafx.h"
#include "eXosip2/eXosip.h"
#include "eXosip2.h"

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
	struct eXosip_t *context_eXosip = new struct eXosip_t;
	int errCode = eXosip_init(context_eXosip);

	if (errCode != 0)
	{
		std::cout<<"ע�� eXosip ʧ�ܣ�"<<std::endl;
		return errCode;
	}
	else
		std::cout<<"ע�� eXosip �ɹ���"<<std::endl;

	// ��UAC�Լ��Ķ˿�15060��������
	errCode = eXosip_listen_addr(context_eXosip, IPPROTO_UDP, NULL, 15060, AF_INET, 0);
	if (errCode != 0)
	{
		std::cout<<"��ʼ�������ʧ�ܣ�"<<std::endl;
		eXosip_quit(context_eXosip);
		return errCode;
	}
	else
		std::cout<<"��ʼ�������ɹ���"<<std::endl;

	int flag = 1;

	
	// ȫ���Եı���
	osip_message_t *invite = NULL;	// �����������֮�������

	int call_id = -1;
	int dialog_id = -1;

	while (flag)
	{
		std::cout<<"���������";
		char command_[4096] = {0};
		std::cin.clear();
		std::cin>>command_;
		std::cout<<std::endl;

		std::string command = command_;

		if (command.compare("r") == 0)
		{
			std::cout<<"��δʵ��"<<std::endl;
		}
		else if (command.compare("i") == 0)
		{
			// �����������INVITE
			errCode = eXosip_call_build_initial_invite(context_eXosip, &invite, dest_call, source_call, NULL, "This is a call for conversation");
			if (errCode != 0)
				std::cout<<"��ʼ�� INVITE ʧ��"<<std::endl;
			else
			{
				// ���� SDP����������˵��ffmpeg��av_create_sdp()Ҳ�ǿ�������SDP��Ϣ��
				char body[4096] = {0};
				strcpy_s(body, 4096,
					"v=0\r\n"
					"0=anonymous 0 0 IN IP4 0.0.0.0\r\n"  
					"t=1 10\r\n"  
					"a=username:rainfish\r\n"  
					"a=password:aaaaaa\r\n");

				osip_message_set_body(invite, body, strlen(body));
				osip_message_set_content_type(invite, "application/sdp");

				eXosip_lock(context_eXosip);
				errCode = eXosip_call_send_initial_invite(context_eXosip, invite);
				eXosip_unlock(context_eXosip);

				// ��Ϣ�ѷ��ͣ��ȴ�Ӧ��
				int flag_1 = 1;
				while (flag_1)
				{
					eXosip_event_t *event = eXosip_event_wait(context_eXosip, 0, 200);

					if (event == NULL)
					{
						// ��ʱ��
						std::cout<<"INVITE Ӧ��ȴ���ʱ"<<std::endl;
						break;
					}

					switch (event->type)
					{
					case EXOSIP_CALL_INVITE:
						std::cout<<"�յ�һ���µ� INVITE ����"<<std::endl;
						break;
					case EXOSIP_CALL_PROCEEDING:
						std::cout<<"�յ� 100 trying��Ϣ����ʾ�������ڴ�����"<<std::endl;
						break;
					case EXOSIP_CALL_RINGING:
						std::cout<<"�յ� 180 RingingӦ�𣬱�ʾ���յ�INVITE�����UAS�����򱻽��û�����"<<std::endl;
						std::cout<<"call_id : "<<event->cid<<", dialog_id : "<<event->did<<std::endl;
						break;
					case EXOSIP_CALL_ANSWERED:
						{
							std::cout<<"�յ� 200 OK����ʾ�����Ѿ����ɹ����ܣ��û�Ӧ��"<<std::endl;
							call_id = event->cid;
							dialog_id = event->did;
							std::cout<<"call_id : "<<event->cid<<", dialog_id : "<<event->did<<std::endl;

							// ��ӦackӦ��
							osip_message_t *ack = NULL;
							eXosip_call_build_ack(context_eXosip, dialog_id, &ack);
							eXosip_call_send_ack(context_eXosip, dialog_id, ack);

							// �˳������Ϣѭ��
							flag_1 = 0;
						}
						break;
					case EXOSIP_CALL_CLOSED:
						std::cout<<"�յ�һ�� BYE ��Ϣ"<<std::endl;
						break;
					case EXOSIP_CALL_ACK:
						std::cout<<"�յ�һ�� ACK"<<std::endl;
						break;
					default:
						std::cout<<"�յ�������Ӧ�����ﲻ����"<<std::endl;
						break;
					}

					eXosip_event_free(event);
				}
			}
		}
		else if (command.compare("h") == 0)
		{
			// �Ҷ�
			std::cout<<"ִ�йҶ�"<<std::endl;

			eXosip_lock(context_eXosip);
			eXosip_call_terminate(context_eXosip, call_id, dialog_id);
			eXosip_unlock(context_eXosip);
		}
		else if (command.compare("c") == 0)
		{
			// 
			std::cout<<"�˹���δ���"<<std::endl;
		}
		else if (command.compare("s") == 0)
		{
			// ���� INFO ����
			std::cout<<"���� INFO ��Ϣ"<<std::endl;

			osip_message_t *info = NULL;
			eXosip_call_build_info(context_eXosip, dialog_id, &info);

			char msg[4096] = {0};
			strcpy_s(msg, 4096, "This a sip message (Method : INFO)");
			osip_message_set_body(info, msg, strlen(msg));

			// ��ʽ���������趨��28181Э���ƺ��ǹ̶�ΪXML��
			osip_message_set_content_type(info, "text/plain");
			eXosip_call_send_request(context_eXosip, dialog_id, info);
		}
		else if (command.compare("m") == 0)
		{
			// ���� MESSAGE ��������ʱ��Ϣ
			// �� INFO ��ͬ���ǣ� MESSAGE ��������Ҫ������ INVITE �Ļ�����
			std::cout<<"���� MESSAGE ��Ϣ"<<std::endl;

			osip_message_t *message = NULL;
			eXosip_message_build_request(context_eXosip, &message, "MESSAGE", dest_call, source_call, NULL);

			char msg[4096] = {0};
			strcpy_s(msg, 4096, "This is a MESSAGE message...");
			osip_message_set_body(message, msg, strlen(msg));

			// �����ʽ��xml
			osip_message_set_content_type(message, "text/xml");

			// ������Ϣ
			eXosip_message_send_request(context_eXosip, message);
		}
		else if (command.compare("q") == 0)
		{
			// 
			std::cout<<"�����˳�..."<<std::endl;
			eXosip_quit(context_eXosip);
			flag = 0;
		}
	}

	system("pause");
	return 0;
}

