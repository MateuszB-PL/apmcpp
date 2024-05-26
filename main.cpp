#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include "pkg.cpp"


//main

int main(int argc, char *argv[]) {
    if (getuid() != 0){ std::cout<< "Please run as root"; exit(1);}
    for(;;)
    {
        switch(getopt(argc, argv, "iu:s:h")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
        case 'i':
            std::cout << v.prefix <<"Installing from APPCONF"<<std::endl;
            install();
            continue;

        case 'u':
            uninstall(optarg);
            continue;
        case 's':
            continue;
        case '?':
        case 'h':
        default :
          printf("Help N/A\n");
            break;

        case -1:
            break;
        }

        break;
    }
    
}