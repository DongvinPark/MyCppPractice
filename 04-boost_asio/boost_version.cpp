#include<iostream>
#include<boost/version.hpp>

int main(){
    std::cout << "Raw BOOST_VERSION integer: " << BOOST_VERSION << "\n";

    // Method 1: Using BOOST_LIB_VERSION (String format)
    std::cout << "Boost Library Version (String): " << BOOST_LIB_VERSION << "\n";

    // Method 2: Using BOOST_VERSION (Decoded numeric format)
    constexpr int major = BOOST_VERSION / 100000;
    constexpr int minor = BOOST_VERSION / 100 % 1000;
    constexpr int patch = BOOST_VERSION % 100;

    std::cout << "Boost Library Version (Decoded): "
              << major << "." << minor << "." << patch << "\n";

    return 0;
}