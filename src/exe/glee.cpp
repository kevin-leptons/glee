#include <glee/api.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <string>

namespace po = boost::program_options;

int main(int argc, char *argv[])
{
        std::vector<std::string> inc_dirs;

        try {
                po::options_description desc("allowed options");
                desc.add_options()
                        ("help", "xxxxxxxxxxxxxxxxxx")
                        ("optimize", po::value<int>()->default_value(10), 
                                "set compression level")
                        ("include,I", po::value<std::vector<std::string>>(),
                                "include directory");

                po::variables_map vm;
                po::store(po::parse_command_line(argc, argv, desc), vm);
                po::notify(vm);

                if (vm.count("help")) {
                        std::cout << desc << std::endl;
                        return EXIT_FAILURE;
                }

                if (vm.count("optimize")) {
                        std::cout << "compression level was set to " << 
                                vm["optimize"].as<int>() << std::endl;
                } else {
                        std::cout << "optimize level was not set" 
                                << std::endl;
                }

                if (vm.count("include")) {
                        inc_dirs = vm["include"].
                                as<std::vector<std::string>>();
                        for (auto it = inc_dirs.begin(); it != inc_dirs.end(); ++it)
                                std::cout << *it << std::endl;
                } else {
                        std::cout << "include dir was not set" << std::endl;
                }
        } catch (const std::exception &e) {
                std::cout << e.what() << std::endl;
        }
}
