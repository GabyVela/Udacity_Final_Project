#ifndef JOB_AGENT_HPP
#define JOB_AGENT_HPP
#include "turtlebot_navigator/jobqueue.hpp"
#include <memory>

template<typename T>
class JobAgent {
private:
    JobQueue<T> jobs;
public:
    JobQueue<T>& getJobs() {
        return jobs;
    }

    bool isJobDone(T* job) {
        return false;
    }
protected:
    void checkForNewJobs() {
        while (jobs.hasJobs()) {
            auto job = jobs.takeJob();
            
            processJob(job.get());
        }
    }
    virtual void processJob(std::unique_ptr<T> job) = 0;
};



#endif