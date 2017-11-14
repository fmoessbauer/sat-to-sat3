#include <vector>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <string>

using VAR     = unsigned int; 
using LITERAL = int; 
using CLAUSE  = std::pair<LITERAL, LITERAL>; // two-literal clause
using CNF     = std::pair<std::vector<unsigned int>, std::vector<LITERAL>>;

/**
 * Efficient implementation of an SAT -> SAT-3 converter
 *
 * Internally the CNF is stored in CSR format
 */
class SAT_Converter {
private:
  static VAR maxvar(const CNF & F){
    auto minmax = std::minmax_element(F.second.begin(), F.second.end());
    return std::max(std::abs(*(minmax.first)), *(minmax.second));
  }

  static VAR replacevar(CNF & F, VAR var, VAR nextfreevar){
    // do not replace first occurance
    bool firsthit = true;
    for(auto & l : F.second){
      if(l == var){
        if(firsthit){firsthit=false; continue;}
        l = nextfreevar++;
      } else if(l == -var){
        if(firsthit){firsthit=false; continue;}
        l = -nextfreevar++;
      }
    }
    return nextfreevar;
  }

  static inline CLAUSE implication_clause(VAR a, VAR b){
    //( "=>" ) 
    return CLAUSE{-static_cast<LITERAL>(a),
                   static_cast<LITERAL>(b)};
  }

  static inline void ensure_implication(CNF & F, VAR a, VAR b){
    auto implc = implication_clause(a, b); 
    F.first.push_back(F.first.back()+2);
    F.second.emplace_back(implc.first);
    F.second.emplace_back(implc.second);
  }

public:
  /**
   * Convert arbitrary CNF to a SAT-3 CNF where 
   * each variable occurs at most 3 times
   */
  static void to_sat3(CNF & F){
    std::unordered_map<VAR,int> vars;
    for(const auto & l : F.second){
      ++(vars[std::abs(l)]);
    }
    auto it = vars.begin();
    while(it != vars.end()){
      if(it->second <= 3){
        it = vars.erase(it);
      } else {
        ++it;
      }
    }
    VAR nextfreevar = maxvar(F)+1;
    for(const auto & kv : vars){
      VAR newfreevar = replacevar(F, kv.first, nextfreevar);
      ensure_implication(F, kv.first, nextfreevar);
      for(VAR i=nextfreevar; i<newfreevar; ++i){
        ensure_implication(F, i, i+1);
      }
      ensure_implication(F, newfreevar, kv.first);
      nextfreevar = newfreevar;
    }
  }

  /**
   * Convert CNF to DIMACS format
   */
  static std::string to_dimacs(const CNF & F){
    int num_clauses = F.first.size(); 
    VAR max_var     = maxvar(F);
  
    std::stringstream buffer;
    auto it = F.first.begin();
    auto beg = 0;
    while(it != F.first.end()){
      auto end = *it;
      for(auto i=beg; i<end; ++i){
        buffer << F.second[i] << " "; 
      }
      buffer << "0" << std::endl;
      beg = end;
      ++it;
    }
    std::stringstream result;
    result << "p cnf " << max_var << " " << num_clauses << std::endl;
    return result.str() + buffer.str();
  }

  /**
   * Get CNF from DIMACS input
   */
  template<typename IStream>
  static CNF from_dimacs(IStream str){
    CNF F;
    std::string buffer;
    std::getline(str, buffer); // skip header
    auto pos = buffer.find("p cnf");
    if(pos == std::string::npos){
      throw("Input not in DIMACS format");
    }
    int c_idx = 0; // end index of clause
    while(std::getline(str, buffer)){
      if(buffer[0] == 'c') { continue; }
      std::stringstream clause(buffer);
      while(true){
        auto lstr = *(std::istream_iterator<std::string>(clause));
        if(lstr == "") break;
        LITERAL l = std::stoi(lstr);
        if(l != 0){
          F.second.push_back(l);
          ++c_idx;
        } else {
          F.first.push_back(c_idx);
          break;
        }
      }
    }
    return F;
  }
};
