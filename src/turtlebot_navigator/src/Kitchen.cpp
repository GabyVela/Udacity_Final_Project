#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <fstream>
#include <sstream>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <chrono>
#include <mutex>
#include "turtlebot_navigator/jobqueue.hpp"
#include <ios>

// for numeric_limits
#include <limits>

class DishOrder{
    public:
    std::string dish_name;
};


class Dish {
    public:
    int prep_time; 

    std::string dish_name;
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


int main(){
    std::mutex recibe_order_mutex;
    Chef chef(recibe_order_mutex);
    
    recibe_order_mutex.lock();

    JobQueue<int> queue;
    
    std::string input_name;

    std::cout<<"Menu Name: "<<std::endl;
    // std::cin>>input_name;
    input_name = "Mexicano";
    Menu menu (input_name);
    chef.menu = menu;
    chef.cooking = true; 
    std::thread chef_thread([&chef](){chef.start_cycle();});
    std::thread cooking_thread([&chef](){chef.cook_cycle();});


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

