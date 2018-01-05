#include "stdafx.h"

#include "eXosip2/eXosip.h"
#include "eXosip2.h"
#include "UACImp.h"

bgUACImp::bgUACImp()
{

}

bgUACImp::~bgUACImp()
{

}

int bgUACImp::Init(const char *code, const char *ip /* = "0.0.0.0" */, int port /* = 5090 */, const char *name /* = "_bgUAC_" */)
{
	int errCode = 0;

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
		uac_ip_ = ip;
		uac_port_ = port;
		uac_name_ = name;
		uac_code_ = code;
	}

	return errCode;
}

void bgUACImp::Close()
{
	if (sip_context_)
		eXosip_quit(sip_context_);
	sip_context_ = NULL;
}

int bgUACImp::SetUASEnvironment(const char *code, const char *ip, int port)
{
	uas_ip_ = ip;
	uas_port_ = port;
	uas_code_ = code;

	return 0;
}

int bgUACImp::Register()
{
	int errCode = 0;

	// ��������Ȩ��Ϣ
	eXosip_clear_authentication_info(sip_context_);

	// ƴװ��Ҫ�ֶ�
	//char from[4096] = {0};
	//sprintf_s(from, 4096, "sip:%s@%s:%d", uac_code_.c_str(), uac_ip_.c_str(), uac_port_);

	//char to[4096] = {0};
	//sprintf_s(to, 4096, "sip:%s@%s:%d", uas_code_.c_str(), uas_ip_.c_str(), uas_port_);

	// ����ע����Ϣ
	errCode = eXosip_add_authentication_info(sip_context_, uac_code_.c_str(), uac_code_.c_str(), "12345678", "MD5", NULL);

	eXosip_lock(sip_context_);

	// ����ע����Ϣ���õ�ע��ID
	std::string uas_uri = get_uas_sip_uri_();
	std::string uac_uri = get_uac_sip_uri_();
	osip_message_t *reg = NULL;
	int reg_id = eXosip_register_build_initial_register(sip_context_, uac_uri.c_str(), uas_uri.c_str(), NULL, 1800, &reg);

	if (reg == NULL)
		return -1;

	// ����ע����Ϣ
	//errCode = eXosip_register_build_register(sip_context_, reg_id, 1800, &reg);

	// ����ע����Ϣ
	errCode = eXosip_register_send_register(sip_context_, reg_id, reg);

	// �ȴ�ע����
	while (true)
	{
		eXosip_event *sip_event = eXosip_event_wait(sip_context_, 0, 50);
		if (!sip_event)
			continue;

		eXosip_lock(sip_context_);
		eXosip_automatic_action(sip_context_);
		eXosip_unlock(sip_context_);

		bool need_break_loop = false;
		switch (sip_event->type)
		{
		case EXOSIP_CALL_ACK:
			break;
		case EXOSIP_CALL_ANSWERED:
			break;
		case EXOSIP_REGISTRATION_FAILURE:
			{
				// ���״̬��Ϊ401����˵����Ҫ����ע����Ϣ
				// ���״̬��Ϊ������˵��ע��ʧ���ˣ���������
				if (sip_event->response && sip_event->response->status_code == 401)
				{
					eXosip_lock(sip_context_);

					// ȡ����֤��Ϣ
					osip_www_authenticate_t *authen = NULL;
					osip_message_get_www_authenticate(sip_event->response, 0, &authen);
					if (authen == NULL)
					{
						eXosip_unlock(sip_context_);
						continue;
					}

					// �����û���֤��Ϣ
					eXosip_clear_authentication_info(sip_context_);
					char *authen_realm = osip_www_authenticate_get_realm(authen);
					eXosip_add_authentication_info(sip_context_, uac_code_.c_str(),uac_code_.c_str(), "12345678", "MD5", authen_realm);

					// ����ע����Ϣ
					osip_message_t *reg = NULL;
					eXosip_register_build_register(sip_context_, sip_event->rid, 1800, &reg);
					if (reg == NULL)
					{
						errCode = -2;
						need_break_loop = true;
						break;
					}

					// ����ע������
					eXosip_register_send_register(sip_context_, sip_event->rid, reg);

					eXosip_unlock(sip_context_);
				}
				else if (sip_event->response)
				{
					errCode = sip_event->response->status_code;
					need_break_loop = true;
					break;
				}
				else
				{
					errCode = -3;
					need_break_loop = true;
					break;
				}
			}
			break;
		case EXOSIP_REGISTRATION_SUCCESS:
			{
				// ע��ɹ�
				errCode = 0;
				need_break_loop = true;
			}
			break;
		default:
			break;
		}

		if (need_break_loop)
			break;
	}

	return errCode;
}

int bgUACImp::Unregister()
{
	int errCode = 0;

	return errCode;
}

int bgUACImp::Call()
{
	int errCode = 0;

	return errCode;
}

int bgUACImp::ReleaseCall()
{
	int errCode = 0;

	return errCode;
}

int bgUACImp::SendSMS(const char *text)
{
	int errCode = 0;

	if (!text)
		return -1;

	if (strlen(text) > 4095)
		return -2;

	std::string uas_uri = get_uas_sip_uri_();
	std::string uac_uri = get_uac_sip_uri_();

	osip_message_t *message = NULL;
	eXosip_message_build_request(sip_context_, &message, "MESSAGE", uas_uri.c_str(), uac_uri.c_str(), NULL);

	char msg[4096] = {0};
	strcpy_s(msg, 4096, text);
	osip_message_set_body(message, msg, strlen(msg));

	// �����ʽ��xml
	osip_message_set_content_type(message, "text/xml");

	// ������Ϣ
	eXosip_message_send_request(sip_context_, message);

	return errCode;
}

std::string bgUACImp::get_uas_sip_uri_()
{
	char result[4096] = {0};
	sprintf_s(result, 4096, "sip:%s@%s:%d", uas_code_.c_str(), uas_ip_.c_str(), uas_port_);

	return result;
}

std::string bgUACImp::get_uac_sip_uri_()
{
	char result[4096] = {0};
	sprintf_s(result, 4096, "sip:%s@%s", uac_code_.c_str(), uac_ip_.c_str());

	return result;
}