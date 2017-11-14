#include "fast/sat_converter.hpp"

#include <iostream>
#include <fstream>

void print_usage(char * pname){
  std::cout << "This program takes a CNF in DIMACS format and reduces it "
            << "to a SAT-3 CNF" << std::endl
            << "Usage: " << pname << " <input.dim> <output.dim>" << std::endl;
}

int main(int argc, char** argv){
  if(argc != 3){
    print_usage(argv[0]);
    return 1;
  }
  
  auto input  = std::string(argv[1]);
  auto output = std::string(argv[2]);

  CNF F = SAT_Converter::from_dimacs(std::ifstream(input, std::ios::in));

  SAT_Converter::to_sat3(F);

  std::ofstream os(output);
  os << SAT_Converter::to_dimacs(F);
  os.flush();

  return 0;
}
