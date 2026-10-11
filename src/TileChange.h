#pragma once

//sert à enregistrer les modifications apportées. 
//est une struct car c'est temporaire, pas besion d'en faire quelque chose de compliqué.
struct TileChange {
    int x; //coordonnées gloable, pas au sein d'un chunk
    int y;
    int tile_id;
};