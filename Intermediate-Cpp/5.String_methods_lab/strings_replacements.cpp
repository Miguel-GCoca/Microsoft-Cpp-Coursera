#include <iostream>
#include <string>
int main() {
    std::string text = "Hello world! The world is beautiful.";
    std::cout << "Original: " << text << std::endl; 
    // Replace "world" with "universe"
    size_t pos = text.find("world");
    if (pos != std::string::npos) {
        text.replace(pos, 5, "universe");
        std::cout << "After first replacement: " << text << std::endl;
    }   
    // Your code here: Replace the second occurrence of "world" with "universe"
    // Hint: Use find() with a starting position parameter    
    size_t pos2 = text.find("world");
    if(pos2 != std::string::npos){
        text.replace(pos2, 5, "universe");
        std::cout << "After second replacement: " << text << std::endl;
    }


    // Your code here: Remove all exclamation marks from the text using erase()   
    
    while(true){

        size_t pos3 = text.find("! ");
        if (pos3 != std::string::npos){
            text.replace(pos3, 2, " ");
        }else{
            break;
        }
    }

    // Convert text to uppercase (for each character, use std::toupper)
    for (char& c : text) {
        c = std::toupper(c);
    }
    std::cout << "Uppercase: " << text << std::endl;    
    return 0;
}