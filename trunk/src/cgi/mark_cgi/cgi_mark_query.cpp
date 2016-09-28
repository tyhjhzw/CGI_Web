#include "cgi_mark_server.h"
#include "proto_io_tcp_client.h"
#include <time.h>
#include "msg.pb.h"

class CgiMarkQuery: public CgiMarkServer
{
public:
	CgiMarkQuery():
		CgiMarkServer(0, "config.ini", "logger.properties")
	{		
	}
	
	bool InnerProcess()
	{	
		//1.get HTTP params
		uint64_t openid_md5 = strtoul(((string)GetInput().GetValue("openid_md5")).c_str(), NULL, 10);	

		if(0 == openid_md5)
		{
			DoReply(CGI_RET_CODE_INVALID_PARAM);
			LOG4CPLUS_ERROR(logger, "INVALID OPENID MD5");
			return true;			
		}

		//2.Build requestMsg protobuf
		string strErrMsg = "";
		::hoosho::msg::Msg stRequestMsg;
		::hoosho::msg::Msg stResponseMsg;

		::hoosho::msg::MsgHead* stRequestMsgHead = stRequestMsg.mutable_head();
		stRequestMsgHead->set_cmd(::hoosho::msg::QUERY_USER_MARK_REQ);
		stRequestMsgHead->set_seq(time(NULL));

		::hoosho::usermark::QueryUserMarkReq* stMarkQueryReq = stRequestMsg.mutable_usermark_query_req();
		stMarkQueryReq->set_openid_md5(openid_md5);

		//3.send to server, and recv responseMsg protobuf
		common::protoio::ProtoIOTcpClient ioclient(markServerIP, markServerPort);

		int iRet;
		iRet = ioclient.io(stRequestMsg, stResponseMsg, strErrMsg);

		if(iRet != 0)
		{
			LOG4CPLUS_DEBUG(logger, "ProtoIOTcpClient IO failed, errmsg = " << strErrMsg);
			DoReply(CGI_RET_CODE_SERVER_BUSY);
			return true;
		}

		//LOG4CPLUS_DEBUG(logger, "response Msg: \n" << stResponseMsg.Utf8DebugString());	
		//parse response
		const ::hoosho::msg::MsgHead& stHead = stResponseMsg.head();
		if(stHead.cmd() != ::hoosho::msg::QUERY_USER_MARK_RES)
		{
			DoReply(CGI_RET_CODE_SERVER_BUSY);
			LOG4CPLUS_ERROR(logger, "response.cmd="<<stHead.cmd()<<", unknown, fuck!!!");
			return true;	
		}

		if(stHead.result() != ::hoosho::msg::E_OK)
		{
			DoReply(CGI_RET_CODE_SERVER_BUSY);
			LOG4CPLUS_ERROR(logger, "response.result="<<stHead.result());
			return true;	
		}

		//4.Build strResponse Json from responseMsg protobuf
		lce::cgi::CAnyValue user_mark_list;
		::hoosho::usermark::QueryUserMarkRes stMarkQueryRes = stResponseMsg.usermark_query_res();
		for(int i=0; i<stMarkQueryRes.usermark().size(); i++)
		{
			::hoosho::commstruct::UserMark userMark = stMarkQueryRes.usermark(i);
			lce::cgi::CAnyValue item;		
			item["openid_md5"] = int_2_str(userMark.openid_md5());
			item["pa_appid_md5"] = int_2_str(userMark.pa_appid_md5());
			item["pa_openid"] = int_2_str(userMark.pa_openid());
			item["is_follow"] = userMark.is_follow();
			item["create_ts"] = int_2_str(userMark.create_ts());

			user_mark_list.push_back(item);
		}

		GetAnyValue()["user_mark_list"] = user_mark_list;
		DoReply(CGI_RET_CODE_OK);
		return true;
	}

};

int main(int argc, char** argv)
{
	CgiMarkQuery cgi;
	if(!cgi.Run())
	{
		return -1;
	}
	return 0;
}

