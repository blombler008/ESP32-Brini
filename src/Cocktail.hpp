#ifndef COCKTAIL_HPP
#define COCKTAIL_HPP

#include "config.hpp"

typedef struct {
    uint8_t pump;
    uint16_t time;
} CocktailPump;


class Cocktail {
    private: 
        CocktailPump pumpConfig[4];
        String name;
    public:
        Cocktail(String name, CocktailPump pumps[4]); 
        String getName();
};

#endif
