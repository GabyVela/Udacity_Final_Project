#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <fstream>
#include <sstream>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include <chrono>
#include <mutex>


class Dish {
    public:
    int prep_time;
    /*
    std::this_thread::sleep_for(std::chrono::milliseconds(1));//NOT SURE
    std::chrono::time_point<std::chrono::system_clock> currentTime;//Returns a time point representing the current point in time.
    currentTime = std::chrono::system_clock::now();

    long timeLapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()- currentTime).count(); //duration_cast: Converts the value of dtn into some other duration type, taking into account differences in their periods.
    std::uniform_int_distribution<int> distr(900,1500); // entrega promedio entre 15 y 25 minutos
    
    float prep_time = time_point+timeLapsed;*/

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

        std::string package_share_directory = ament_index_cpp::get_package_share_directory("turtlebot_navigator");
        std::cout<<package_share_directory<<std::endl;

        

        std::ifstream file(package_share_directory+"/Menu/Mexicano.txt");
        if (file.is_open()){
        

            while (std::getline(file,line)){

                Dish dish;
                std::stringstream linestream(line);
                std::getline(linestream,token,',');
                std::cout<<token<<" ";
                dish.prep_time = stoi(token);
                
                std::getline(linestream,token,',');
                std::cout<<token<<std::endl;
                dish.dish_name = token;
                dishes_list.push_back(dish);
            }
        }
    }

};

class DishOrder{
    public:
    std::string dish_name;
    std::vector<DishOrder>dishes_list;
};

class Chef{
    public:
    Menu menu;
    std::mutex& order_mutex;
    
    Chef(std::mutex& mutex):order_mutex(mutex){}

    void start_to_cook(DishOrder dish_order){
        
       // std::cout<<"Please enter the Dish you want to order"<<menu.getdish<<std::endl;
        //std::cin>>menu.dishes_list; //ordenes de pedido que recibe el chef... no me muestra en la consola la opcion de elegir el platillo
        int time = menu.getdish(dish_order.dish_name).prep_time;
    	std::cout<<"start cooking: "<<dish_order.dish_name<<" "<<"Estimated Time to cook: "<<time<<" ";

    }    

    void start_cycle(){

        std::cout<<"Preparing Food"<<std::endl;
        order_mutex.lock();
        std::cout<<" Food Prepared!"<<std::endl;
        order_mutex.unlock();

    }
};



int main(){
    std::cout<<"funciona"<<std::endl;
    std::mutex recibe_order_mutex;
    Chef chef(recibe_order_mutex);
    recibe_order_mutex.lock();

    std::string input_name;

    std::cout<<"Menu Name: "<<std::endl;
    std::cin>>input_name;
    Menu menu (input_name);
    chef.menu = menu;

    DishOrder order;
    order.dish_name= "Enchiladas";
    
    std::cout<<"Dish Name: "<<std::endl;
    std::cin>>order.dish_name;
    chef.start_to_cook(order);

    auto cook_thread = [&chef]() {
        chef.start_cycle();
        
    };
    std::thread thread_object(cook_thread);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    recibe_order_mutex.unlock();
    thread_object.join();
    std::cout<<"Done"<<std::endl;
    return 0;
}

