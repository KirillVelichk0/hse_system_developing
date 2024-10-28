#include "parentsession.h"
#include <iostream>
#include <string>
#include <algorithm>

int main(int argc, char* argv[]) {
    int returnCode = 0;
    try{
    if (argc == 2) {
        std::string inputText;
        inputText += argv[1];
        auto isCorrectSymbol = [](char symb){
          symb = std::tolower(symb);
          return (symb >= 'a' && symb <= 'z') || (symb >= '0' && symb <= '9');
        };
        auto it = std::find_if_not(inputText.begin(), inputText.end(), isCorrectSymbol);
        if(it != inputText.end()){
            std::cout << "Uncorrect text passed\n";
            returnCode = 1;
        } else {
            ParentSession session(inputText);
            auto answer = session.FindAnswer();
            std::cout << answer << std::endl;
        }
    } else {
        std::cout << "Not correct params passed." << std::endl;
        returnCode = 1;
    }
    } catch(ChildSessionExit& ){

    } catch(std::exception& ex){
        returnCode = 1;
        std::cout << "Cant process input. Error: " << ex.what() << std::endl;
    } catch(...){
        returnCode = 1;
        std::cout << "Cant process input. Get unknown error" << std::endl;
    }

    return returnCode;
}
