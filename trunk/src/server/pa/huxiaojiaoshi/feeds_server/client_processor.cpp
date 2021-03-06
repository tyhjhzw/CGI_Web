#include "client_processor.h"
#include "global_var.h"

IMPL_LOGGER(ClientProcessor, logger);

ClientProcessor::ClientProcessor()
{

}

ClientProcessor::~ClientProcessor()
{

}

void ClientProcessor::accept(lce::net::ConnectionInfo & conn)
{
//    cerr<<"one client connected";
    LOG4CPLUS_TRACE(logger, "one client connected");

    ConnMapIterator iter = _conn_map.find(conn.get_id());
    if(iter == _conn_map.end())
    {
        _conn_map.insert(std::make_pair(conn.get_id(), &conn));
    }
}

void ClientProcessor::remove(lce::net::ConnectionInfo & conn)
{
//    cerr<<"one client disconnected";
    LOG4CPLUS_TRACE(logger, "one client disconnected\n\n");

    ConnMapIterator iter = _conn_map.find(conn.get_id());
    if(iter != _conn_map.end())
    {
        _conn_map.erase(iter);
    }
}


void ClientProcessor::process_input(lce::net::ConnectionInfo& conn, lce::net::IDatagram& ogram )
{
    lce::net::DatagramStringBuffer& stDatagramStringBuffer = dynamic_cast<lce::net::DatagramStringBuffer&>(ogram);
    hoosho::msg::Msg stMsg;
    if(!stMsg.ParseFromString(stDatagramStringBuffer._strbuffer))
    {
        LOG4CPLUS_ERROR(logger, "ClientProcessor::process_input fail"
                        <<", ParseFromString failed, data.size="<<stDatagramStringBuffer._strbuffer.size());
        conn.close();
        return;
    }

    LOG4CPLUS_DEBUG(logger, "process_input, one msg:"<<stMsg.Utf8DebugString());

    uint32_t dwCmd = stMsg.head().cmd();

    if(dwCmd == ::hoosho::msg::J_ADD_FOLLOW_REQ || dwCmd == ::hoosho::msg::J_ADD_LISTEN_REQ
            || dwCmd == ::hoosho::msg::J_GET_LISTEN_DETAIL_REQ)
    {
        FsmFollow* fsm = g_fsm_follow_container->new_fsm();
        if(!fsm)
        {
            LOG4CPLUS_ERROR(logger, "ClientProcessor::process_input fail, new fsm failed");
            conn.close();
        }
        fsm->client_req_event(conn, stMsg);
        return;
    }

    FsmFeeds* fsm = g_fsm_feeds_container->new_fsm();

    if(!fsm)
    {
        LOG4CPLUS_ERROR(logger, "ClientProcessor::process_input fail, new fsm failed");
        conn.close();
    }
    fsm->client_req_event(conn, stMsg);

    return;
}

int ClientProcessor::send_datagram(uint32_t conn_id, const hoosho::msg::Msg& stMsg)
{
    ConnMapIterator iter = _conn_map.find(conn_id);
    if(iter == _conn_map.end())
    {
        LOG4CPLUS_DEBUG(logger, "ClientProcessor::send_datagram failed, connection not found, conn_id="<<conn_id);
        return -1;
    }
    lce::net::ConnectionInfo* conn = iter->second;

    lce::net::DatagramStringBuffer datagram;
    if(!stMsg.SerializeToString(&datagram._strbuffer))
    {
        LOG4CPLUS_DEBUG(logger, "ClientProcessor::send_datagram failed, msg  SerializeToString failed");
        return -1;
    }

    LOG4CPLUS_DEBUG(logger, "send_datagram, one msg:"<<stMsg.Utf8DebugString());
    conn->write(datagram);

    return 0;
}


void ClientProcessor::process_output()
{

}



