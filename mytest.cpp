// CMSC 341 - Fall 2023 - Project 4
/*
File: mytest.cpp
Author: Shreya Sivakumar
Date: 12/5/2023
Email: shreyas7@umbc.edu
Description: This is a test file to test the functions of the dealer.cpp file
*/
#include "dealer.h"
#include <random>
#include <vector>
#include <algorithm>
enum RANDOM {UNIFORMINT, UNIFORMREAL, NORMAL, SHUFFLE};
class Random {
public:
    Random(int min, int max, RANDOM type=UNIFORMINT, int mean=50, int stdev=20) : m_min(min), m_max(max), m_type(type)
    {
        if (type == NORMAL){
            //the case of NORMAL to generate integer numbers with normal distribution
            m_generator = std::mt19937(m_device());
            //the data set will have the mean of 50 (default) and standard deviation of 20 (default)
            //the mean and standard deviation can change by passing new values to constructor
            m_normdist = std::normal_distribution<>(mean,stdev);
        }
        else if (type == UNIFORMINT) {
            //the case of UNIFORMINT to generate integer numbers
            // Using a fixed seed value generates always the same sequence
            // of pseudorandom numbers, e.g. reproducing scientific experiments
            // here it helps us with testing since the same sequence repeats
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_unidist = std::uniform_int_distribution<>(min,max);
        }
        else if (type == UNIFORMREAL) { //the case of UNIFORMREAL to generate real numbers
            m_generator = std::mt19937(10);// 10 is the fixed seed value
            m_uniReal = std::uniform_real_distribution<double>((double)min,(double)max);
        }
        else { //the case of SHUFFLE to generate every number only once
            m_generator = std::mt19937(m_device());
        }
    }
    void setSeed(int seedNum){
        // we have set a default value for seed in constructor
        // we can change the seed by calling this function after constructor call
        // this gives us more randomness
        m_generator = std::mt19937(seedNum);
    }

    void getShuffle(vector<int> & array){
        // the user program creates the vector param and passes here
        // here we populate the vector using m_min and m_max
        for (int i = m_min; i<=m_max; i++){
            array.push_back(i);
        }
        shuffle(array.begin(),array.end(),m_generator);
    }

    void getShuffle(int array[]){
        // the param array must be of the size (m_max-m_min+1)
        // the user program creates the array and pass it here
        vector<int> temp;
        for (int i = m_min; i<=m_max; i++){
            temp.push_back(i);
        }
        std::shuffle(temp.begin(), temp.end(), m_generator);
        vector<int>::iterator it;
        int i = 0;
        for (it=temp.begin(); it != temp.end(); it++){
            array[i] = *it;
            i++;
        }
    }

    int getRandNum(){
        // this function returns integer numbers
        // the object must have been initialized to generate integers
        int result = 0;
        if(m_type == NORMAL){
            //returns a random number in a set with normal distribution
            //we limit random numbers by the min and max values
            result = m_min - 1;
            while(result < m_min || result > m_max)
                result = m_normdist(m_generator);
        }
        else if (m_type == UNIFORMINT){
            //this will generate a random number between min and max values
            result = m_unidist(m_generator);
        }
        return result;
    }

    double getRealRandNum(){
        // this function returns real numbers
        // the object must have been initialized to generate real numbers
        double result = m_uniReal(m_generator);
        // a trick to return numbers only with two deciaml points
        // for example if result is 15.0378, function returns 15.03
        // to round up we can use ceil function instead of floor
        result = std::floor(result*100.0)/100.0;
        return result;
    }

private:
    int m_min;
    int m_max;
    RANDOM m_type;
    std::random_device m_device;
    std::mt19937 m_generator;
    std::normal_distribution<> m_normdist;//normal distribution
    std::uniform_int_distribution<> m_unidist;//integer uniform distribution
    std::uniform_real_distribution<double> m_uniReal;//real uniform distribution

};
unsigned int hashCode(const string str);

class Tester{
public:
    bool insertnormalcase();
    bool getcarerrorcase();
    bool getcarnoncollliding();
    bool getcarcollliding();
    bool removenoncollliding();
    bool removecollliding();
    bool rehashinsert();
    bool rehashinsert2();
    bool rehashremove();
    bool rehashremove2();
};
//insertnormalcase()
// Name: Insert normal case
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::insertnormalcase(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    //Using randon to find the range between the Min and max
    Random RndCar(0,4);
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    int id = 1000; //make an id
    for (int i =0; i<5;i++){
        Car dataObj = Car(carModels[i], RndQuantity.getRandNum(),id++, true);
        if (!(cardb.insert(dataObj))){ //if it does not insert properly return false
            result = false;
        }
        else{
            int hashindex = (cardb.m_hash(dataObj.getModel()) % cardb.m_currentCap);
            if (!(cardb.m_currentTable[hashindex] ==dataObj)){ //if it does not exist
                result  = result && false;
            }else{
                result = result && true;
            }

        }
    }
    return result;
}
//getcarnoncollliding()
// Name: Get car non colliding test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::getcarnoncollliding(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    //string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    int id = 1000;
    //loop through to insert the nodes
    for (int i =0; i<5;i++) {
        Car dataObj = Car(carModels[i], RndQuantity.getRandNum(), id++, true);
        cardb.insert(dataObj);
    }
    for (int i = 0; i < 5; i++) {//loop through to use the get car
        Car getCar = cardb.getCar(carModels[i], 1000+i);

        // if the car matches the inserted car return true else return false
        if (getCar.getModel() == carModels[i] &&(getCar.getQuantity() >= 0) && (getCar.getDealer() >= 1000)&&(getCar.getUsed() == true)){
            result  = result && true;
        }
        else{
            result = false;
        }
    }
    return result;
}
//getcarerrorcase()
// Name: Get car error case test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::getcarerrorcase() {
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    bool result = true;
    int id = 1000;
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH); //assigning a cardb
    //loop through to insert the nodes
    for (int i =0; i<5;i++) {
        Car dataObj = Car(carModels[i], RndQuantity.getRandNum(), id++, true);
        cardb.insert(dataObj);
    }
    //make a nonexsistent car
    Car emptycar = cardb.getCar(carModels[3],1006);
    //check if it is used or not
    if (emptycar.getUsed() != false){
        result  = result && false;
    }
    else{
        result = result && true;
    }
    return result;

}
//getcarcollliding()
// Name: Get car colliding test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::getcarcollliding() {
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    //string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    Random RndID(MINID,MAXID);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    //loop through to insert the nodes
    for (int i =0; i<5;i++) {
        Car dataObj = Car(carModels[i], RndQuantity.getRandNum(), 1000+i, true);
        cardb.insert(dataObj);
    }
    for (int i = 0; i < 5; i++) {
        Car getCar = cardb.getCar(carModels[i], 1000+i); // Assuming hashindex is the correct index

        // Check if the retrieved car matches the inserted car
        if (getCar.getModel() == carModels[i] &&(getCar.getQuantity() >= 0) && (getCar.getDealer() >= 1000)&&(getCar.getUsed() == true)){
            result  = result && true;
        }
        else{
            result = result && false;
        }
    }
    return result;
}
//removenoncollliding()
// Name: remove car non colliding test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::removenoncollliding(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    //string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    Car dataObj;
    int id = 1000;
    //loop through to insert the nodes
    for (int i =0; i<5;i++) {
        Car dataObj = Car(carModels[i], RndQuantity.getRandNum(), id++, true);
        cardb.insert(dataObj);
        cardb.remove(dataObj);
    }
    for (int i = 0; i < cardb.m_currentCap; i++) {
        if (cardb.m_currentTable[i].getUsed()){
            result = result && false;
        }
        else{
            result = result && true;
        }
    }
    return result;
}
//removecollliding()
// Name: remove car colliding test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::removecollliding(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    Car dataObj;
    int id = 1000;
    //loop through to insert the nodes
    for (int i =0; i<15;i++) {
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), id++, true);
        cardb.insert(dataObj);
        cardb.remove(dataObj);
    }
    for (int i = 0; i < cardb.m_currentCap; i++) {
        if (cardb.m_currentTable[i].getUsed()){
            result = result && false;
        }
        else{
            result = result && true;
        }
    }
    return result;
}
//rehashinsert2()
// Name: rehash triggers after load factor
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::rehashinsert2(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndID(MINID,MAXID);
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    Car dataObj;

    for (int i =0; i<50;i++) {
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj);
    }
    if (cardb.lambda() < 0.5){
        //inserting manually 5 nodes
        Car dataObj1 = Car("challenger", RndQuantity.getRandNum(), 1005, true);
        Car dataObj2 = Car("gt500", RndQuantity.getRandNum(), 1003, true);
        Car dataObj3 = Car("miura", RndQuantity.getRandNum(), 1004, true);
        Car dataObj4 = Car("gt500", RndQuantity.getRandNum(), 1034, true);
        Car dataObj5 = Car("miura", RndQuantity.getRandNum(), 1036, true);
        cardb.insert(dataObj1);
        cardb.insert(dataObj2);
        cardb.insert(dataObj3);
        cardb.insert(dataObj4);
        cardb.insert(dataObj5);
        //checking whether the old tale is nullptr and otehr conditoins to check whether the rehash works correctly
        if ((cardb.m_oldTable == nullptr) &&(cardb.m_currentSize == 55) &&(cardb.lambda() < 0.5)){
            result = result && true;
        }
        else{
            result = result && false;
        }
    }
    else{
        result = result && false;
    }
    return result;

}
//rehashinsert()
// Name: rehash test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::rehashinsert(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndID(MINID,MAXID);
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    Car dataObj;

    for (int i =0; i<50;i++) {
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj);
    }
    if (cardb.lambda() < 0.5){
        //mannualy inserting 5 nodes
        Car dataObj1 = Car("challenger", RndQuantity.getRandNum(), 1005, true);
        Car dataObj2 = Car("gt500", RndQuantity.getRandNum(), 1003, true);
        Car dataObj3 = Car("miura", RndQuantity.getRandNum(), 1004, true);
        Car dataObj4 = Car("gt500", RndQuantity.getRandNum(), 1034, true);
        Car dataObj5 = Car("miura", RndQuantity.getRandNum(), 1036, true);
        cardb.insert(dataObj1);
        cardb.insert(dataObj2);
        cardb.insert(dataObj3);
        cardb.insert(dataObj4);
        cardb.insert(dataObj5);
        //checks if the current size has been incremented
        if ((cardb.m_currentSize == 55)){
            result = result && true;
        }
        else{
            result = result && false;
        }
    }
    else{
        result = result && false;
    }
    return result;

}
//rehashremove()
// Name: rehash remove test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::rehashremove(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndID(MINID,MAXID);
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    Car dataObj;
    //loop through to insert the nodes
    for (int i =0; i<40;i++) {
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj);
    }

    if (cardb.deletedRatio() < 0.8){
        //remove 80% of the nodes
        for (int i =0; i<cardb.m_currentCap;i++){
            cardb.remove(dataObj);
        }
        //and check if the m_old size is 0 else return false
        if (cardb.m_oldSize == 0){
            result = result && true;
        }
        else{
            result  = result && false;
        }
    }
    else{
        result = result  && false;
    }
    return result;

}
//rehashremove2()
// Name: rehash remove test function
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
bool Tester::rehashremove2(){
    string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
    string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};
    bool result = true;
    Random RndID(MINID,MAXID);
    Random RndCar(0,4);// selects one from the carModels array
    Random RndQuantity(0,50);
    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
    Car dataObj;
    //loop through to insert the nodes
    for (int i =0; i<40;i++) {
        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(), RndID.getRandNum(), true);
        cardb.insert(dataObj);
    }
//if the delte ratio is less than 880 percent
    if (cardb.deletedRatio() < 0.8){
        for (int i =0; i<cardb.m_currentCap;i++){
            cardb.remove(dataObj);
        }
        if ((cardb.m_oldTable == nullptr)&&(cardb.deletedRatio() < 0.8)&&(cardb.m_oldSize == 0)){
            result = result && true;
        }
        else{
            result  = result && false;
        }
    }
    else{
        result = result  && false;
    }
    return result;

}
//unsigned int hashCode(const string str);

string carModels[5] = {"challenger", "stratos", "gt500", "miura", "x101"};
string dealers[5] = {"super car", "mega car", "car world", "car joint", "shack of cars"};

int main(){

//    vector<Car> dataList;
//    Random RndID(MINID,MAXID);
//    Random RndCar(0,4);// selects one from the carModels array
//    Random RndQuantity(0,50);
//    CarDB cardb(MINPRIME, hashCode, DOUBLEHASH);
//    bool result = true;
//
//    for (int i=0;i<49;i++){
//        // generating random data
//        Car dataObj = Car(carModels[RndCar.getRandNum()], RndQuantity.getRandNum(),
//                          RndID.getRandNum(), true);
//        // saving data for later use
//        dataList.push_back(dataObj);
//        // inserting data in to the CarDB object
//        if (!cardb.insert(dataObj)) cout << "Did not insert " << dataObj << endl;
//    }

    // dumping the data m_currentTable[index] ints to the standard output
    //cardb.dump();
    // checking whether all data points are inserted
//    for (vector<Car>::iterator it = dataList.begin(); it != dataList.end(); it++){
//        result = result && (*it == cardb.getCar((*it).getModel(), (*it).getDealer()));
//    }
//    if (result)
//        cout << "All data points exist in the CarDB object!\n";
//    else
//        cout << "Some data points are missing in the CarDB object\n";
    cout<<"**************** Insert *****************"<<endl;
    Tester inserttest;
    if(inserttest.insertnormalcase()){
        cout<<"The Insert for the normal case passed \n";
    } else {
        cout << "The Insert for the normal case failed \n";

    }
    cout<<"**************** Get Car *****************"<<endl;
    Tester getCarerror;
    if(getCarerror.getcarerrorcase()){
        cout<<"The get Car for the error case passed \n";
    } else {
        cout << "The get Car for the error case failed \n";

    }
    Tester getCarnoncollidding;
    if(getCarnoncollidding.getcarnoncollliding()){
        cout<<"The get Car for the non colliding keys case passed \n";
    } else {
        cout << "The get Car for the non colliding keys case failed \n";

    }
    Tester getCarcollidding;
    if(getCarcollidding.getcarcollliding()){
        cout<<"The get Car for the colliding keys case passed \n";
    } else {
        cout << "The get Car for the colliding keys case failed \n";

    }
    cout<<"**************** Remove *****************"<<endl;
    Tester removenoncollidding;
    if(removenoncollidding.removenoncollliding()){
        cout<<"The Remove Car for the non colliding keys case passed \n";
    } else {
        cout << "The Remove Car for the non colliding keys case failed \n";

    }
    Tester removecolliding;
    if(removecolliding.removecollliding()){
        cout<<"The Remove Car for the colliding keys case passed \n";
    } else {
        cout << "The Remove Car for the colliding keys case failed \n";

    }
    cout<<"**************** Rehash *****************"<<endl;
    Tester rehashinsert;
    if(rehashinsert.rehashinsert()){
        cout<<"The rehash after inserting decent nodes case passed \n";
    } else {
        cout << "The rehash after inserting decent nodes case failed \n";

    }
    Tester rehashinsert2;
    if(rehashinsert2.rehashinsert2()){
        cout<<"The rehash is triggered due to load factor case passed \n";
    } else {
        cout << "The rehash is triggered due to load factor case failed \n";

    }
    Tester rehashremove;
    if(rehashremove.rehashremove()){
        cout<<"The rehash after removing decent nodes case passed \n";
    } else {
        cout << "The rehash after removing decent nodes case failed \n";

    }
    Tester rehashremove2;
    if(rehashremove2.rehashremove2()){
        cout<<"The rehash is triggered due to delete ratio case passed \n";
    } else {
        cout <<"The rehash is triggered due to delete ratio case failed \n";

    }
    return 0;
}
unsigned int hashCode(const string str) {
    unsigned int val = 0 ;
    const unsigned int thirtyThree = 33 ;  // magic number from textbook
    for (unsigned int i = 0 ; i < str.length(); i++)
        val = val * thirtyThree + str[i] ;
    return val ;
}
