#ifndef LOAD_MINIMAP_HPP
#define LOAD_MINIMAP_HPP

class Map
{
    private:
        int selectedMap;

    public:
        void selectMap(int selectedMap);
        void setupLights();
        void load_map();
};

#endif