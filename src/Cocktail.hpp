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
        char name[18] = {0};
    public:
        Cocktail(const char* name, CocktailPump pumps[4]); 
        const char* getName();
};

#endif
