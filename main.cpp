#include <iostream>
#include <fstream>
#include <unistd.h>
#include <string>
#include "pkg.cpp"
#include "mkfile.cpp"
//main

int main(int argc, char *argv[]) {
    if (getuid() != 0){ std::cout<< vars.prefix << "Please run as root"; exit(1);}
    for(;;)
    {
        switch(getopt(argc, argv, "iu:s:he")) // note the colon (:) to indicate that 'n' has a parameter and is not a switch
        {
        case 'i':
            std::cout << vars.prefix <<"Installing from APPCONF"<<std::endl;
            install();
            continue;

        case 'u':
            uninstall(optarg);
            continue;
        case 's':
            continue;
        case 'e':
            gen_example_appconf();
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