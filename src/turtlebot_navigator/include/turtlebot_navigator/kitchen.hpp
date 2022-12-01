#ifndef KITCHEN_HPP
#define KITCHEN_HPP

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <mutex>
#include "turtlebot_navigator/jobqueue.hpp"
#include "turtlebot_navigator/jobagent.hpp"

class DishOrder{
    public:
    std::string dish_name;
};


class Dish {
    public:
    int prep_time; 

    std::string dish_name;
};

class Mesero : public JobAgent<int> {
public:
    void processJob(int* job) {
        std::cout << "Soy un mesero y estoy haciendo " << *job << std::endl;
    }
};

class Camarero : public JobAgent<int> {
public:
    void saluda() {}
    void processJob(int* job) {
        std::cout << "Soy un Camarero y proceso trabajo: " << *job << std::endl;
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


    void createJobAgent(std::unique_ptr<JobAgent<int>>& newAgent) {
        newAgent = std::move(std::unique_ptr<JobAgent<int>>(new Camarero()));
    }

    Menu(std::string inputname){
        std::string line;
        std::string token;
        std::string dishName;
        int prepTime;

        int* a = new int[1];
        *a = 3;
        int* b = new int[1];
        *b = 1;

        std::unique_ptr<JobAgent<int>> aAgent;
        createJobAgent(aAgent);
        aAgent->getJobs().add(std::unique_ptr<int>(a));
        aAgent->getJobs().add(std::unique_ptr<int>(b));
        aAgent->checkForNewJobs();

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

class Chef{
    public:

    JobQueue<DishOrder>recibe_order;
    std::vector<Dish>preparing_orders;
    bool cooking;

    Menu menu;
    std::mutex& order_mutex;
    
    Chef(std::mutex& mutex):order_mutex(mutex){}
   
    private:

    Dish start_to_cook(std::unique_ptr<DishOrder> dish_order){
        
       // std::cout<<"Please enter the Dish you want to order"<<menu.getdish<<std::endl;
        //std::cin>>menu.dishes_list; //ordenes de pedido que recibe el chef... no me muestra en la consola la opcion de elegir el platillo
        int time = menu.getdish(dish_order->dish_name).prep_time;
    	std::cout<<"start cooking: "<<dish_order->dish_name<<" "<<"Estimated Time to cook: "<<time<<" ";
        auto dish = menu.getdish(dish_order->dish_name);

        return dish;
    }    

    public:

    void start_cycle(){

        while(cooking){        
            order_mutex.lock();

            while (recibe_order.hasJobs()) {
                std::unique_ptr<DishOrder> order = recibe_order.takeJob();
                preparing_orders.push_back(start_to_cook(std::move(order)));
            }

            // for(size_t i=0; i < recibe_order.size(); i++){
                
            //     preparing_orders.push_back(start_to_cook(recibe_order[i]));

            // }
            // recibe_order.clear();
            order_mutex.unlock();
        }
    }

    void cook_cycle(){


        while(cooking || preparing_orders.size()> 0 ){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            for(int i=0; i < preparing_orders.size(); i++){
                preparing_orders[i].prep_time-=1;
                
            }
            std::vector<Dish>::iterator it = preparing_orders.begin();
            while(it != preparing_orders.end()) {

                if((*it).prep_time == 1) {
                    std::cout<<"Done Preparing "<<(*it).dish_name<<std::endl;
                    it= preparing_orders.erase(it);
                }
                else ++it;
            }
                 
        }
    }
};

#endif