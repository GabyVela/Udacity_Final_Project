# UDACITY FINAL PROJECT 

HOW TO RUN THE PROJECT

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Open the project directory and navigate to RestaurantProject
```bash
cmake -S . -B build
cd build
make
cp -r ../Menu .
./RestaurantProject
```
Write the name of the Menu: Mexican
Select the number of the action for adding a dish to your order: 3
Write the name of the dish: Mole
Do you want to wait until dish is delivered: N
Add another dish to your order: 3
Write the name of the dish: Pozole
Do you want to wait until dish is delivered: Y

The program will show a message that the food is being prepared after that a message that the food is ready will appear.
It should take about 15 to 18 seconds to be ready. 
------------------------------------------------------------------------------------------------------------------------------------------------------------------
HOW DOES THE PROJECT WORKS

The end goal of this project is to simulate a restaurant. There are three subjects involved in this. The chef will take the order of the client and then process it to cook it.
The menu will display in the console and the client can choose the dish he wants to consume.
Within the options, the client can add or delete a dish. There will be a queue of dishes that the chef has to prepare.
The chef will grab the order after a couple of seconds and it will later report into the console when it's finished.
Inside the console you will be able to see when the order is finished.
After the order is ready the waiter will pick it up and move to deliver it.
Inside the console you will be able to see when the order is delivered to the client. 


POINTS THAT ARE ADRESSED IN THIS PROJECT

1. Loops, Functions, I/O
1.1 The project demonstrates an understanding of C++ functions and control structures. 
    Inside the project you will find the functions and control structures that show the logic of the program.

1.2 The project reads data from a file and process the data, or the program writes data to a file. 
    File Mexicano, Americano, Japanesse.

1.3 The project accepts user input and processes the input. 
    Yes. You can input information from the terminal and it will be processed.

2. Object Oriented Programming
2.1 The project uses Object Oriented Programming techniques.
    Yes. There are Classes like ChefAgent, DishOrder, etc. They represent the main actors/concepts of the system.

2.2 Classes use appropriate access specifiers for class members.
    All the classes in this project are in the Kitchen.hpp file. Each of them has functions that are Private, Public or Protected depending of their purpose in the project.

2.3 Class constructors utilize member initialization lists.
    Uses list to initialized objects in file Kitchen.hpp Line: 185

2.4 Classes abstract implementation details from their interfaces.
    Chef and Waiter agent have their start and stop functions to be called by main that internally set up threads and control variables.

2.5 Classes encapsulate behavior.
    The classes in Kitchen.hpp have private andd protected functions that hide the behavior.

2.6 Classes follow an appropriate inheritance hierarchy.
    There is a hierarchy between ChefAgent, WaiterAgent with JobAgent.

2.7 Overloaded functions allow the same function to operate on different parameters.
    JobQueue constructor can set a maximum specified size or a default size if the parameter is not passed.

2.8 Derived class functions override virtual base class functions.
    JobAgent has the function processJob that is a virtual function and ChefAgent and WaiterAgent decide how it will work when it gets executed it.

2.9 Templates generalize functions in the project.
    You can find thee template in file jobagent.hpp

3. Memory Management
3.1 The project makes use of references in function declarations.
    In file Kitchen.cpp you can find references. E.I: Line 105 // std::future<void>& orderDeliveredFuture = orderChecker.createOrderBarrier(orderId);

3.2 The project uses destructors appropriately.
    In this project we dont use destructors, is not necessary for the project as all dynamic memory is handle by smart pointers.

3.3 The project uses scope / Resource Acquisition Is Initialization (RAII) where appropriate.
    The project uses local stack variables when posible and pointers are move to appropiate places.

3.4 The project follows the Rule of 5.
    The dynamic memory management is handle by smart pointers. The project uses move semantics and references to optimize. Custom constructors were not neccesary to implement.

3.5 The project uses move semantics to move data, instead of copying it, where possible.
    You can find the move semantics in Kitchen.cpp Line: 102  // chefAgent.getJobs().add(std::move(order));

3.6 The project uses smart pointers instead of raw pointers.
    You can find smart pointers in Kitchen.cpp Line: 90 // std::unique_ptr<DishOrder> order = std::make_unique<DishOrder>();

4. Concurrency
4.1 The project uses multithreading.
    The Threads we use in this project are in Kitchen.hpp 
    You can find them in ChefAgent(2) and WaiterAgent.

4.2 A promise and future is used in the project.
    The promise and future in this project is in file Kitchen.hpp Line:40 and 41
    Makes the waiter notify when the order is completed. 

4.3 A mutex or lock is used in the project.
    The mutex are in the file jobqueue.hpp.
     It controls the access of the data structure
      
4.4 A condition variable is used in the project.
    The condition variable is in the file jobqueue.hpp line: 15. Allows the project to pause the function until there's space available inside the queue.









