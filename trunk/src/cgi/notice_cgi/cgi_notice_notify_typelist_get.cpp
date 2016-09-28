#include "notice_server_cgi.h"
#include "proto_io_tcp_client.h"

class CgiNoticeNotifyTypeListGet: public NoticeServerCgi
{
public:
	CgiNoticeNotifyTypeListGet() :
			NoticeServerCgi(0, "config.ini", "logger.properties")
	{

	}
	bool InnerProcess()
	{
		uint64_t iPaAppidMd5 = strtoul(((string)GetInput().GetValue("pa_appid_md5")).c_str(),NULL,10);
		uint64_t iOpenidMd5 = strtoul(((string)GetInput().GetValue("openid_md5")).c_str(),NULL,10);


		ZERO_INT_RETURN(iOpenidMd5);


		::common::protoio::ProtoIOTcpClient client(m_notice_server_ip, m_notice_server_port);
		std::string strErrMsg = "";
		::hoosho::msg::Msg stRequest;
		::hoosho::msg::Msg stResponse;

		::hoosho::msg::MsgHead* header = stRequest.mutable_head();
		uint32_t iSeq = 1;
		header->set_seq(iSeq);
		header->set_cmd(hoosho::msg::QUERY_NOTICE_NOTIFY_TYPELIST_REQ);
		::hoosho::noticenotify::QueryNoticeNotifyTypeListReq* req = stRequest.mutable_notice_notify_typelist_query_req();
		req->set_pa_appid_md5(iPaAppidMd5);
		req->set_openid_md5(iOpenidMd5);
		req->set_type(0);

		if (client.io(stRequest, stResponse, strErrMsg) < 0)
		{
			LOG4CPLUS_DEBUG(logger, "failed, strErrMsg=" << strErrMsg << std::endl;);

			return -1;
		}
		std::ostringstream oss;
		if (stResponse.head().cmd() == hoosho::msg::QUERY_NOTICE_NOTIFY_TYPELIST_RES && stResponse.head().seq() == iSeq)
		{
			if (stResponse.head().result() == hoosho::msg::E_OK)
			{
				LOG4CPLUS_DEBUG(logger, "succe!!\n"<<stResponse.Utf8DebugString() << std::endl);
				const hoosho::noticenotify::QueryNoticeNotifyTypeListRes& res = stResponse.notice_notify_typelist_query_res();
				int iSize = res.type_status_list_size();
				for (int i = 0; i < iSize; i++)
				{
					lce::cgi::CAnyValue stTypeStatus;

					stTypeStatus["type"] = res.type_status_list(i).type();
					stTypeStatus["status"] = res.type_status_list(i).status();
					GetAnyValue()["type_status_list"].push_back(stTypeStatus);
				}
				DoReply(CGI_RET_CODE_OK);

			}
			else if (stResponse.head().result() == hoosho::msg::E_SERVER_INNER_ERROR)
			{
				LOG4CPLUS_DEBUG(logger, "error!! result=1" << std::endl);
				LOG4CPLUS_DEBUG(logger, stResponse.Utf8DebugString() << std::endl);
				DoReply(CGI_RET_CODE_SERVER_BUSY);
			}
			else
			{
				LOG4CPLUS_DEBUG(logger, "error!! result no 1 or 0" << std::endl);
				LOG4CPLUS_DEBUG(logger, stResponse.Utf8DebugString() << std::endl);
				DoReply(CGI_RET_CODE_SERVER_BUSY);
			}
		}
		else
		{
			LOG4CPLUS_DEBUG(logger, "error!! what 7 package?" << std::endl);
			LOG4CPLUS_DEBUG(logger, stResponse.Utf8DebugString() << std::endl);
			DoReply(CGI_RET_CODE_SERVER_BUSY);

		}

		return true;
	}
};

int main()
{
	CgiNoticeNotifyTypeListGet cgi;
	if (!cgi.Run())
	{
		return -1;
	}
	return 0;
}
