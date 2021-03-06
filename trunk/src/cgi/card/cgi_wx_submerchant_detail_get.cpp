#include "hoosho_cgi_card.h"
#include "wx_https_req.h"

class CgiWxSubmerchantGet: public HooshoCgiCard
{
public:
	CgiWxSubmerchantGet() :
			HooshoCgiCard(FLAG_POST_ONLY, "config_card.ini", "logger.properties_card", true)
	{

	}

	bool InnerProcess()
	{
		string strMerchantid = (string) GetInput().GetValue("merchant_id");

		EMPTY_STR_RETURN(strMerchantid);
		SubmerchantInfo stSubmerchantInfo(m_table_name_submerchant_info);
		stSubmerchantInfo.m_id = strMerchantid;
		string strErrMsg;
		CHECK_DB(stSubmerchantInfo,strErrMsg);
		if(stSubmerchantInfo.m_uin != m_user_info.m_uin)
		{
			LOG4CPLUS_ERROR(logger, "merchant_id not match uin");
			DoReply(CGI_RET_CODE_NO_PREVILEDGES);
			return true;
		}

		/*
		int iRet;
		string strMyAccessToken = "";
		string strPostData = "{\"merchant_id\": " + strMerchantid + "}";
		string strSubmerchantInfo = "";
		for(int i = 0; i < 2; i++)
		{
			iRet = WXHttpsReq::WXAPIGetMyAccessToken(WX_HX_PLATFORM_DEV_APPID, WX_HX_PLATFORM_DEV_SECRET, m_cache_ip, m_cache_port_vc, strMyAccessToken, strErrMsg);
			if(iRet != 0)
			{
				LOG4CPLUS_ERROR(logger, "WXAPIGetMyAccessToken failed, errmsg = " << strErrMsg);
				DoReply(CGI_RET_CODE_SERVER_BUSY);
				return true;
			}
			LOG4CPLUS_DEBUG(logger, "my access_token = " << strMyAccessToken);
			iRet = WXHttpsReq::WXAPISubmerchantGet(strMyAccessToken, strPostData, strSubmerchantInfo, strErrMsg);
			if(iRet != 0)
			{
				if(iRet == 40001)
				{
					if(!WXHttpsReq::AccessTokenDelete(m_cache_ip, m_cache_port_vc, strErrMsg))
					{
						LOG4CPLUS_ERROR(logger, "WXHttpsReq::AccessTokenDelete failed, errmsg = " << strErrMsg);
						DoReply(CGI_RET_CODE_SERVER_BUSY);
						return true;
					}
					continue;
				}
				LOG4CPLUS_ERROR(logger, "WXHttpsReq::WXAPISubmerchantGet failed, errmsg = " << strErrMsg);
				DoReply(CGI_RET_CODE_SERVER_BUSY);
				return true; 
			}
			break;
		}

		Json::Value oJson;
		Json::Reader reader;
		if(!reader.parse(strSubmerchantInfo, oJson, false))
		{
			LOG4CPLUS_ERROR(logger, "parse json error, merchant_info = " << strSubmerchantInfo);
			DoReply(CGI_RET_CODE_SERVER_BUSY);
			return true;
		}
		stSubmerchantInfo.m_brand_name = oJson["brand_name"].asString();
		stSubmerchantInfo.m_logo_url = oJson["logo_url"].asString();

		if(oJson["status"].asString() ==  "CHECKING")
		{
			stSubmerchantInfo.m_status = SUBMERCHANT_NOT_VERIFY;
		}
		else if(oJson["status"].asString() == "APPROVED")
		{
			stSubmerchantInfo.m_status = SUBMERCHANT_PASS;
		}
		else if(oJson["status"].asString() ==  "REJECTED")
		{
			stSubmerchantInfo.m_status = SUBMERCHANT_NOT_PASS;
		}
		else if(oJson["status"].asString() ==  "EXPIRED")
		{
			stSubmerchantInfo.m_status = SUBMERCHANT_EXPIRED;
		}
		else
		{
			LOG4CPLUS_ERROR(logger, "status = " << oJson["status"] << " not define");
		}

		stSubmerchantInfo.m_extra_data = strSubmerchantInfo;
		UPDATE_DB(stSubmerchantInfo,strErrMsg);
		*/


		lce::cgi::CAnyValue anyValue;
		stSubmerchantInfo.ToAnyValue(anyValue);
		GetAnyValue()["info"] = anyValue;

//		ostringstream ossInfo;
//		ossInfo.str("");
//		ossInfo <<  "{\"ec\": 0, \"info\": " << strSubmerchantInfo << ", \"status\":  " << stSubmerchantInfo.m_status <<  ", \"wx_msg\": \"" << stSubmerchantInfo.m_wx_msg << "\" }";
//		SetOutputJson(ossInfo.str());

		DoReply(CGI_RET_CODE_OK);
		return true;
	}
};

int main()
{
	CgiWxSubmerchantGet cgi;
	if (!cgi.Run())
	{
		return -1;
	}
	return 0;
}
