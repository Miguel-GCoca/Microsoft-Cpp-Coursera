#include <iostream>
#include <string>
int main() {
    std::string message = "Welcome to the world of C++ programming!";
    std::cout << "Original message: " << message << std::endl;
    // Extract the first word using substr()
    size_t first_space = message.find(' ');
    if (first_space != std::string::npos) {
        std::string first_word = message.substr(0, first_space);
        std::cout << "First word: " << first_word << std::endl;
    }
    // Your code here: Find and extract the last word using rfind() and substr()   
    
    size_t last_space = message.rfind(' ');
    if (last_space != std::string::npos){
        std::string last_word = message.substr(last_space, message.length());
        std::cout << "Last word is: " << last_word << std::endl;
    }
    // Your code here: Count how many times the letter 'o' appears in the message  
    int counter = 0;
    for(char c : message){
        if (c == 'o'){
            counter++;
        }
    }
    std::cout << "Number of 'o's: " << counter << std::endl;
    return 0;
}