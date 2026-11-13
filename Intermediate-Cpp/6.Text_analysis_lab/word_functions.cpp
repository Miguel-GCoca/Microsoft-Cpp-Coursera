#include <string>
#include <cctype>
int word_counter(std::string text){
    int counter = 0;
    if(text.length() == 0){
        return -1;
    }else{
        while(true){

            size_t first_space = text.find_first_of(" ,.!?;:");
            std::string sub_string;

            if (first_space != std::string::npos){
                sub_string = text.substr(0, first_space);
                text.erase(0, first_space + 1);
            }
            else{
                break;
            }

            if (sub_string.length() != 0){
                if (sub_string.length() == 1  && 
                    (sub_string.at(0) == 'a' || sub_string.at(0) == 'A' || sub_string.at(0) == 'I')){
                    counter++;
                }
                else{
                    counter++;
                }
            }
        }
        if (text.length() > 0){
            counter++;
        }
            
        
    }
    return counter;
}

//returns number of instances of a specified character
int how_many_instances(std::string text, char c){
    int counter = 0;
    if(text.length() == 0){
        return -1;
    }

    for(char character : text){
        if (character == c){
            counter++;
        }
    }
    return counter;
}

bool palindrome_detector(std::string text){
    std::string lower_text,reverse;

    for (char c : text){
        char l = static_cast<char>(std::tolower(c));
        lower_text.insert(lower_text.end(), l);
    }
    for (char c : lower_text){
        reverse.insert(reverse.begin(), c);
    }

    if (lower_text == reverse){
        return true;
    }
    else{
        return false;
    }
}