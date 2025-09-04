/*
References:
    - https://www.w3schools.com/cpp/cpp_classes.asp
    - https://www.geeksforgeeks.org/how-to-dynamically-allocate-an-array-in-cpp/
*/

#ifndef RESIZING_ARRAY_H
#define RESIZING_ARRAY_H

template <typename T>
class ResizingArray {
    private:
        int numValues; //number of values (valid and invalid)
        int capacity; //maximum capacity
        int numValidValues; //number of non -1 values
        T *arr = new T[2]; //array
    
    public:
        //constructor
        ResizingArray(){
            numValues = 0;
            capacity = 2;
            numValidValues = 0;
        }

        ~ResizingArray(){
            delete[] arr;
            arr = nullptr;
        }

        //prints every value in the array to the console
        void print(){
            //print the first value
            std::cout << arr[0];

            int index = 1;
            while (index < numValues){
                std::cout << ", " << arr[index];
                index++;
            }
            return;
        }

        //append value to end of array
        void appendVal(T value){
            //increment numValidValues if appending a valid value
            if (value >= 0){
                numValidValues++;
            }

            //if special case of empty list, set the first index as value and return
            if (numValues == 0){
                numValues++;
                arr[0] = value;
                return;
            }
            //increment num values to account for the new value
            numValues++;

            //checks if numValues = capacity (full), and doubles capacity if full
            if (this -> isFull()){
                this -> doubleCapacity();
            }
            //numValues should always be 1 greater than the index assuming no blanks spaces in the array
            arr[numValues - 1] = value;

            return;
        }

        //insert value to a given index and returns true if successful
        bool insertVal(int index, T value){
            //if index out of bounds, return false for failed delete
            if (index < 0 || index > numValues){
                return false;
            }

            //increment num valid values
            if (value >= 0){
                numValidValues++;
            }

            //increment num values to account for the new value
            numValues++;

            //checks if numValues = capacity (full), and doubles capacity if full
            if (this -> isFull()){
                this -> doubleCapacity();
            } 

            //create a new array to copy the old values to
            T *newArr = new T[capacity];

            //create a counter for the index of the new array
            int newIndex = 0;

            //numValues was incremented to account for the new, yet uninserted value
            //however, old array doesn't contain the uninserted value, so the maxIndex is 1 less than the numValues
            int maxIndex = numValues - 1;

            //copy the entire array over while inserting the new value
            for (int i = 0; i < maxIndex; i++){
                //if you arrive at the insertion index, insert the new value and increment the new array index
                if (i == index){
                    newArr[newIndex] = value;
                    newIndex++;
                } 
                
                newArr[newIndex] = arr[i];

                newIndex++;
            }

            //temporarily store the pointer to the old array for deletion later
            T *temp = arr;

            //store the new array pointer into the class's private variable
            arr = newArr;

            //deallocate old array
            delete[] temp;
            temp = nullptr;
            
            //return true for a successful delete
            return true;
        }
        
        //delete a value from a given index and returns true if successful
        //unfinished
        bool deleteVal(int index){
            //if index out of bounds, return false for failed delete
            if (index < 0 || index > numValues){
                return false;
            }

            //decrement if the delete value was valid
            if (arr[index] >= 0){
                numValidValues--;
            }

            //create a new array to copy the non-deleted values to
            T *newArr = new T[capacity];

            //create a counter for the index of the new array
            int newIndex = 0;
            //copy the entire array over except for the deleted value
            for (int i = 0; i < numValues; i++){
                //if you arrive at the deleted value, keep newIndex in place
                if (i == index){
                    newIndex--;
                } else {
                    newArr[newIndex] = arr[i];
                }

                newIndex++;
            }

            //temporarily store the pointer to the old array for deletion later
            T *temp = arr;

            //store the new array pointer into the class's private variable
            arr = newArr;

            //deallocate old array
            delete[] temp;
            temp = nullptr;

            //decrement number of values
            numValues--;

            //if the number of values in the array is 1/4 the capacity, call halveCapacity
            if (this -> isQuartered()){
                this -> halveCapacity();
            }
            
            //return true for a successful delete
            return true;
        }

        void emptyArray(){
            //reset numValues and capacity to the lowest
            numValues = 0;
            capacity = 2;
            numValidValues = 0;

            //saved old array pointer for deletion later and create a new array of the minimum size
            T* temp = arr;
            arr = new T[2];

            //deallocate old and array and set the temporary pointer to null pointer
            delete[] temp;
            temp = nullptr;
        }

        //doubles the capacity of the ResizingArray and copies all old array values into the new array
        void doubleCapacity(){
            //create a new array of double capacity
            capacity = 2*capacity;
            T *newArr = new T[capacity];

            //doubleCapacity is only used for insertions, ad they always increment numValues before calling doubleCapacity
            //thus we need to decrement numValues by 1 to make sure we don't go past the length of the array
            int maxIndex = numValues - 1;

            //copy all values from the old array into the new array
            for (int i = 0; i < maxIndex; i++){
                newArr[i] = arr[i];
            }

            //temporarily store the pointer to the old array for deletion later
            T *temp = arr;

            //store the new array pointer into the class's private variable
            arr = newArr;

            //deallocate old array
            delete[] temp;
            temp = nullptr;

            return;
        }

        //halves capacity if half capacity is still greater than 2 and copies all old array values in the new array
        void halveCapacity(){
            //if half capacity is less than 2, return immediately
            if ( capacity/2 < 2 ){
                return;
            }
            
            //otherwise, create a new array with half the capacity
            capacity = capacity/2;
            T *newArr = new T[capacity];

            //copy all values from the old array into the new array
            for (int i = 0; i < numValues; i++){
                newArr[i] = arr[i];
            }

            //temporarily store the pointer to the old array for deletion later
            T *temp = arr;

            //store the new array pointer into the class's private variable
            arr = newArr;

            //deallocate old array
            delete[] temp;
            temp = nullptr;

            return;
        }

        //boolean methods
        //--------------------------------------------------------------------------------------
        //return true if capacity is reached, false otherwise
        bool isFull() {
            return numValues == capacity;
        }
        
        //return true if less than a quarter of capacity is filled, false otherwise
        bool isQuartered() {
            return numValues <= capacity/4;
        }

        //return true if there are no values
        bool isEmpty() {
            return numValues == 0;
        }

        bool isEmptyValid() {
            return numValidValues == 0;
        }

        //getters
        int getNumValues(){
            return numValues;
        }

        int getCapacity(){
            return capacity;
        }

        int getNumValidValues(){
            return numValidValues;
        }

        T* getArray(){
            return arr;
        }

        //incrementer
        void incrementNumValidValues(){
            numValidValues++;
        }
        //allows ResizingArray to return values at specific array indexes like basic arrays
        T operator[] (int index){
            return arr[index];
        }
};

#endif
