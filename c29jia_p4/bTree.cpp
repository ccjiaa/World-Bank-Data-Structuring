#include "bTree.h"
#include <iostream>
#include <cassert>

//Constructor, copies parameter arrays, initializes member variables, and calls node constructor to build the root and the rest of the tree
BTree::BTree(std::string arrS[512], double arrD[512], std::string sc, int nv){
    //Copy arrays
    for (int i = 0; i < 512; i++){
        strArr[i] = "";
        dblArr[i] = -1.0;
    }
    
    for (int i = 0; i < nv; i++){
        strArr[i] = arrS[i];
        dblArr[i] = arrD[i];
    }
    //initialize member variables
    seriesCode = sc;
    numVal = nv;
    root = nullptr;
    //check if arrays were empty
    assert(numVal > 0);
    //build root
    root = new BTreeNode(strArr, dblArr, numVal);
    numNode = 1;
    //use the root to build the rest of the tree
    buildTree(root);
}

//destructor, deletes all nodes
BTree::~BTree(){
    destroyRecursive(root);
    root = nullptr;
}
/*
Deletes each node in the tree using post-order traversal, code from https://stackoverflow.com/questions/34170164/destructor-for-binary-search-tree
*/
void BTree::destroyRecursive(BTreeNode* node){
    if (node == nullptr){
        return;
    }
    destroyRecursive(node->getLeft());
    destroyRecursive(node->getRight());
    delete node;
    node = nullptr;
}

/*
Builds all nodes in the tree and links them appropriately
*/
void BTree::buildTree(BTreeNode* parent){
    //if parent is nullptr, only contains one value, or contains countries whose means are all equal, do not build a node
    if (parent == nullptr){
        return;
    }
    if (parent -> getNumVal() < 2){
        return;
    }
    if (parent -> isAllEqual()){
        return;
    }

    //otherwise build a new node
    //get parent's midMean (i.e. parent value) and its two arrays for mean and countryname
    double parentMean = parent -> getMidMean();
    std::string* parentStrArray = parent -> getStrArr();
    double* parentDblArray = parent -> getDblArr();

    //create 4 arrays, one pair for potential right child, one for potential left child
    std::string rightStrArray[512]{""};
    double rightDblArray[512]{-1.0};
    std::string leftStrArray[512]{""};
    double leftDblArray[512]{-1.0};

    //create indexes to add values to left and right array pairs
    int leftIdx = 0;
    int rightIdx = 0;
    //max = max index = length of parent node's array
    int max = parent -> getNumVal();
    //go through the parent array, if the mean of the country is less than the mid mean of the parent, put in left array
    //other put in right array
    for (int i = 0; i < max; i++){
        if (parentDblArray[i] > parentMean || std::abs(parentDblArray[i] - parentMean) <= 0.001){
            rightStrArray[rightIdx] = parentStrArray[i];
            rightDblArray[rightIdx] = parentDblArray[i];
            rightIdx++;
        }  else {
            leftStrArray[leftIdx] = parentStrArray[i];
            leftDblArray[leftIdx] = parentDblArray[i];
            leftIdx++;
        }
    }

    //check if all the countries are inside a single child's array. If so, all means must be approximately the same, don't create a node and return
    //this check helps in case there is a specific array of country means that pass the same mean guard at the start of the function
    if (leftIdx >= parent->getNumVal() || rightIdx >= parent->getNumVal()) {
        return;
    }

    //if there are children in the left child array, build the node and link it to the parent, call build on the newly built child node
    if (leftIdx > 0){
        numNode++;
        BTreeNode* leftChild = new BTreeNode(leftStrArray, leftDblArray, leftIdx);
        parent -> setLeft(leftChild);
        buildTree(leftChild);
    }
    //if there are children in the right child array, build the node and link it to the parent, call build on the newly built child node
    if (rightIdx > 0){
        numNode++;
        BTreeNode* rightChild = new BTreeNode(rightStrArray, rightDblArray, rightIdx);
        parent -> setRight(rightChild);
        buildTree(rightChild);
    }

    return;
}

/*
Finds countries with means that satisfy the given operation (less, greater, equal)
*/
void BTree::findOperation(double tarMean, std::string operation){
    //if the tree is empty, stop
    if (root == nullptr){
        std::cout << std::endl;
        return;
    }

    //otherwise, traverse through the tree
    BTreeNode* curNode = root;
    BTreeNode* prevNode = root;
    //for operation less, check all countries in the parent node of the node whose mid mean is higher than target mean
    //parent node because only the parent mean guarantees that it will find all nodes the satisfy
    //child node may only have a subset, this also applies to operation greater
    if (operation == "less"){
        while (curNode != nullptr){
            //update prevNode
            prevNode = curNode;
            //keep getting left children (small midMeans) until midMean becomes less than the target mean
            if (curNode -> getMidMean() >= tarMean){
                curNode = curNode -> getLeft();
            } else {
                break;
            }
        }
        //find and print all nodes with means that satify the operation
        prevNode -> printTargetMean(tarMean, operation);
    } else if (operation == "greater"){
        //same logic as operation less, but getting right children, and stopping when the midMean is less than the target mean
        while (curNode != nullptr){
            prevNode = curNode;
            if (curNode -> getMidMean() <= tarMean){
                curNode = curNode -> getRight();
            } else {
                break;
            }
        }
        prevNode -> printTargetMean(tarMean, operation);
    } else if (operation == "equal"){
        //if operation equal
        //every country with a mean less than midmean must be to the left, everything with a mean more must be to the right
        //thus you keep going until target equals the midmean, or you reach a leaf node
        while (curNode != nullptr){
            prevNode = curNode;
            //if midmean is equal to the target mean, break
            if (std::abs(curNode -> getMidMean() - tarMean) <= 0.001){
                break;
            } else if (curNode -> getMidMean() > tarMean){
                curNode = curNode -> getLeft();
            } else {
                curNode = curNode -> getRight();
            }
        }
        //find and print all nodes with means that satify the operation
        prevNode -> printTargetMean(tarMean, operation);
    }
    return;
}

void BTree::deleteCountryFromTree(std::string countryName){
    if (root == nullptr){
        // std::cout << "failure" << std::endl;
        return;
    }
    std::string tempStr = "";
    double tempDbl;
    double countryMean = 0.0;
    bool foundCountry = false;
    assert(numVal < 512);
    for (int i = 0; i < numVal; i++){
        if (strArr[i] == countryName){
            foundCountry = true;
            tempStr = strArr[i];
            strArr[i] = strArr[numVal - 1];
            strArr[numVal - 1] = tempStr;

            countryMean = dblArr[i];

            tempDbl = dblArr[i];
            dblArr[i] = dblArr[numVal - 1];
            dblArr[numVal - 1] = tempDbl;
            
            numVal--;
            break;
        }
    }

    //if the country to be deleted is found, delete that country from all nodes in the tree
    if (foundCountry) {
        BTreeNode* curNode = root;
        BTreeNode* prevNode = root;
        bool left = false;
        while (curNode != nullptr) {
            curNode->deleteCountry(countryName);
            if (curNode->getNumVal() < 1) {
                if (left) {
                    prevNode->setLeft(nullptr);
                } else {
                    prevNode->setRight(nullptr);
                }
                delete curNode;
                curNode = nullptr;
                break;
            }
            prevNode = curNode;
            if (countryMean >= curNode->getMidMean()) { 
                curNode = curNode->getRight();
                left = false;
            } else {
                curNode = curNode->getLeft();
                left = true;
            }
        }
    }

    // if (foundCountry){
    //     std::cout << "success" << std::endl;
    // } else {
    //     std::cout << "failure" << std::endl;
    // }

    return;
}

void BTree::findLims(std::string condition){
    if (root == nullptr){
        std::cout << std::endl;
        return;
    }

    BTreeNode* curNode = root;
    if (condition == "lowest"){
        //go to leftmost node
        while (curNode -> getLeft() != nullptr){
            curNode = curNode -> getLeft();
        }
    } else {
        //assume that condition will always either be highest or lowest
        while (curNode -> getRight() != nullptr){
            curNode = curNode -> getRight();
        }
    }
    curNode -> printStrArr();
}


BTreeNode* BTree::getRoot(){
    return root;
}

void BTree::printTreeStart(){
    if (root == nullptr){
        return;
    }
    printTree(root);
    return;
}
void BTree::printTree(BTreeNode* node){
    if (node == nullptr){
        // std::cout << "triggered nullptr :D" << std::endl;
        return;
    }

    // First recur on left subtree
    printTree(node->getLeft());

    // Now deal with the node
    node->printNode();

    // Then recur on right subtree
    printTree(node->getRight());
}