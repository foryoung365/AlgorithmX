#include "Solver.h"
#include <iostream>
#include <sstream>

using namespace AlgorithmX;
using namespace std;
int main(){
    using R = std::tuple<std::string, int>;
    std::vector<R> requirements = {{"req1", 1}, {"req2", 2}};
    std::unordered_map<ActionID, std::vector<R>> actions = {
        {"action1", {{"req1", 1}}},
        {"action2", {{"req2", 2}}}
    };

    std::unordered_set<R> optionRequirements = {{"req3", 3}};

    AlgorithmX::Solver<R> s(requirements, actions, optionRequirements);

    cout << "end" << endl;

    istringstream is("M 1 2 T 3");
    string str;
    string str2;
    int n;

    is >> n;
    is >> str;
    is >> str2;
    cout << "n: " << n << ", str: " << str << ",str2:" << str2 << endl;

}