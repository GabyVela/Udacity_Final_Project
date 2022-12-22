#ifndef JOB_QUEUE_HPP
#define JOB_QUEUE_HPP
#include <vector>
#include <memory>
#include <iostream>
#include <mutex>            
#include <condition_variable>

template <typename T>
class JobQueue {
private:

    std::vector<std::unique_ptr <T>> jobs;
    std::mutex _mu;
    std::condition_variable _cond;
    int _maxSize;


public:

    JobQueue() {
        _maxSize = 100;
    }

    JobQueue(int maxSize){
        _maxSize = maxSize;
    }

    void add(std::unique_ptr<T> item){
        while (true) {
            std::unique_lock<std::mutex> locker(_mu);
            _cond.wait(locker, [this](){ return jobs.size() < _maxSize; });
            jobs.push_back(std::move (item));
            locker.unlock();
            _cond.notify_all();
            return;
        }

    }

    std::vector<T*>listJobs(){
        std::vector<T*>result;
        std::unique_lock<std::mutex> locker(_mu);

        for(int i=0; i < jobs.size(); i++){
            result.push_back(jobs[i].get());
        }

        locker.unlock();
        _cond.notify_all();
    
        return result;
    }

    bool cancelJob(T* job){
        bool found = false;

         while (true) {
            std::unique_lock<std::mutex> locker(_mu);
            

            typename std::vector<std::unique_ptr<T>>::iterator it = jobs.begin();
            while(it != jobs.end()) {

                if((*it).get() == job) {
                    it= jobs.erase(it);
                    found = true;
                    break;
                }
                else ++it;
            }

            locker.unlock();
            _cond.notify_all();
            return found;
        }
        
    }

    std::unique_ptr<T> takeJob(){
        while (true) {
            std::unique_lock<std::mutex> locker(_mu);
            if (jobs.size() == 0) {
                return std::unique_ptr<T>(nullptr);
            }
            std::unique_ptr<T> result = std::move (jobs.front());
            jobs.erase(jobs.begin());
            locker.unlock();
            _cond.notify_all();
            return result;
        }
    }


    bool hasJobs(){
        return jobs.size() > 0;
    }


};



#endif 
