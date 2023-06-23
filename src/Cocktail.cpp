#include "Cocktail.hpp"
#include <iostream>
#include <string>
Cocktail::Cocktail(const char* name, CocktailPump pumps[4]) {
    memcpy(this->name, name, 18);
      
    for (int i=0; i<4; i++)
        pumpConfig[i] = pumps[i];
  
}
 
const char* Cocktail::getName() {
    return (name); 
    
}