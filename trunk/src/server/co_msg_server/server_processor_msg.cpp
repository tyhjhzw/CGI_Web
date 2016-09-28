#include "server_processor_msg.h"
#include "global_var.h"

IMPL_LOGGER(ServerProcessorMsg, logger);

ServerProcessorMsg::ServerProcessorMsg()
{
}

ServerProcessorMsg::~ServerProcessorMsg()
{
	
}

void ServerProcessorMsg::accept(lce::net::ConnectionInfo & conn)
{
    ConnMapIterator iter = _conn_map.find(conn.get_id());
	if(iter == _conn_map.end())
	{
		_conn_map.insert(std::make_pair(conn.get_id(), &conn));
	}
}

void ServerProcessorMsg::remove(lce::net::ConnectionInfo & conn)
{
	ConnMapIterator iter = _conn_map.find(conn.get_id());
	if(iter != _conn_map.end())
	{
		_conn_map.erase(iter);
	}
}

void ServerProcessorMsg::process_input(lce::net::ConnectionInfo& conn, lce::net::IDatagram& ogram )
{
	lce::net::DatagramStringBuffer& stDatagramStringBuffer = dynamic_cast<lce::net::DatagramStringBuffer&>(ogram);
	::hoosho::msg::Msg stMsg;
	if(!stMsg.ParseFromString(stDatagramStringBuffer._strbuffer))
	{
		LOG4CPLUS_ERROR(logger, "ServerProcessorMsg::process_input fail"
											<<", ParseFromString failed, data.size="<<stDatagramStringBuffer._strbuffer.size());
		conn.close();
		return;
	}

	LOG4CPLUS_DEBUG(logger, "ServerProcessorMsg::process_input, one msg:"<<stMsg.Utf8DebugString());	
	

	//Get respon from msg server, swapcontext from main -> coroutine
	
	int64_t coroutine_id = stMsg.head().seq();
	if(coroutine_id == 0)
		return;
		
	CoroutineModuleBase<TimerModule>& co_module = Singleton<CoroutineModuleBase<TimerModule>>::Instance();
	co_module.ReadLock();
	co_module.Resume(coroutine_id, &stMsg);
	co_module.ReleaseLock();

	return;
}

int ServerProcessorMsg::send_datagram(const hoosho::msg::Msg& stMsg)
{
	if(_conn_map.empty())
	{
		LOG4CPLUS_DEBUG(logger, "ServerProcessorMsg::send_datagram failed, connection not found");
		return -1;
	}
	
	ConnMapIterator iter = _conn_map.begin();
	lce::net::DatagramStringBuffer datagram;
	if(!stMsg.SerializeToString(&datagram._strbuffer))
	{
		LOG4CPLUS_DEBUG(logger, "ServerProcessorMsg::send_datagram failed, msg  SerializeToString failed");
		return -1;
	}
	
	LOG4CPLUS_DEBUG(logger, "ServerProcessorMsg::send_datagram, one msg:"<<stMsg.Utf8DebugString());
	iter->second->write(datagram);
	
	return 0;
}


void ServerProcessorMsg::process_output()
{

}


