/*
 * © 2023 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <cstring>
#include <exception>
#include <iostream>
#include <memory>

#include <kos_net.h>

#include "general.h"
#include "publisher.h"

namespace {
constexpr auto DefaultPort = 5672;
} // namespace

int main()
{
    if (!wait_for_network())
    {
        std::cerr << app::AppTag << "Wait for network failed!" << std::endl;
        return EXIT_FAILURE;
    }

    char *brokerAddress = getenv("AMQP_BROKER_ADDRESS");
    if (!brokerAddress)
    {
        std::cerr << app::AppTag << "Unable to get broker address from env AMQP_BROKER_ADDRESS."
                  << std::endl;
        return EXIT_FAILURE;
    }

    const int brokerPort = []{
        try
        {
            return std::stoi(getenv("AMQP_BROKER_PORT"));
        }
        catch (const std::invalid_argument &ex)
        {
            std::cerr << app::AppTag << "Failed to get AMQP broker port. Using default "
                      << DefaultPort << " port." << std::endl;
            return DefaultPort;
        }
    }();

    std::cout << app::AppTag << "AMQP BROKER " << brokerAddress << ":" << brokerPort << std::endl;

    try
    {
        auto pub = std::make_unique<Publisher>(brokerAddress, brokerPort);
        std::cout << app::AppTag << "Sending data ..." << std::endl;
        pub->SendData();
    }
    catch (const std::exception& e)
    {
        std::cerr << app::AppTag << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
