#include <array>
#include <string>
#include "TileRegistry.h"

//Registre qui garde toutes les infos pour les différentes Tiles. on associe par id_tile = index.
//Le extern permet de pas créer un copie pour chaque fichier qui inclut tileregistry
extern const std::array<RegistryEntry, 16> TileRegistry ={
    RegistryEntry{"water", false, false},
    RegistryEntry{"grass", true, false}, 
    RegistryEntry{"sand", true, false},
    RegistryEntry{"air_land", true, false},
    RegistryEntry{"water_land", true, false},
    RegistryEntry{"fire_land", true, false},
    RegistryEntry{"earth_land", true, false},
    RegistryEntry{"air_relic",   true, true},
    RegistryEntry{"water_relic", true, true},
    RegistryEntry{"fire_relic",  true, true},
    RegistryEntry{"earth_relic", true, true}
};