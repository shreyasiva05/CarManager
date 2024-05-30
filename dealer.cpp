// CMSC 341 - Fall 2023 - Project 4
#include "dealer.h"
/*
File: dealer.cpp
Author: Shreya Sivakumar
Date: 12/5/2023
Email: shreyas7@umbc.edu
Description: This program is to implement a car manufactoring company using hash tables data structures which implements the map ADT.
*/
//CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY)
// Name: Default Constructor
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
CarDB::CarDB(int size, hash_fn hash, prob_t probing = DEFPOLCY){
    m_currentCap = size; //setting the m_current size to the size in the parameter
    if(m_currentCap > MAXPRIME){
        m_currentCap = MAXPRIME;
    }else if (m_currentCap < MINPRIME){
        m_currentCap = MINPRIME;
    }else if(!isPrime(m_currentCap)){
        m_currentCap = findNextPrime(m_currentCap);
    }
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_hash = hash;
    m_currProbing = probing;
    m_newPolicy = NONE;
    m_currentTable = new Car[m_currentCap]{EMPTY}; //creating an empty current table
    m_oldTable = nullptr;
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = NONE; //setting the policies to NONE

}
//~CarDB()
// Name: Destructor
// Desc: Setting to default values
// Preconditions: the values weren't initialized
// Postconditions: the values were set
CarDB::~CarDB(){

    m_currentCap = 0;
    m_currentSize = 0;
    m_currNumDeleted = 0;
    m_hash = nullptr;
    m_newPolicy = NONE;
    if (m_currentTable != nullptr){
        delete[] m_currentTable;
        m_currentTable = nullptr;
    }
    if (m_oldTable != nullptr){
        delete[] m_oldTable;
        m_oldTable = nullptr;
    }
    m_oldCap = 0;
    m_oldSize = 0;
    m_oldNumDeleted = 0;
    m_oldProbing = NONE;

}
//changeProbPolicy(prob_t policy)
// Name: Change policy
// Desc: The user can change the collision handling policy of the hash table at the runtime.
// If a change request has been submitted by a user, the new policy will be stored in the variable
// m_newPolicy. Once the next rehash operation is initiated the new policy will be used for the newly created hash table.
// Preconditions: the policy hasn't changed by the user
// Postconditions: the policy has been changed
void CarDB::changeProbPolicy(prob_t policy){
    m_newPolicy = policy;
}
//insert(Car car)
// Name: Insert
// Desc: This function inserts an object into the current hash table.Hash collisions should be resolved
// using the probing policy specified in the m_currProbing variable. We insert into the table
// indicated by m_currentTable. After every insertion we need to check for the proper criteria,
// and if it is required, we need to rehash the entire table incrementally into a new table.
// Preconditions: Nodes weren't inserted
// Postconditions: Inserted the notes
bool CarDB::insert(Car car){
    if (!(getCar(car.getModel(), car.getDealer()) ==EMPTY)){
        return false;//return false if it already exsists
    }
    int hashindex = m_hash(car.getModel()) % m_currentCap; //creating a hash index
    if (m_currentTable[hashindex] == EMPTY){ //checking if the currentable whith that index is empty
        m_currentTable[hashindex] = car; //insert the car
        m_currentSize++;//increment the size after inserting
        return true;
    }
    //Intialize a variable to interate through the while loop
    int iteration = 0;
    while(!(m_currentTable[hashindex].getUsed() == false)){
        if( m_currProbing == QUADRATIC){ //if the policy is quadratic
            hashindex = ((m_hash(car.m_model) % m_currentCap + (iteration * iteration)) % m_currentCap);
        }
        else if ( m_currProbing == DOUBLEHASH){// if the policy is double hash
            hashindex = ((m_hash(car.m_model) % m_currentCap) + iteration * (11-(m_hash(car.m_model) % 11))) % m_currentCap;
        }
        //increment the interation
        iteration++;
        if (iteration >= m_currentCap){
            break; //if it reaches at the end break the while loop
        }
    }
    m_currentTable[hashindex] = car;
    m_currentSize++; //increment the size

    if (lambda()> 0.5){
        rehash(); //call rehash if the labda is more than 50 percent
    }
    if (deletedRatio() > 0.8){
        rehash();//call rehash if the delete ratio is more than 80 percent
    }
    return true;
}
//rehash()
// Name: Rehash
// Desc: This function inserts an object into the current hash table.Hash collisions should be resolved
// using the probing policy specified in the m_currProbing variable. We insert into the table
// indicated by m_currentTable. After every insertion we need to check for the proper criteria,
// and if it is required, we need to rehash the entire table incrementally into a new table.
// The incremental transfer proceeds with 25% of the nodes at a time.
// Once we transferred 25% of the nodes for the first time, the second 25% will be transferred
// at the next operation (insertion or removal). Once all data is transferred to the new table,
// the old table will be removed, and its memory will be deallocated.
// Preconditions: Nodes weren't inserted
// Postconditions: Inserted the notes
void CarDB::rehash(){
    //intialize a new capacity
    int newCap = findNextPrime(m_currentSize - m_currNumDeleted) * 4;
    //first rehash, if the old table is nullptr
    if (m_oldTable==nullptr) {
        int transfer = 0;
        //copy the values from current table to old table
        m_oldTable = m_currentTable;
        //delete the current table and set it to nulltpr
        delete[] m_currentTable;
        m_currentTable = nullptr;
        //setting all current to 0
        m_currentSize = 0;
        m_currentCap = 0;
        m_currNumDeleted = 0;
        //make a new empty current table
        m_currentTable = new Car[newCap]{EMPTY};
        //divide the old size by 25%
        int totattransfer = (0.25 * m_oldSize);
        //loop through until it copies the first 25 % of nodes
        for (int i = 0; i < m_oldCap && transfer < totattransfer; i++) {
            if (!(m_oldTable[i].m_used == false)) {
                //create a hashindex
                int hashindex = m_hash(m_oldTable[i].getModel()) % newCap;
                int iteration = 0;
                //if the hashindex is nto empty
                while (!(m_currentTable[hashindex].m_used == false)) {
                    if( m_oldProbing == QUADRATIC){ //do the quadratic policy
                        hashindex = ((m_hash(m_oldTable[i].getModel()) % m_oldCap + (iteration * iteration)) % m_oldCap);
                    }
                    else if( m_oldProbing == DOUBLEHASH){//do the double hash policy
                        hashindex = ((m_hash(m_oldTable[i].getModel()) % m_oldCap) + iteration * (11-(m_hash(m_oldTable[i].getModel()) % 11))) % m_oldCap;
                    }
                    iteration++; //increment the iteration
                }
                //transfering the old table values to current table values
                m_currentTable[hashindex]= m_oldTable[i];
                //increment the current size
                m_currentSize++;
                transfer++;
                //setting the old table used places to false since it has been transfered and later
                //increment the delete size
                m_oldTable[i].m_used= false;
                m_oldNumDeleted++;
            }

        }
    }
    //this rehash happens when the old table already has the values after copying the values from the current table
    if(m_oldTable!=nullptr){
        int transfer = 0;
        int totattransfer = (0.25 * m_oldSize);
        //loop through until it transfers 25 percent of the old table data to current table data
        for (int i = 0; i < m_oldCap && transfer < totattransfer; i++) {
            if (!(m_oldTable[i].m_used == false)) {
                int hashindex = m_hash(m_oldTable[i].getModel()) % newCap;
                int iteration = 0;
                while (!(m_currentTable[hashindex].m_used == false)) {
                    if( m_oldProbing == QUADRATIC){
                        hashindex = ((m_hash(m_oldTable[i].getModel()) % m_oldCap + (iteration * iteration)) % m_oldCap);
                    }
                    else if( m_oldProbing == DOUBLEHASH){
                        hashindex = ((m_hash(m_oldTable[i].getModel()) % m_oldCap) + iteration * (11-(m_hash(m_oldTable[i].getModel()) % 11))) % m_oldCap;
                    }
                    //increment the iteration to loop through the while loop
                    iteration++;
                }
                //transfer the nodes to the current table and incement the size
                m_currentTable[hashindex]= m_oldTable[i];
                m_currentSize++;
                transfer++;
                m_oldTable[i].m_used= false;
                m_oldNumDeleted++;
            }
        }
        //at the end check whther the old size is empty or equal to num deleted
        if (m_oldSize==m_oldNumDeleted){
            //set all the old table values to 0
            delete[]m_oldTable;
            m_oldTable = nullptr;
            m_oldCap = 0;
            m_oldSize = 0;
            m_oldNumDeleted = 0;
            m_oldProbing = NONE;
        }
    }
}
//remove(Car car)
// Name: Remove
// Desc: This function removes a data point from either the current hash table or the old hash table
// where the object is stored. In a hash table we do not empty the bucket, we only tag it as deleted.
// To tag a removed bucket we can use the member variable m_used in the Car class.
// To find the bucket of the object we should use the proper probing policy for the table
// Preconditions: Nodes weren't set to false
// Postconditions: the get used  were set to false after deleting the nodes
bool CarDB::remove(Car car){
    //initialize a hash index
    int hashindex = m_hash(car.m_model) % m_currentCap;
    int iteration = 0;
    //loop until they found the empty index
    while (!(m_currentTable[hashindex].getUsed() == false)) {
        if (m_currProbing == QUADRATIC) {
            hashindex = ((m_hash(car.m_model) % m_currentCap + (iteration * iteration)) % m_currentCap);
        } else if (m_currProbing == DOUBLEHASH) {
            hashindex = ((m_hash(car.m_model) % m_currentCap) + iteration * (11 - (m_hash(car.m_model) % 11))) % m_currentCap;
        }
        if (m_currentTable[hashindex] == car){
            m_currentTable[hashindex].setUsed(false);
            m_currNumDeleted++;
            //returns true after incrementing the deleted nodes in the current table and setting the used to false
            return true;
        }
        iteration++;
        //loop until the end of the while loop and break after reached
        if (iteration >= m_currentCap) {
            break;
        }
    }
    if (m_oldTable != nullptr){
        int hashindex2 = m_hash(car.m_model) % m_currentCap;
        int iteration2 = 0;

        while (!(m_oldTable[hashindex2].getUsed() == false)) {
            if (m_oldProbing == QUADRATIC) {
                hashindex2 = ((m_hash(car.m_model) % m_oldCap + (iteration * iteration)) % m_oldCap);
            } else if (m_oldProbing == DOUBLEHASH) {
                hashindex2 = ((m_hash(car.m_model) % m_oldCap) + iteration * (11 - (m_hash(car.m_model) % 11))) % m_oldCap;
            }
            if (m_oldTable[hashindex] == car){
                m_oldTable[hashindex].setUsed(false);
                m_oldNumDeleted++;
                //returns true after setting the setused to false
                //and increment the deleted the noded on the old table
                return true;
            }
            iteration2++;
            if (iteration >= m_oldCap) {
                break;
            }
        }
    }
    return false;
}
//getCar(string model, int dealer) const
// Name: get Car
// Desc: This function looks for the Car object with the model and the dealer id in the hash table,
// if the object is found the function returns it, otherwise the function returns empty object.
// If there are two hash tables at the time, the function needs to search both tables.
// Preconditions: get car wasn't implemented
// Postconditions: the get car is used
Car CarDB::getCar(string model, int dealer) const {
    //Initialize a hashindex
    int hashindex = m_hash(model) % m_currentCap;
    //intiialize a variable to iterate through the while loop
    int iteration = 0;
    //checks if it already exsists before the while loop
    if (m_currentTable[hashindex].m_model == model && m_currentTable[hashindex].m_dealer == dealer) {
        return m_currentTable[hashindex]; //returns the index
    }
    //loop until it is not empty
    while (!(m_currentTable[hashindex] == EMPTY)) {
        if (m_currProbing == QUADRATIC) { //quadratic probing
            hashindex = ((m_hash(model) % m_currentCap + (iteration * iteration)) % m_currentCap);
        } else if (m_currProbing == DOUBLEHASH) {//double hash probing
            hashindex = ((m_hash(model) % m_currentCap) + iteration * (11 - (m_hash(model) % 11))) % m_currentCap;
        }
        //check if it is avaible and returns the index
        if (m_currentTable[hashindex].m_model == model && m_currentTable[hashindex].m_dealer == dealer) {
            return m_currentTable[hashindex];
        }
        //increment the iteration
        iteration++;
        //if the loop is over break the loop
        if (iteration >= m_currentCap) {
            break;
        }
    }
    //if old table is not equal to nullptr
    if (m_oldTable!= nullptr){
        //create a hashindex
        int hashindex2 = m_hash(model) % m_oldCap;
        int olditeration = 0;
        // return the index if found before the while loop
        if (m_oldTable[hashindex2].m_model == model && m_oldTable[hashindex2].m_dealer == dealer) {
            return m_oldTable[hashindex2];
        }
        while (!(m_oldTable[hashindex2] == EMPTY)) {
            if (m_oldProbing == QUADRATIC) {
                hashindex2 = ((m_hash(model) % m_oldCap + (olditeration * olditeration)) % m_oldCap);
            } else if (m_oldProbing == DOUBLEHASH) {
                hashindex2 = ((m_hash(model) % m_oldCap) + olditeration * (11 - (m_hash(model) % 11))) % m_oldCap;
            }
            if (m_oldTable[hashindex2].m_model == model && m_oldTable[hashindex2].m_dealer == dealer) {
                return m_oldTable[hashindex2];
            }
            olditeration++;
            if (olditeration >= m_oldCap) {
                break;
            } //loops until the end and breaks after the iteration is equal to m_oldcap
        }
    }
    //retruns empty if not found
    return EMPTY;
}
//lambda() const
// Name: Lamda
// Desc: This function returns the load factor of the current hash table.
// The load factor is the ratio of occupied buckets to the table capacity.
// The number of occupied buckets is the total of available buckets and deleted buckets.
// An occupied bucket is a bucket which can contain either a live data node (available to be used)
// or a deleted node.
// Preconditions: does not return the load factor
// Postconditions: returns the load factor
float CarDB::lambda() const {
    double loadfactor;

    int totalbuckets = m_currentSize;
    //calcualtion for load factor
    loadfactor = double(totalbuckets / m_currentCap);
    //returns the loadfactor
    return loadfactor;

}
//deletedRatio() const
// Name: get Car
// Desc: This function returns the ratio of the deleted buckets to the total number of occupied buckets
// Preconditions: does not return the delete ratio
// Postconditions: returns the delete ratio
float CarDB::deletedRatio() const {
    double deleteratio = double(m_currNumDeleted / m_currentCap) * 100;
    return deleteratio; //returns the delete ratio
}

void CarDB::dump() const {
    cout << "Dump for the current table: " << endl;
    if (m_currentTable != nullptr)
        for (int i = 0; i < m_currentCap; i++) {
            cout << "[" << i << "] : " << m_currentTable[i] << endl;
        }
    cout << "Dump for the old table: " << endl;
    if (m_oldTable != nullptr)
        for (int i = 0; i < m_oldCap; i++) {
            cout << "[" << i << "] : " << m_oldTable[i] << endl;
        }
}
//updateQuantity(Car car, int quantity)
// Name: get Car
// Desc: This function looks for the Car object in the hash table, if the object is
// found the function updates its quantity and returns true, otherwise the function returns false.
// If there are two hash tables at the time, the function needs to search both tables for the
// search operation
// Preconditions: hasnt updated the quantity
// Postconditions: returns boolean
bool CarDB::updateQuantity(Car car, int quantity){
    if ((getCar(car.getModel(), car.getDealer()) ==EMPTY)){
        return false;
    }else{
        Car carquantity = getCar(car.m_model, car.m_dealer);
        carquantity.setQuantity(quantity);
        return true;
    }
}

bool CarDB::isPrime(int number){
    bool result = true;
    for (int i = 2; i <= number / 2; ++i) {
        if (number % i == 0) {
            result = false;
            break;
        }
    }
    return result;
}

int CarDB::findNextPrime(int current){
    //we always stay within the range [MINPRIME-MAXPRIME]
    //the smallest prime starts at MINPRIME
    if (current < MINPRIME) current = MINPRIME-1;
    for (int i=current; i<MAXPRIME; i++) {
        for (int j=2; j*j<=i; j++) {
            if (i % j == 0)
                break;
            else if (j+1 > sqrt(i) && i != current) {
                return i;
            }
        }
    }
    //if a user tries to go over MAXPRIME
    return MAXPRIME;
}

ostream& operator<<(ostream& sout, const Car &car ) {
    if (!car.m_model.empty())
        sout << car.m_model << " (" << car.m_dealer << "," << car.m_quantity<< ")";
    else
        sout << "";
    return sout;
}

bool operator==(const Car& lhs, const Car& rhs){
    // since the uniqueness of an object is defined by model and delaer
    // the equality operator considers only those two criteria
    return ((lhs.m_model == rhs.m_model) && (lhs.m_dealer == rhs.m_dealer));
}
