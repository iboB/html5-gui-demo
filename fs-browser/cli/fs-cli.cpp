#include <iostream>
#include "fsbrowser/FSBrowser.hpp"

int main()
{
    fsbrowser::FSBrowser fsBrowser;

    std::string curRequest = "contents:";
    while(true)
    {
        std::string response = fsBrowser.onRequest(curRequest);
        std::cout << response << std::endl;
        std::getline(std::cin, curRequest);
        if (curRequest == "quit") break;
    }

    return 0;
}