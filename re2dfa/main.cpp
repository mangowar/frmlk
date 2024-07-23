#include "api.hpp"
#include <iostream>
#include <fstream>
#include <string>

extern std::string add_eps(std::string s);
extern std::string make_poliz(std::string s);

extern DFA re2dfa(const std::string& regExp);
using namespace std;
int main() {
  std::ifstream infile("re2dfa.in");
  std::ofstream outfile("re2dfa.out");

  std::string line;
  std::getline(infile, line);
  outfile << re2dfa(line).to_string();
  return 0;
}
