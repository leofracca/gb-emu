#include "gb.h" // GB

#include <boost/program_options.hpp> // boost::program_options
#include <iostream> // std::cout, std::endl
#include <optional> // std::optional

std::optional<boost::program_options::variables_map> handleArguments(int argc, char *argv[])
{
    namespace po = boost::program_options;
    // Parse the command line arguments
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce this help message")
        ("rom,r", po::value<std::string>(), "path to the ROM file")
        ("scale,s", po::value<int>()->default_value(1), "initial scale of the window (default: 1)")
        ("maximize,m", "maximize the window on startup");
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
        return {};
    }
    if (!vm.count("rom"))
    {
        std::cout << "No ROM file specified" << std::endl;
        return {};
    }
    if (vm["scale"].as<int>() < 1)
    {
        std::cout << "Scale must be greater than 0" << std::endl;
        return {};
    }

    return vm;
}

int main(int argc, char *argv[])
{
    auto vm = handleArguments(argc, argv);
    if (!vm)
        return 0;

    auto scale = vm.value()["scale"].as<int>();
    auto rom = vm.value()["rom"].as<std::string>();
    bool maximize = vm->count("maximize") > 0;

    // Run the emulator
    gameboy::GB gameboy(scale, maximize);
    if (gameboy.run(rom) == 1)
        return 1; // An error occurred
    return 0;
}
