#include "Cocktail.hpp"

Cocktail::Cocktail(String name, CocktailPump pumps[4]) {
    this->name = name;

    for (int i=0; i<4; i++)
        pumpConfig[i] = pumps[i];
  
}
 
String Cocktail::getName() {
    return name;
}