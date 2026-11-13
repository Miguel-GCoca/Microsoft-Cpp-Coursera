#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

int main(){

    ofstream reportFile("sales_report.txt");
    if(!reportFile){
        cerr << "error: could not create report" << endl;
        return 1;
    }
    reportFile << "DAILY SALES REPORT" << std::endl;
    reportFile << "==================" << std::endl;
    reportFile << std::left << std::setw(15) << "Product"
        << std::setw(10) << "Quantity"
        << std::setw(10) << "Price" << std::endl;

    reportFile << std::left << std::setw(15) << "Laptop"
        << std::setw(10) << "5"
        << "$" << std::fixed << std::setprecision(2) << 999.99 << std::endl;
    reportFile << std::left << std::setw(15) << "Mouse"
        << std::setw(10) << "12"
        << "$" << std::fixed << std::setprecision(2) << 29.99 << std::endl;
    reportFile.close();
    cout << "Sales report generated" << endl;
    
    return 0;
}