#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <fstream>
#include <sstream>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <mutex>
#include "turtlebot_navigator/kitchen.hpp"
#include <ios>
#include <limits>

// for numeric_limits

int main(){
    std::mutex recibe_order_mutex;
    Chef chef(recibe_order_mutex);
    
    recibe_order_mutex.lock();

    
    std::string input_name;

    std::cout<<"Menu Name: "<<std::endl;
    // std::cin>>input_name;
    input_name = "Mexicano";
    Menu menu (input_name);
    chef.menu = menu;
    chef.cooking = true; 
    std::thread chef_thread([&chef](){chef.start_cycle();});

    WaiterAgent waiterAgent;
    ChefAgent chefAgent(menu, waiterAgent.getJobs());

    std::thread cooking_thread([&chefAgent](){
      chefAgent.startCooking();
    });

    int option =0;
    int running= 1;
    std::string target_name;


    // ver las ordenes pendientes, eliminar algun platillo, ver el tiempo estimado de preparacion, pedir platillo
    while(running){
         std::cout<<"MENU MEXICANO"<<std::endl;
         std::cout<<"1) Pending Orders"<<std::endl;
         std::cout<<"2) Estimated Preparation Time"<<std::endl;
         std::cout<<"3) Add a dish"<<std::endl;
         std::cout<<"4) Delete a Dish"<<std::endl;
         std::cout<<"5) Exit The Menu"<<std::endl;

         std::cout<<"Insert number of option: ";

         std::cin>>option;
         if (std::cin.fail()) {
            std::cout<<"You have to put a number!"<<std::endl;
            std::string dummy;
            std::cin >> dummy;
            std::cout << "Var = " << dummy << std::endl;
            // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
         }

         std::cout << "\n\n\n";
         if(option == 1){
            std::cout<<"Final Order is: "<<std::endl;
            
            auto orders = chef.recibe_order.listJobs();

            if (orders.size() > 0) {
                for(int i=0 ; i< orders.size(); i++){
                    std::cout << orders[i]->dish_name<<" / " <<menu.getdish(orders[i]->dish_name).prep_time <<std::endl;
                }
            }
            else {
                std::cout << "There are no enqueued elements yet." << std::endl;
            }
         }
         else if(option ==2){
            std::cout<<"Which Dish you want to consult? "<<std::endl;
            std::cin>> target_name;
            std::cout<< menu.getdish(target_name).prep_time<<std::endl;
         }
         else if(option ==3){

            std::cout<<"Adding another Dish "<<std::endl;
            std::unique_ptr<DishOrder> order = std::make_unique<DishOrder>();
    
            std::cin>>order->dish_name;            
            chef.recibe_order.add(std::move(order));
            // chef.recibe_order.push_back(order);
            recibe_order_mutex.unlock();

            }
         else if(option ==4){
            std::cout<<"Deleting Dish "<<std::endl;

            }
          else if(option ==5){
            std::cout<<"Good Bye"<<std::endl;
            running = 0;
            chef.cooking = false;
            recibe_order_mutex.unlock();
            }
         else{
            std::cout<<"Option Not Found"<<std::endl;
         }

    }
        
    
    chef_thread.join();
    cooking_thread.join();
    std::cout<<"Done"<<std::endl;
    return 0;
}

