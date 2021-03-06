#ifndef _FEEDS_SERVER_FSM_COLLECT_H_
#define _FEEDS_SERVER_FSM_COLLECT_H_

#include <string>
#include "net/lce_net.h"
#include "msg.pb.h"
#include "executor_thread_queue.h"

class FsmCollectState;
class FsmCollect : public lce::app::TimerHandler
{
public:
    FsmCollect();
    virtual ~FsmCollect();

public:
    void set_state(FsmCollectState& state);

    void client_req_event(lce::net::ConnectionInfo& conn, const hoosho::msg::Msg& stMsg);
	void db_reply_event(const ExecutorThreadResponseElement& element);
    void timeout_event(void* param);

public:
    void cancel_timer();
    void reset_timer(int wait_time);
    void handle_timeout(void* param);

public:
	int reply_timeout();

public:
    uint32_t _id;
    uint32_t _conn_id;
    uint32_t _req_type;
    uint32_t _req_seq_id;
    FsmCollectState* _state;
    long _timer_id;

public:
	CollectInfo _collect_info;
	
private:
    DECL_LOGGER(logger);
};

#endif

