#ifndef KITCHEN_HPP
#define KITCHEN_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <mutex>
#include <random>
#include "turtlebot_navigator/jobqueue.hpp"
#include "turtlebot_navigator/jobagent.hpp"

class DishOrder{
    public:
    std::string dish_name;
    bool isOrderDone = false;
};


class Dish {
    public:
    int prep_time; 

    std::string dish_name;
};

class WaiterAgent : public JobAgent<DishOrder> {
protected:
    void processJob(std::unique_ptr<DishOrder> job) {
    }
};


class Menu {
   public:
    std::string name;
    std::vector<Dish> dishes_list;

    Dish getdish(std::string dish_name){
        auto dish_it = std::find_if(dishes_list.begin(), dishes_list.end(), [dish_name](Dish dish){return dish.dish_name == dish_name;});
        if (dish_it == dishes_list.end()){
            return Dish(); // si no esta en la lista podemos imprimir std::cout<<"Please enter one of our available dishes"<<std::endl;
        }
        else {
            return *dish_it;
        }
    }

    Menu(){}

    Menu(std::string inputname){
        std::string line;
        std::string token;
        std::string dishName;
        int prepTime;

        std::string package_share_directory = ament_index_cpp::get_package_share_directory("turtlebot_navigator");
        std::cout<<package_share_directory<<std::endl;

        std::ifstream file(package_share_directory+"/Menu/Mexicano.txt");
        if (file.is_open()){
        

            while (std::getline(file,line)){

                Dish dish;
                std::stringstream linestream(line);
                std::getline(linestream,token,',');
                std::cout<<token<<" ";
                prepTime = stoi(token);
                
                std::getline(linestream,token,',');
                std::cout<<token<<std::endl;
                dishName = token;
                dishes_list.push_back({prepTime,dishName});
            }
        }
    }
};

class ChefAgent : public JobAgent<DishOrder> {
private:
    std::vector<Dish> _preparing_orders;
    Menu& _menu;
    bool _cooking;
    JobQueue<DishOrder>& _preparedOrders;
    std::thread _chef_thread, _cooking_thread;

    void cook_cycle(){
        while(_cooking || _preparing_orders.size() > 0 ){

            // Each preparing dish has remaining time to cook, we will decrease 1 unit from the remaining time
            // each second
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for(int i=0; i < _preparing_orders.size(); i++){
                _preparing_orders[i].prep_time-=1;
            }

            // Check if a preparing dish is done with cooking (remaining time to cook is 1)
            std::vector<Dish>::iterator it = _preparing_orders.begin();
            while(it != _preparing_orders.end()) {
                if((*it).prep_time == 1) {
                    //
                    std::cout<<"Done Preparing "<<(*it).dish_name<<std::endl;

                    auto preparedOrder = std::make_unique<DishOrder>();
                    preparedOrder->dish_name = (*it).dish_name;
                    preparedOrder->isOrderDone = true;
                    _preparedOrders.add(std::move(preparedOrder));
                    //
                    it= _preparing_orders.erase(it);
                }
                else ++it;
            }
                 
        }
    }

    void chef_cycle(){
        while(_cooking){        
            checkForNewJobs();
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }
protected:
    void processJob(std::unique_ptr<DishOrder> order) {
        int sleepSeconds = 1 + random() % 2;
        std::this_thread::sleep_for(std::chrono::seconds(sleepSeconds));
        _preparing_orders.push_back(_menu.getdish(order->dish_name));
    }
public:
    ChefAgent(Menu& menu, JobQueue<DishOrder>& preparedOrders) : _menu(menu), _preparedOrders(preparedOrders) {
        _cooking = false;
    }

    void startCooking() {
        _cooking = true;
        
        _chef_thread = std::thread([this](){
            chef_cycle();
        });
        _cooking_thread = std::thread([this](){
            cook_cycle();
        });
    }

    void stopCooking() {
        _cooking = false;
    }

    void waitToFinish() {
        _chef_thread.join();
        _cooking_thread.join();
    }
};

#endif