// UMBC - CMSC 341 - Fall 2023 - Proj2
#include "satnet.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <vector>

const char * FAIL_STATEMENT = "*****TEST FAILED: ";
const char * PASS_STATEMENT = "*****TEST PASSED: ";

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

class Tester{
    public:
    double sampleTimeMeasurement(SatNet & aNet, int tempArray[], int arraySize){
        double T = 0.0;//stores running times
        clock_t start, stop;//stores the clock ticks while running the program
        start = clock();
        // the algorithm to be measured
        for (int j=0;j<arraySize;j++){
            aNet.findSatellite(tempArray[j]);
        }
        stop = clock();
        T = stop - start;//number of clock ticks the algorithm took
        double measureTime = T/CLOCKS_PER_SEC;//time in seconds
        return measureTime;
    }

    void InsertDelTest(int nodeNum, int expected, bool r1, bool r2){
        cout << "Testing all nodes were inserted or deleted" << endl;
        if (nodeNum == expected){
            cout << PASS_STATEMENT << "all " << nodeNum << " nodes were inserted" << endl;
        }
        else{
            cout << FAIL_STATEMENT << "some nodes were not inserted or got lost in deletion" << endl;
        }

        cout << "BST Properties Check:" << endl;
        if (r1 == true){
            cout << PASS_STATEMENT << "All nodes have a balance faactor of -1, 0, or 1" << endl;
        }
        else{
            cout << FAIL_STATEMENT << "There are children nodes that do not follow the BST properties" << endl;
        }

        cout << "AVL Properties Check:" << endl;
        if (r2 == true){
            cout << PASS_STATEMENT << "All nodes have a balance faactor of -1, 0, or 1" << endl;
        }
        else{
            cout << FAIL_STATEMENT << "There is at least 1 node with a balance factor less than -1 or greater than 1" << endl;
        }
        cout << endl;

    }

    void compareEach(Sat* lhsR, Sat* rhsR, bool & result){
        if ((lhsR == nullptr) && (rhsR == nullptr)){
            return;
        }
        else if ((lhsR == nullptr) && (rhsR != nullptr)){
            result = false;
            return;
        }
        else if((rhsR == nullptr) && (lhsR != nullptr)){
            result = false;
            return;
        }
        else{
            if (lhsR->getID() != rhsR->getID()){
                result = false;
            }
            compareEach(lhsR->getLeft(), rhsR->getLeft(), result);
            compareEach(lhsR->getRight(), rhsR->getRight(), result);
        }
    }

    void BSTPropCheck(Sat* rSat, bool& check){
        //Helper function that checks that every node's left child is smaller than the root node
        // and every parent's right child is greater than the parent
        if (rSat == nullptr){
            return;
        }
        if (rSat->getLeft() == nullptr && rSat->getRight() == nullptr){
        }
        else if (rSat->getLeft() != nullptr && rSat->getRight() != nullptr){
            if (rSat->getLeft()->getID() > rSat->getID() || rSat->getRight()->getID() < rSat->getID()){
                check = false;
            }
        }
        else if ((rSat->getLeft() == nullptr) && (rSat->getRight() != nullptr)){
            if (rSat->getRight()->getID() < rSat->getID()){
                check = false;
            }
        }
        else if ((rSat->getLeft() != nullptr) && (rSat->getRight() == nullptr)){
            if (rSat->getLeft()->getID() > rSat->getID()){
                check = false;
            }
        }

        //traverse tree
        BSTPropCheck(rSat->getLeft(), check);

        BSTPropCheck(rSat->getRight(), check);
    }

    void AVLPropCheck(Sat* rSat, bool& check, SatNet& net){
        //helper function meant to ensure the entire tree is balanced 
        //all nodes in tree must have a balance factor > -1 and < 1
        if (rSat == nullptr){
            return;
        }
        if (check == false){
            return;
        }
        if (net.balanceF(rSat) < -1 || net.balanceF(rSat) > 1){
            check = false;
        }

        //traverse tree
        AVLPropCheck(rSat->getLeft(), check, net);
        AVLPropCheck(rSat->getRight(), check, net);
    }

    //helper test function gets the number of nodes in a tree
    int totalNodes(Sat* r){
        int count = 1;
        if (r == nullptr){
            return 0;
        }
        else{
            count += totalNodes(r->getLeft());
            count += totalNodes(r->getRight());
            //recursively get the total of all nodes
            return count;
        }
    }

    Sat* getMroot(const SatNet& net){return net.m_root;}
    int getMrootID(const SatNet& net){return net.m_root->getID();}
    
};

int main(){
    Tester tester;

    //set up for most functions
    Random idGen(MINID,MAXID);
    Random inclinGen(0,3);  // there are 4 inclination
    Random altGen(0,3);     // there are 4 altitudes

    //SatNet I will use for testing multiple functions
    SatNet network1;
        int Ids[10] = {54513, 11867, 36888, 49871, 67028, 54865, 62498, 77392, 79418, 84872};
        for (int i = 0; i < 10; i++){
            Sat satellite(Ids[i], static_cast<ALT>(altGen.getRandNum()),
                    static_cast<INCLIN>(inclinGen.getRandNum()));
            network1.insert(satellite);
        }

    //Normal insertion checks number of nodes is equal to number of insertions
    //checks balance factor for all node
    //checks that all left children are less than parent and all right children are greater
    {
        cout << "|||||||||||Standard Insert basic test|||||||||||" << endl;
        cout << endl;

        //after inserting 10 elements totalsats counts the number of nodes in a tree 
        int totalSats = tester.totalNodes(tester.getMroot(network1));
        
        //make sure all nodes were inserted
        // expected output: 10 nodes were inserted to the network in a normal case so all 10 should be inserted

        //check that all parent nodes are greater than their left child and less than their right child
        //make sure tree is a binary tree
        // if conditions met function returns true and pass statement couts
        bool result = true;
        tester.BSTPropCheck(tester.getMroot(network1), result);

        //make sure tree is height Balanced
        //checks that all nodes balance factors are either -1, 0, or 1
        bool result2 = true;
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);
        
        tester.InsertDelTest(totalSats, 10, result, result2);
    }

    //Testing inserting a new node with a height > 1
    {
        cout << "|||||||||||Inserting a sat with a height greater than 0|||||||||||" << endl;
        Sat errorSat(11113);
        errorSat.setHeight(2);
        //all new nodes should have a height of
        network1.insert(errorSat);
        bool result = true;
        bool result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        int totalSats = tester.totalNodes(tester.getMroot(network1));
        
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        tester.InsertDelTest(totalSats, 11, result, result2);
    }

    //Inserting an empty object
    {
        cout << "|||||||||||Inserting an empty object|||||||||||" << endl;
        //create an empty object 
        Sat defaultSat;
        //insert it to the SatNet
        network1.insert(defaultSat);
        bool result = true;
        bool result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        int totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        tester.InsertDelTest(totalSats, 12, result, result2);    
    }

    //Inserting an object with the same ID as a node already in the AVL tree
    {
        cout << "|||||||||||Inserting node with duplicate ID|||||||||||" << endl;
        //setting a new node with the same id as a leaf node
        Sat invalidSat(49871);

        //inserting to network 
        //should not insert the node and instead keep the Tree as is
        network1.insert(invalidSat);
        bool result = true;
        bool result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        int totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        //total nodes should remain 12 as no node should be added
        tester.InsertDelTest(totalSats, 12, result, result2);    
        

        //same test now on root ID copy//
        cout << "|||||||||||Inserting node with duplicate ID|||||||||||" << endl;
        //setting a new node with the same id as a leaf node
        Sat invalidSat2(54865);

        //inserting to network 
        //should not insert the node and instead keep the Tree as is
        network1.insert(invalidSat2);
        //reset
        result = true;
        result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        //total nodes should remain 12 as no node should be added
        tester.InsertDelTest(totalSats, 12, result, result2);
    }

    //edge cases inserting elements in increasing order
    {
        cout << "creating unbalanced avl tree to test Insert's balancing" << endl;
        SatNet network2;
        int nums[10] = {10000, 10001, 10002, 10003, 10004, 10005, 10006, 10007, 10008, 10009};

        //insert only three and test
        for (int i = 0; i < 3; i++){
            Sat satellite(nums[i], static_cast<ALT>(altGen.getRandNum()),
                    static_cast<INCLIN>(inclinGen.getRandNum()));
            network2.insert(satellite);
        }
        bool result = true;
        bool result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        int totalSats = tester.totalNodes(tester.getMroot(network2));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network2), result);
        tester.AVLPropCheck(tester.getMroot(network2), result2, network2);

        //total nodes should remain 12 as no node should be added
        tester.InsertDelTest(totalSats, 3, result, result2);

        cout << "adding the rest of them to test" << endl;
        for (int i = 3; i < 10; i++){
            Sat satellite(nums[i], static_cast<ALT>(altGen.getRandNum()),
                    static_cast<INCLIN>(inclinGen.getRandNum()));
            network2.insert(satellite);
        }
        result = true;
        result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        totalSats = tester.totalNodes(tester.getMroot(network2));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network2), result);
        tester.AVLPropCheck(tester.getMroot(network2), result2, network2);

        //total nodes should remain 12 as no node should be added
        tester.InsertDelTest(totalSats, 10, result, result2);
    }

    //Testing Deletion Normal Case
    {
        cout << "Deleteing Sat with ID 0 a leaf node" << endl;
        //using network 1 delete leaf node
        network1.remove(0);
        bool result = true;
        bool result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        int totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        //total nodes should be 11 after taking one off 12-1 
        tester.InsertDelTest(totalSats, 11, result, result2);

        //using network 1 delete internal node
        cout << "Deleting Sat with ID 77392 it is the right child of m_root" << endl;
        network1.remove(tester.getMroot(network1)->getRight()->getID());
        //reset
        result = true;
        result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        //total nodes should be 10 after taking one off 11-1 
        tester.InsertDelTest(totalSats, 10, result, result2);


        //delete root of netwok 1
        cout << "Deleting network1's m_root " << endl;
        network1.remove(tester.getMrootID(network1));
        //reset
        result = true;
        result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        //total nodes should be 9 after taking one off 10-1 
        tester.InsertDelTest(totalSats, 9, result, result2);
    
    }

    //Delete a non existient sat
    {
        cout << "Deleting a node that doesn't exist" << endl;
        Sat DNESat(11999);
        network1.remove(DNESat.getID());
        bool result = true;
        bool result2 = true;
        ////////////////////// Delete below //////////////////////
        cout << endl;
        ////////////////////// Delete above //////////////////////
        int totalSats = tester.totalNodes(tester.getMroot(network1));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(network1), result);
        tester.AVLPropCheck(tester.getMroot(network1), result2, network1);

        //total nodes should be 9 after 9-0 
        tester.InsertDelTest(totalSats, 9, result, result2);
    }

    SatNet network4;
    int teamSize = 10;
    //int tempID = 0;
    int ID = 0;
    for(int i=0;i<teamSize;i++){
        ID = idGen.getRandNum();
        //if (i == teamSize / 2) tempID = ID;//we store this ID for later use
        Sat satellite(ID,
                    static_cast<ALT>(altGen.getRandNum()),
                    static_cast<INCLIN>(inclinGen.getRandNum()));
        network4.insert(satellite);
    }

    SatNet network5;
    int teamSize2 = 10;
    //int tempID = 0;
    int ID2 = 0;
    for(int i=0;i<teamSize2;i++){
        ID2 = idGen.getRandNum();
        //if (i == teamSize / 2) tempID = ID;//we store this ID for later use
        Sat satellite(ID2,
                    static_cast<ALT>(altGen.getRandNum()),
                    static_cast<INCLIN>(inclinGen.getRandNum()));
        network5.insert(satellite);
    }


    SatNet emptyNet;
    //Testing assignment operator
    {
        cout << "Normal assignment operator" << endl;
        network4 = network5;
        bool test = true;
        tester.compareEach(tester.getMroot(network4), tester.getMroot(network5), test);
        if (test == true && (tester.totalNodes(tester.getMroot(network4)) == tester.totalNodes(tester.getMroot(network5)))){
            cout << PASS_STATEMENT << "Tree copied exactly" << endl;
        }
        else{
            cout << FAIL_STATEMENT << "some error assignming over" << endl;
        }
    }

    //Empty assignment
    {
        cout << "assignment operator net 5 = emptynet" << endl;
        network5 = emptyNet;
        bool test = true;
        tester.compareEach(tester.getMroot(network5), tester.getMroot(emptyNet), test);
        if (test == true && (tester.totalNodes(tester.getMroot(network5)) == tester.totalNodes(tester.getMroot(emptyNet)))){
            cout << PASS_STATEMENT << "Tree copied exactly" << endl;
        }
        else{
            cout << FAIL_STATEMENT << "some error assignming over" << endl;
        }
    }

    //empty  = net;
    {
        cout << "assignment operator empty = net4" << endl;
        emptyNet = network4;
        bool test = true;
        tester.compareEach(tester.getMroot(emptyNet), tester.getMroot(network4), test);
        if (test == true && (tester.totalNodes(tester.getMroot(emptyNet)) == tester.totalNodes(tester.getMroot(network4)))){
            cout << PASS_STATEMENT << "Tree copied exactly" << endl;
        }
        else{
            cout << FAIL_STATEMENT << "some error assignming over" << endl;
        }
    }

    SatNet largeInsert;
    int size = 1000;
    int tempIDs[1001] = {0};
    int deleteArr[1000] = {0};
    int id = 0;
    for(int i=0;i<size;i++){
        id = idGen.getRandNum();
        tempIDs[i] = id;//we store this ID for later use
        if (id % 2 == 0){deleteArr[i] = id;}
        Sat satellite(id,
                    static_cast<ALT>(altGen.getRandNum()),
                    static_cast<INCLIN>(inclinGen.getRandNum()));
        largeInsert.insert(satellite);
    }
    //Testing Large Scale Insertion
    {
        cout << "Testing Large Scale Insertion" << endl;
        bool result = true;
        bool result2 = true;
        int totalSats = tester.totalNodes(tester.getMroot(largeInsert));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(largeInsert), result);
        tester.AVLPropCheck(tester.getMroot(largeInsert), result2, largeInsert);

        //total nodes should be 1000
        tester.InsertDelTest(totalSats, 1000, result, result2);    
    }

    //Testing Large Scale Deletion
    {
        int count = 0;
        for (int i = 0; i < 1000; i++){
            if (deleteArr[i] != 0){
                largeInsert.remove(deleteArr[i]);
                count++;
            }
        }
        bool result = true;
        bool result2 = true;
        int totalSats = tester.totalNodes(tester.getMroot(largeInsert));
        //testing BST and AVL properties
        tester.BSTPropCheck(tester.getMroot(largeInsert), result);
        tester.AVLPropCheck(tester.getMroot(largeInsert), result2, network1);

        //total nodes should be 11 after taking one off 12-1 
        int nodesLeft = 1000 - count;
        tester.InsertDelTest(totalSats, nodesLeft, result, result2);
    }
    

    //largescale find
    {
        cout << endl << "Calling Tester::sampleTimeMeasurement(...): " << endl;
        cout << "\tFinding 1000 nodes takes " << tester.sampleTimeMeasurement(network1, tempIDs, size) << " seconds." << endl;
    }
    return 0;
}
