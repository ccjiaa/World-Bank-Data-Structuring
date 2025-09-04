#include "bTree.h"
#include <iostream>
#include <cassert>

BTree::BTree(std::string arrS[512], double arrD[512], std::string sc, int nv){
    for (int i = 0; i < 512; i++){
        strArr[i] = "";
        dblArr[i] = -1.0;
    }
    
    for (int i = 0; i < nv; i++){
        strArr[i] = arrS[i];
        dblArr[i] = arrD[i];
    }
    seriesCode = sc;
    numVal = nv;
    root = nullptr;
    assert(numVal > 0);
    root = new BTreeNode(strArr, dblArr, numVal);
    buildTree(root);
    numNode = 1;
}

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

void BTree::buildTree(BTreeNode* parent){
    // if (numNode > 500){
    //     return;
    // }
    if (parent == nullptr){
        return;
    }
    if (parent -> getNumVal() < 2){
        return;
    }
    if (parent -> isAllEqual()){
        return;
    }

    double parentMean = parent -> getMidMean();

    std::string* parentStrArray = parent -> getStrArr();
    double* parentDblArray = parent -> getDblArr();

    std::string rightStrArray[512]{""};
    double rightDblArray[512]{-1.0};
    std::string leftStrArray[512]{""};
    double leftDblArray[512]{-1.0};

    int leftIdx = 0;
    int rightIdx = 0;
    int max = parent -> getNumVal();
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

    if (leftIdx >= parent->getNumVal() || rightIdx >= parent->getNumVal()) {
        return;
    }

    if (leftIdx > 0){
        numNode++;
        BTreeNode* leftChild = new BTreeNode(leftStrArray, leftDblArray, leftIdx);
        parent -> setLeft(leftChild);
        buildTree(leftChild);
    }
    if (rightIdx > 0){
        numNode++;
        BTreeNode* rightChild = new BTreeNode(rightStrArray, rightDblArray, rightIdx);
        parent -> setRight(rightChild);
        buildTree(rightChild);
    }

    return;
}

void BTree::findOperation(double tarMean, std::string operation){
    if (root == nullptr){
        std::cout << std::endl;
        return;
    }
    BTreeNode* curNode = root;
    BTreeNode* prevNode = root;
    if (operation == "less"){
        while (curNode != nullptr){
            prevNode = curNode;
            if (curNode -> getMidMean() >= tarMean){
                curNode = curNode -> getLeft();
            } else {
                break;
            }
        }
        prevNode -> printTargetMean(tarMean, operation);
    } else if (operation == "greater"){
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
        while (curNode != nullptr){
            prevNode = curNode;
            if (std::abs(curNode -> getMidMean() - tarMean) <= 0.001){
                break;
            } else if (curNode -> getMidMean() > tarMean){
                curNode = curNode -> getLeft();
            } else {
                curNode = curNode -> getRight();
            }
        }
        prevNode -> printTargetMean(tarMean, operation);
    }
    return;
}

void BTree::deleteCountryFromTree(std::string countryName){
    if (root == nullptr){
        std::cout << "failure" << std::endl;
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

    if (foundCountry){
        std::cout << "success" << std::endl;
    } else {
        std::cout << "failure" << std::endl;
    }

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