//
// Created by rufus on 11.12.17.
//

#include "data.h"
#include "gui/gCircuit.h"
#include <fstream>
#include <gates/not.h>
#include <gates/delay.h>
#include <gates/switch.h>
#include <gates/and.h>
#include <gates/or.h>
#include <gates/xor.h>
#include <gates/nand.h>
#include <gates/nor.h>
#include <gates/xnor.h>
#include <gates/clock.h>
#include <gates/input.h>

namespace Logicon{
    void Data::save(std::string path, std::shared_ptr<GCircuit> gCircuit) {
        const std::shared_ptr<Circuit> circuit = gCircuit->getCircuit();
        nlohmann::json j_gates = nlohmann::json::array();
        for (auto gate : circuit->getGates()) {
            auto pos = gCircuit->getGBlockByID(gate->id)->getPosition();
            nlohmann::json j_in_ports = nlohmann::json::array();
            auto in_ports = gate->getInputs();
            for (int i = 0; i < in_ports.size(); i++) {
                nlohmann::json j_in_port = nlohmann::json::array();
                auto in_port = in_ports[i].second;
                for (int l = 0; l < in_port.size(); l++) {
                    j_in_port += {{"id",   in_port[l].id},
                                  {"port", in_port[l].port}};
                }
                nlohmann::json j_in = {{"state", in_ports[i].first},
                                       {"in",    j_in_port}};
                j_in_ports += j_in;
            }
            auto out = gate->getOutputs()[0];
            nlohmann::json j_out_conn = nlohmann::json::array();
            for (int i = 0; i < out.second.size(); i++) {
                nlohmann::json conn = {{"id",   out.second[i].id},
                                       {"port", out.second[i].port}};
                j_out_conn += conn;
            }
            nlohmann::json j_out = {{"state", out.first},
                                    {"ports", j_out_conn}};

            nlohmann::json j_gate = {{"id",   gate->id},
                                     {"x",    pos.x},
                                     {"y",    pos.y},
                                     {"type", gate->getGateType()},
                                     {"in",   j_in_ports},
                                     {"out",  j_out}};
            j_gates += j_gate;
        }
        nlohmann::json j_circuit = {{"flag", circuit->INITIALIZED_FLAG},
                                    {"id",   circuit->getId()},
                                    {"list", j_gates}};

        std::ofstream file(path);
        if (file.is_open()) {
            file << j_circuit;
            file.close();
        }
    }

    std::shared_ptr<GCircuit> Data::read(std::string path) {
        std::ifstream my_file(path);
        nlohmann::json j;
        j << my_file;
        std::shared_ptr<Circuit> circuit = std::make_shared<Circuit>(j.value("id", 0));
        std::shared_ptr<GCircuit> gCircuit = std::make_shared<GCircuit>(circuit);
        circuit->INITIALIZED_FLAG = j.value("flag", 0);
        auto arr = j["list"];
        //std::cout << arr << std::endl;
        //std::cout.flush();
        for (int i = 0; i < arr.size(); i++) {
            nlohmann::json j_gate = arr[i];
            //std::cout << j_gate;
            //std::cout.flush();
            std::shared_ptr<Gate> gate;
            GATE_TYPE type = (GATE_TYPE) (j_gate.value("type", -1));
            ID id = j_gate.value("id", 1);
            UI::Vec2 pos = UI::Vec2(j_gate.value("x", 1), j_gate.value("y", 1));
            switch (type) {
                case NOT:
                    gate = std::make_shared<Not>(id);
                    break;
                case DELAY:
                    gate = std::make_shared<Delay>(id);
                    break;
                case SWITCH_ON:
                    gate = std::make_shared<Switch>(id);
                    std::static_pointer_cast<Switch, Gate>(gate)->setClicked(true);
                    break;
                case SWITCH_OFF:
                    gate = std::make_shared<Switch>(id);
                    std::static_pointer_cast<Switch, Gate>(gate)->setClicked(false);
                    break;
                case AND:
                    gate = std::make_shared<And>(id);
                    break;
                case OR:
                    gate = std::make_shared<Or>(id);
                    break;
                case XOR:
                    gate = std::make_shared<Xor>(id);
                    break;
                case NAND:
                    gate = std::make_shared<Nand>(id);
                    break;
                case NOR:
                    gate = std::make_shared<Nor>(id);
                    break;
                case XNOR:
                    gate = std::make_shared<Xnor>(id);
                    break;
                case CLOCK:
                    gate = std::make_shared<Clock>(id);
                    break;
                case INPUT_ON:
                    gate = std::make_shared<Input>(id);
                    std::static_pointer_cast<Input, Gate>(gate)->setClicked(true);
                    break;
                case INPUT_OFF:
                    gate = std::make_shared<Input>(id);
                    std::static_pointer_cast<Input, Gate>(gate)->setClicked(false);
                    break;
            }
            auto ins = j_gate["in"];
            auto out = j_gate["out"];
            std::cout << ins << std::endl;
            gate->setOutputState(0, out["state"]);
            auto out_conns = out["ports"];
            for (int l = 0; l < out_conns.size(); l++) {
                gate->addOutputConnection(0, out_conns[l]["id"], out_conns[l]["port"]);
            }
            for (int l = 0; l < ins.size(); l++) {
                auto in = ins[l];
                auto in_port = in["in"];
                std::cout << in << std::endl;
                std::cout << in_port << std::endl;
                for (int r = 0; r < in_port.size(); r++) {
                    gate->setInputConnection(l, in_port[r].value("id", 0), in_port[r].value("port", 0));
                }
                gate->setInputState(l, in.value("state", 0));
            }
            circuit->add(gate);
            gCircuit->insert(gate, pos);
        }
        return gCircuit;
    }

} // namespace Logicon