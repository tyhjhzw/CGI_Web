#ifndef _HOOSHO_TOKEN_SERVER_EXECUTOR_THREAD_PROCESSOR_H_
#define _HOOSHO_TOKEN_SERVER_EXECUTOR_THREAD_PROCESSOR_H_

#include "executor_thread_queue.h"
#include "util/logger.h"

class ExecutorThreadProcessor
{
public:
    ExecutorThreadProcessor()
    {
    }
    ~ExecutorThreadProcessor()
    {
    }

    int init(int size,int queue_capacity);
    int get_size();
	ExecutorThreadQueue* get_queue(int n);
    int send_request(ExecutorThreadRequestElement& request);
    int poll();

    void process(ExecutorThreadResponseElement& reply);
	void process_pa_token_update_resp(ExecutorThreadResponseElement& reply);
	
private:
    int m_size;
    int m_queue_capacity;
    ExecutorThreadQueue* m_queue_arr;

    int m_lastIndex;

    DECL_LOGGER(logger);
};

#endif

