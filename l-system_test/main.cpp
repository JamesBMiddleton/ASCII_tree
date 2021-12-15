#include <sstream>
#include <map>
#include <iostream>

void l_system(std::string axiom, std::map<char, std::string> rules, int iterations=10)
{
    std::stringstream ss;
    for (char letter : axiom)
    {
        ss << rules[letter];
    }
    std::string output;
    ss >> output;
    std::cout << output << std::endl;
    if (iterations)
        l_system(output, rules, --iterations);
}

int main()
{
    std::string axiom{"A"};
    std::map<char, std::string> rules = {{'A', "AB"}, {'B', "ABA"}};
    l_system("A", rules, 10);
    return 0;
}

