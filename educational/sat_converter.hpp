#include <set>
#include <vector>
#include <iterator>
#include <unordered_map>
#include <algorithm>
#include <sstream>
#include <string>

using VAR     = unsigned int; 
using LITERAL = int; 
using CLAUSE  = std::set<LITERAL>;
using CNF     = std::vector<CLAUSE>;

/**
 * Educational implementation of an SAT -> SAT-3 converter
 *
 * Internally the CNF is stored in CSR format
 */
class SAT_Converter {
private:
  static VAR maxvar(const CNF & F){
    VAR max_var = 0;
    for(const auto & c : F){
      for(const auto & l : c){
        if(std::abs(l) > max_var) max_var = std::abs(l);
      }
    }
    return max_var;
  }

  static VAR replacevar(CNF & F, VAR var, VAR nextfreevar){
    // do not replace first occurance
    bool firsthit = true;
    for(auto & c : F){
      auto lit = c.begin();
      while(lit != c.end()){
        if(*lit == var){
          if(firsthit){firsthit=false; ++lit; continue;}
          lit = c.erase(lit);
          c.insert(nextfreevar++);
        } else if(*lit == -var){
          if(firsthit){firsthit=false; ++lit; continue;}
          lit = c.erase(lit);
          c.insert(-nextfreevar++);
        } else {
          ++lit;
        }
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
    F.emplace_back(implication_clause(a, b));
  }

public:
  /**
   * Convert arbitrary CNF to a SAT-3 CNF where 
   * each variable occurs at most 3 times
   */
  static void to_sat3(CNF & F){
    std::unordered_map<VAR,int> vars;
    for(const auto & c : F){
      for(const auto & l : c){
        ++(vars[std::abs(l)]);
      }
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
    int num_clauses = F.size(); 
    VAR max_var     = maxvar(F);
  
    std::stringstream buffer;
    for(auto & c : F){
      for(auto & l : c){
        buffer << l << " ";
      }
      buffer << "0" << std::endl;
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
    while(std::getline(str, buffer)){
      if(buffer[0] == 'c') { continue; }
      CLAUSE c;
      std::stringstream clause(buffer);
      while(true){
        auto lstr = *(std::istream_iterator<std::string>(clause));
        if(lstr == "") break;
        LITERAL l = std::stoi(lstr);
        if(l != 0){
          c.insert(l);
        } else {
          F.push_back(c);
          break;
        }
      }
    }
    return F;
  }

};
