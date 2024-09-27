// UMBC - CMSC 341 - Fall 2023 - Proj2
#include "satnet.h"
SatNet::SatNet(){
    m_root = nullptr;
}

SatNet::~SatNet(){
    clear();
}
void SatNet::insert(const Sat& satellite){
    //cout << "Calling insert()" << endl;
    m_root = recursiveInsert(m_root, satellite);

}

void SatNet::clear(){
    //cout << "Calling clear()" << endl;
    recursiveClear(m_root);
    m_root = nullptr;
}

void SatNet::remove(int id){
    //cout << "Calling remove()" << endl;
    m_root = recursiveRemove(m_root, id);
}

void SatNet::dumpTree() const {
    dump(m_root);
}

void SatNet::dump(Sat* satellite) const{
    if (satellite != nullptr){
        cout << "(";
        dump(satellite->m_left);//first visit the left child
        cout << satellite->m_id << ":" << satellite->m_height;//second visit the node itself
        dump(satellite->m_right);//third visit the right child
        cout << ")";
    }
}

void SatNet::listSatellites() const {
    //cout << "calling listSatellites()" << endl;
    //in order traversal dumping out the info
    ////cout the info left root right

    //check calling on empty satNet
    //check calling on 1
    //check calling on 10;
    recursiveList(m_root);
    cout << m_root->getID() << endl;
}

bool SatNet::setState(int id, STATE state){
    //cout << "calling setState()" << endl;
    //do on sat with id that dont exist
    //do on empty satelie net
    //make sure state is a possible state listed above
    //make sure id is allowed
    return recursiveSetState(m_root, id, state);
}

void SatNet::removeDeorbited(){
    //cout << "calling removeDeorbited()" << endl;
    //might not have called DEORBITED correctly
    //giving it 1 might work
    recursiveFindState(m_root, DEORBITED);
    
}

bool SatNet::findSatellite(int id) const {
    ////cout << "findSatellites" << endl;
    bool found = recursiveFind(m_root, id);
    return found;
}

const SatNet & SatNet::operator=(const SatNet & rhs){
    // prevent self assignment
    if (this == &rhs){
        return *this;
    }
    
    //clear LHS
    clear();
    
    //copy over rhs to lhs
    recursiveOverloaded(rhs.m_root, m_root);

    return *this;

}

int SatNet::countSatellites(INCLIN degree) const{
    //cout << "calling countSatellites()" << endl;
    int total = 0;
    recursiveCountSat(m_root, degree, total);
    return total;
}

//Helper function implementations
Sat* SatNet::recursiveInsert(Sat* rSat, const Sat& sat){
    //cout << "in recursive Insert ";
    const int MIN_BALANCE = -1;
    const int MAX_BALANCE = 1;
    int balanceFactor = 0;

    if (rSat == nullptr){
        //when node has reached the end it is inserted
        Sat* newNode = new Sat(sat.getID(), sat.getAlt(), sat.getInclin(), sat.getState());
        if (newNode->getHeight() != 0){
            newNode->setHeight(0);
            return newNode;
        }
        //cout << "node reached level 0 " << endl;
        return newNode;
    }
    
    if (sat.getID() < rSat->getID()){
        
        rSat->setLeft(recursiveInsert(rSat->getLeft(), sat));
        //cout << "recursive set left finished" << endl;
    }
    else if (sat.getID() > rSat->getID()){
        rSat->setRight(recursiveInsert(rSat->getRight(), sat));
        //cout << "recursive set right finished" << endl;
    }
    else{
        // case where rSat is equal to a node
        return rSat;
    }
    int height = maxHeight(rSat->getLeft(), rSat->getRight());
    rSat->setHeight(height + 1);
    //cout <<"nodes height is " << rSat->getHeight() << endl;
    
    balanceFactor = balanceF(rSat);

    if ((balanceFactor < MIN_BALANCE) && (sat.getID() > rSat->getRight()->getID())){
        //balance factor is less than -1 so right subtree is larger
        //sat node is also the right child so we are in a right right case
        //so we left rotate
        return leftRot(rSat);
    }
    if ((balanceFactor > MAX_BALANCE) && (sat.getID() < rSat->getLeft()->getID())){
        //balance factor is greater than 1 so left subtree is larger
        //sat node is also the left child so we are in a left left case
        //to reslove this we do a single right rotation
        return rightRot(rSat);
    }
    if ((balanceFactor < MIN_BALANCE) && (sat.getID() < rSat->getRight()->getID())){
        //balance factor is less than -1 so right subtree is larger
        //sat node is the left child so we are in a right left case
        // so we do a left right Rotation  
        rSat->setRight(rightRot(rSat->getRight()));
        return leftRot(rSat);
    }
    if ((balanceFactor > MAX_BALANCE) && (sat.getID() > rSat->getLeft()->getID())){
        //balance factor is greater than 1 so left subtree is larger
        //sat node is also the right child so we are in a left right case
        //so we do a right left rotation
        rSat->setLeft(leftRot(rSat->getLeft()));
        return rightRot(rSat);
    }

    return rSat;  
}

int SatNet::maxHeight(Sat* left, Sat* right){
    //cout <<"Running maxHeight()" << endl;
    int leftHeight = 1;
    int rightHeight = 1;
    if (left == nullptr){
        leftHeight = 0;
        //cout << "Left child is a nullptr" << endl;
    }
    if (right == nullptr){
        rightHeight = 0;
        //cout << "right child is a nullptr" << endl;
    }
    
    if (rightHeight == 0 && leftHeight == 0){
        //cout <<"returning 0" << endl;
        return 0;
    }
    else if (rightHeight ==  0){
        //cout << "returning left height" << left->getHeight() << endl;
        return left->getHeight();
    }
    else if (leftHeight == 0){
        //cout << "returning right height" << right->getHeight() << endl;
        return right->getHeight();
    }
    leftHeight = left->getHeight();
    rightHeight = right->getHeight();

    

    if (leftHeight > rightHeight){
        //cout << leftHeight << ">" << rightHeight << endl; 
        return leftHeight;  
    } 
    else{
        //cout << leftHeight << "<" << rightHeight << endl; 
        return rightHeight;
    }
}

Sat* SatNet::leftRot(Sat* node){
    Sat* rightChild = node->getRight();
    Sat* temp = rightChild->getLeft();
    //rotate
    rightChild->setLeft(node);
    node->setRight(temp);
    //update height
    node->setHeight(maxHeight(node->getLeft(), node->getRight()) + 1);
    rightChild->setHeight(maxHeight(rightChild->getLeft(), rightChild->getRight()) + 1);
    return rightChild;
}

Sat* SatNet::rightRot(Sat* node){
    Sat* leftChild = node->getLeft();
    Sat* temp = leftChild->getRight();
    //rotate
    leftChild->setRight(node);
    node->setLeft(temp);
    //update height
    node->setHeight(maxHeight(node->getLeft(), node->getRight()) + 1);
    leftChild->setHeight(maxHeight(leftChild->getLeft(), leftChild->getRight()) + 1);
    return leftChild; 
}

Sat* SatNet::recursiveRemove(Sat* rSat, int id){
    int balanceFactor = 0;
    const int MIN_BALANCE = -1;
    const int MAX_BALANCE = 1;

    //leafnode
    if (rSat == nullptr){
        return rSat;
    }

    if (id < rSat->getID()){
        //recurse left subtree
        rSat->setLeft(recursiveRemove(rSat->getLeft(), id));
    }
    else if(id > rSat->getID()){
        //recurse right subtree
        rSat->setRight(recursiveRemove(rSat->getRight(), id));
    }
    else if (id == rSat->getID()){
        //perform deletion
        //cases
        if (rSat->getLeft() == nullptr && rSat->getRight() == nullptr){
            //no child
            //just copy over data and delete rSat
            delete rSat;
            rSat = nullptr;
        }
        else if (rSat->getLeft() != nullptr && rSat->getRight() == nullptr){
            //one left child
            Sat* child = rSat->getLeft();
            //copy data from child 
            assignmentSAT(rSat, child);
            //copy the left and right child also
            rSat->setLeft(child->getLeft());
            rSat->setRight(child->getRight());
            //deallocate node that was copied over
            delete child;

        }
        else if (rSat->getLeft() == nullptr && rSat->getRight() != nullptr){
            //one right child
            Sat* child = rSat->getRight();
            //copy data from child 
            assignmentSAT(rSat, child);
            //copy the left and right child also
            rSat->setLeft(child->getLeft());
            rSat->setRight(child->getRight());
            //deallocate node that was copied over
            delete child;
        }
        else{
            //two children
            //set temp to smallest node of right subtree
            Sat* tempSat = minRightSubTree(rSat->getRight());
            //copy data from temp
            assignmentSAT(rSat, tempSat);
            //get root using deleteNode and set the right subtree root to it and delte the copied node
            rSat->setRight(recursiveRemove(rSat->getRight(), tempSat->getID()));
        }
    }

    if (rSat == nullptr){
        //if last node was deleted
        return rSat;
    }

    //before checking balance factor update height
    rSat->setHeight(maxHeight(rSat->getLeft(), rSat->getRight()) + 1);

    balanceFactor = balanceF(rSat);

    if (balanceFactor < MIN_BALANCE && balanceF(rSat->getRight()) <= 0){
        //the tree is unbalanced from right subtree
        //the right subtree is larger or the same so Right Right case
        //do left rotation
        return leftRot(rSat);
    }
    if (balanceFactor > MAX_BALANCE && balanceF(rSat->getLeft()) >= 0){
        //the tree is unbalanced from Left subtree
        //the Left subtree is larger or the same so Left Left case
        //do right rotation
        return rightRot(rSat);
    }
    if (balanceFactor > MAX_BALANCE && balanceF(rSat->getLeft()) < 0){
        //the tree is unbalanced from left subtree
        //the right subtree is larger so Left Right case
        //do left Right rotation
        rSat->setLeft(leftRot(rSat->getLeft()));
        return rightRot(rSat);
    }
    if (balanceFactor < MIN_BALANCE && balanceF(rSat->getRight()) > 0){
        //the tree is unbalanced from right subtree
        //the left subtree is larger so Right Left case
        //do Right Left rotation
        rSat->setRight(rightRot(rSat->getRight()));
        return leftRot(rSat);
    }

    return rSat;
}   

Sat* SatNet::minRightSubTree(Sat* rSat){
    if (rSat == nullptr){
        return rSat;
    }
    else{
        minRightSubTree(rSat->getLeft());
    }

    return rSat;
}

void SatNet::assignmentSAT(Sat*& left, Sat*& right){
    left->setID(right->getID());
    left->setAlt(right->getAlt());
    left->setInclin(right->getInclin());
    left->setState(right->getState());
    left->setHeight(right->getHeight());
}

int SatNet::balanceF(Sat* rSat){
    int balanceF = 0;
    //checking balance cases 
    if ((rSat->getLeft() == nullptr) && (rSat->getRight() == nullptr)){
        // if both children doesn't exist
        return balanceF;
    }
    else if (rSat->getLeft() == nullptr){
        //if left child doesn't exist
        int leftH = 0;
        balanceF = leftH - rSat->getRight()->getHeight();
        return balanceF;
    }
    else if (rSat->getRight() == nullptr){
        //if rights child doesn't exist
        int rightH = 0;
        balanceF = rSat->getLeft()->getHeight() - rightH;
        return balanceF;
    }
    else{
        //two children (normal case)
        balanceF = (rSat->getLeft()->getHeight() - rSat->getRight()->getHeight());
        return balanceF;
    }
}

bool SatNet::recursiveFind(Sat* sat, int id) const{
    bool exists = false;
    if (sat != nullptr){
        if (sat->getID() == id){
            exists = true;
            return exists;
        }
        else if (id < sat->getID()){
            recursiveFind(sat->getLeft(), id);
        }
        else{
            recursiveFind(sat->getRight(), id);
        }
    }
    return exists;
}

bool SatNet::recursiveSetState(Sat* sat, int id, STATE state){
    bool exists = false;
    if (sat != nullptr){
        if (sat->getID() == id){
            exists = true;
            sat->setState(state);
            return exists;
        }
        else if (id < sat->getID()){
            recursiveFind(sat->getLeft(), id);
        }
        else{
            recursiveFind(sat->getRight(), id);
        }
    }
    return exists;
}

void SatNet::recursiveCountSat(Sat* sat, INCLIN degree, int& count) const{

    //prob wrong just run through it tmrw
    if (sat == nullptr){
        return;
    }

    if (sat->getInclin() == degree){
        count++;
    }
    
    recursiveCountSat(sat->getLeft(), degree, count);

    recursiveCountSat(sat->getRight(), degree, count);
}

void SatNet::recursiveFindState(Sat* sat, STATE state){
    //check this function look up how to actually recurse avl tree
    if (sat != nullptr){
        recursiveFindState(sat->getLeft(), state);

        if (sat->getState() == state){
            recursiveRemove(m_root, sat->getID());
            return;
        }
        
        recursiveFindState(sat->getRight(), state);
    }
}

void SatNet::recursiveClear(Sat* sat){
    if (sat != nullptr){
        if (sat->getLeft() != nullptr){
            recursiveClear(sat->getLeft());
        }
        if (sat->getRight() != nullptr){
            recursiveClear(sat->getRight());
        }
        delete sat;
        sat = nullptr;
    }
}

void SatNet::recursiveOverloaded(Sat* RHSsat, Sat*& m_root){
    if (RHSsat == nullptr){
        return;
    }
    m_root = recursiveInsert(m_root, *RHSsat);

    recursiveOverloaded(RHSsat->getLeft(), m_root);
    recursiveOverloaded(RHSsat->getRight(), m_root);
}

void SatNet::recursiveList(Sat* sat) const{
    if (sat == nullptr){
        return;
    }

    recursiveList(sat->getLeft());

    cout << sat->getID() << ": " << sat->getStateStr() << ": " << sat->getInclinStr() << ": " << sat->getAltStr() << endl;

    recursiveList(sat->getRight());
}