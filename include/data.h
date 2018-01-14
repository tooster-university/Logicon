//
// Created by rufus on 11.12.17.
//

#ifndef LOGICON_DATA_H
#define LOGICON_DATA_H

#include "json/json.hpp"


namespace Logicon {
    class GCircuit;

    /**
     * @brief Representing data in JSON format.
     *
     * Class behaves like a map of various types, converts passed values to JSON objects and vice versa.
     * Allows to store values such as: labels, image paths, descriptions, positions of buttons, data about GBlocks etc.
     */
    class Data {
    public:
        static void save(std::string path, std::shared_ptr<GCircuit> gCircuit);

        static std::shared_ptr<GCircuit> read(std::string path);
    };
} // namespace Logicon


#endif //LOGICON_DATA_H
