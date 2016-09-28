#ifndef _XDRIVE_TEST_SERVER_H_
#define _XDRIVE_TEST_SERVER_H_

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include "app/application.h"
#include "net/connectioninfo.h"
#include "net/active_reactor.h"

using namespace log4cplus;

class MsgServer : public lce::app::Application
{
public:
	MsgServer(int argc,char** argv);
	~MsgServer();
	int parse_absolute_home_path();

protected:
	int impl_init();
	void run();


private:
	lce::net::ActiveReactor<lce::net::ConnectionInfo>* _active_reactor;
};

#endif 


