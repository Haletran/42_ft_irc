#include "../includes/Common.hpp"
#include <stdexcept>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc <= 2)
    {
        std::cerr << "Not enough or too much paramaters" << std::endl;
        return (1);
    }

    try {
        std::string password = argv[2];
        int port = static_cast<int>(std::strtod(argv[1], NULL));
        Server instance(port ,password);
        std::cout << "Waiting for connections..." << std::endl;
        while (1)
        {
        }
    } catch (std::runtime_error &e) {
        std::cerr << "Error : " << e.what() << std::endl;
    }
    return (0);
}
