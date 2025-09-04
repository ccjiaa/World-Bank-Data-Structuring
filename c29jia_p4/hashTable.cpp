#include "hashTable.h"

//Constructor, sets numVal and arraySize to their default values
HashTable::HashTable(){
    numVal = 0;
    arraySize = 512;
}

//Destructor, deletes each individual singleCountryTimeSeries from the hashtable
HashTable::~HashTable(){
    int count = 0;
    for (int i = 0; i < arraySize; i++) {
        if (hashArray[i] != nullptr) {
            delete hashArray[i];
            hashArray[i] = nullptr;
            count++;
        }
    }
}

/*
Calculates a new hashed index, depending on the number of times you want it to recalculate the hash
*/
int HashTable::calcHash(std::string countryCode, int hashTimes){
    //split the three letters, find their integer ASCII values, subtract 65 to scale A to 0 and Z to 25
    int firstLetter = (int)countryCode.at(0) - 65;
    int secondLetter = (int)countryCode.at(1) - 65;
    int thirdLetter = (int)countryCode.at(2) - 65;

    //multiple first letter by 26^2, second by 26^1, third by 26^0
    firstLetter = firstLetter*26*26;
    secondLetter = secondLetter*26;

    int w = firstLetter + secondLetter + thirdLetter;
    
    //call firstHash and secondHash while using hashTimes
    int hash = (firstHash(w) + hashTimes*secondHash(w))%arraySize;

    return hash;
}
//calculates the value of the initial hash H1(w)
int HashTable::firstHash(int sumW){
    //sum of letters mod 512
    return sumW%arraySize;
}
//calcules the value of the backup secondary hash H2(w)
int HashTable::secondHash(int sumW){
    //sum of letters divided by 512, mod 512
    int num = (sumW/arraySize)%arraySize;
    //if odd return even number, if even return odd number
    if (num%2 == 1){
        return num;
    } else{
        return num + 1;
    }
    //failure, will break the program with a seg fault
    return -1;
}

/*
Finds the index of a country with the given countryCode and prints it along with how many hashing attempts it took. 
Prints failure if the country isn't found. 
*/
void HashTable::lookUp(std::string countryCode){
    bool isFound = false;
    int curHash = -1;
    int numHash = 0;
    while (!isFound){
        //calculate the hash index
        curHash = calcHash(countryCode, numHash);

        //if the value at that index is nullptr
        if (hashArray[curHash] == nullptr){
            //check if there was previously a value there that was deleted, if no, then you reached an empty spot, break and return false
            if(!isPrevFilledArray[curHash]){
                isFound = true;
                break;
            }
        } else if (hashArray[curHash] -> getCountryCode() == countryCode){
            //found, print out the index and how many hashes it took (+1 because we always do a default hash)
            std::cout << "index " << curHash << " searches " << numHash + 1 << std::endl;
            return;
        } else {
            //do nothing
        }
        //increase the number of hashes
        numHash++;
    }
    //if broken out of loop, not found, print failure and return
    std::cout << "failure" << std::endl;
    return;
}

/*
Finds the country with the given countryCode in the hashtable. If the country is found, returns the index where it was found, otherwise returns -1
*/
int HashTable::findCountry(std::string countryCode){
    int foundIndex = -1;
    int curHash = -1;
    int numHash = 0;
    //while the index of the target country hasn't been found
    while (foundIndex < 0){
        //calculate the hash index
        curHash = calcHash(countryCode, numHash);

        //if the value at that index is nullptr
        if (hashArray[curHash] == nullptr){
            //check if there was previously a value there that was deleted, if no, then you reached an empty spot, break out
            if(!isPrevFilledArray[curHash]){
                break;
            }
        } else if (hashArray[curHash] -> getCountryCode() == countryCode){
            //found, set foundIndex to the current hashed index and break out
            foundIndex = curHash;
            break;
        } else {
            //do nothing
        }
        //increment the number of hashes
        numHash++;
    }

    //return the index if found, otherwise return -1
    return foundIndex;
}

/*
Inserts a given SingleCountryTimeSeries pointer into the hashtable, if it's not already in the hash table return true, false otherwise
*/
bool HashTable::insertToHash(SingleCountryTimeSeries* scts, std::string countryCode){
    //check if the country is present in the hashtable, if yes return false
    if (findCountry(countryCode) >= 0){
        return false;
    }
    //if not, calculate hash indexes until you find an empty one, put the pointer in that index
    int curHash = -1;
    for (int i = 0; i < 512; i++){
        curHash = calcHash(countryCode, i);
        if (hashArray[curHash] == nullptr){
            hashArray[curHash] = scts;
            break;
        }
    }
    numVal++;
    return true;
}

/*
Deletes a singleCountryTimeSeries from the hashtable. Outputs true if successful, outputs false if not
Useful if the index of the target country is unknown, however, if the index is known, deleteIndex is used instead as it's more efficient
*/
bool HashTable::deleteFromHash(std::string countryCode){
    int curHash = -1;
    //calculate hashes with an increasing number of hash steps
    //if after 512 steps neither the target nor an empty slot is found, something is wrong, so end the loop
    for (int i = 0; i < 512; i++){
        //calculate a hash index
        curHash = calcHash(countryCode, i);
        //if the value is a nullptr
        if (hashArray[curHash] == nullptr){
            //check if there was previously a value there that was deleted, if no, then you reached an empty spot, return false
            if(!isPrevFilledArray[curHash]){
                return false;
            }
        } else if(hashArray[curHash] -> getCountryCode() == countryCode){
            //else if you found the country, delete it and return true
            delete hashArray[curHash];
            hashArray[curHash] = nullptr;
            //update the isPrevFilledArray to reflect that this hash index was previously filled
            isPrevFilledArray[curHash] = true;
            return true;
        } else {
            //do nothing
        }
    }
    //default return false if all goes wrong
    return false;
}

/*
Deletes a object at the given index, and marks it as previously occupied
*/
bool HashTable::deleteIndex(int index){
    //if index out of range, return false
    if (index < 0 || index > arraySize){
        return false;
    }
    //otherwise, decrement numbVal then delete the singleCountryTimeSeries object at the given index 
    numVal--;
    delete hashArray[index];
    hashArray[index] = nullptr;
    isPrevFilledArray[index] = true;
    return true;
}

int HashTable::getNumVal(){
    return numVal;
}

int HashTable::getArraySize(){
    return arraySize;
}

//allows ResizingArray to return values at specific array indexes like basic arrays
SingleCountryTimeSeries* HashTable::operator[] (int index){
    return hashArray[index];
}