#include "gb.h" // GB

#include <boost/program_options.hpp> // boost::program_options
#include <iostream>

int main(int argc, char *argv[])
{
    namespace po = boost::program_options;
    // Parse the command line arguments
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce this help message")
        ("rom,r", po::value<std::string>(), "path to the ROM file")
        ("scale,s", po::value<int>()->default_value(1), "scale of the window (default: 1)");
    po::positional_options_description p;
    p.add("rom", 1);
    p.add("scale", 2);
    po::variables_map vm;
    po::store(
            po::command_line_parser(argc, argv)
                    .options(desc)
                    .positional(p)
                    .run(),
            vm);
    po::notify(vm);
    if (vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }
    if (!vm.count("rom"))
    {
        std::cout << "No ROM file specified" << std::endl;
        return 1;
    }

    // Run the emulator
    gameboy::GB gameboy(vm["scale"].as<int>());
    if (gameboy.run(vm["rom"].as<std::string>()) == 1)
        return 1; // An error occurred
    return 0;
}
