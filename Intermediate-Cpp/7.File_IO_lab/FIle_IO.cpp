#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main(){

    ifstream inputFile("products.txt");
    if (!inputFile.is_open()){
        cerr << "error: could not open products.txt" << endl;
        return 1;
    }
    string productName;
    int count = 0;
    cout << "Product Inventory: " << endl;
    while(getline(inputFile, productName)){
        count++;
        cout << count << ". " << productName << endl;
    }
    inputFile.close();
    cout << "Total Products: " << count << endl;
    
    return 0;
}