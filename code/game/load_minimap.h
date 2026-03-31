#ifndef LOAD_MINIMAP_HPP
#define LOAD_MINIMAP_HPP

    #include "load_obj.h"

    class Map
    {
        private:
            int selectedMap;
            Loader loader_map;

        public:
            Map();
            void selectMap(int selectedMap);
            void setupLights();
            void load_map();
    };

#endif