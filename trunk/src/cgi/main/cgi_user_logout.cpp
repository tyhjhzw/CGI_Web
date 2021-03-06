#include "main_cgi.h"

class CgiUserLogout: public MainCgi
{
     public:
        CgiUserLogout() : MainCgi(0, "config.ini", "logger.properties", CGI_NEED_LOGIN_NO)
        {

        }
        
		bool InnerProcess()
		{
		    //clear cookie & reply
		    lce::cgi::CHttpHeader& stHttpRspHeader =  GetHeader();
        	stHttpRspHeader.SetCookie(m_cookie_name_uin, "", m_cookie_domain, 0, m_cookie_path, false, true);
        	stHttpRspHeader.SetCookie(m_cookie_name_key, "", m_cookie_domain, 0, m_cookie_path, false, true);
        	
            DoReply(CGI_RET_CODE_OK);
            return true;
	    }
};


int main()
{
    CgiUserLogout cgi;
    if (!cgi.Run())
    {
        return -1;
    }
    return 0;
}

