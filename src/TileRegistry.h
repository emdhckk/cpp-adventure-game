#pragma once

#include <array>
#include <string>

//Structure qui enregistre les infos d'une tile.
struct RegistryEntry{
    std::string name;
    bool is_walkable;
    bool is_interactable;
    
};
//Rempli à la main, c'est un registre qui garde toutes les infos pour les différentes Tiles. on associe par id_tile = index.
extern const std::array<RegistryEntry, 16> TileRegistry;
