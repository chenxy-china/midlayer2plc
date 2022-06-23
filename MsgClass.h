#include <string.h>
#include <boost/json.hpp>

using namespace boost::json;

namespace NSJsonClass {

    class MsgBase
    {
        public:
            string type;
            MsgBase(string type=""):
            type(type){};
    };

    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MsgBase const &c) {
        auto & jo = jv.emplace_object();
        jo["type"] = c.type;
    };

    MsgBase tag_invoke(boost::json::value_to_tag<MsgBase>, boost::json::value const &jv) {
        auto &jo = jv.as_object();
        return MsgBase(jo.at("type").as_string());
    };

    class MsgSetReg:public MsgBase
    {
        public:
            int address;
            int value;
            MsgSetReg(int address,int value):
            address(address),value(value){};

    };

    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MsgSetReg const &c) {
        auto & jo = jv.emplace_object();
        jo["address"] = c.address;
        jo["value"] = c.value;
    };

    MsgSetReg tag_invoke(boost::json::value_to_tag<MsgSetReg>, boost::json::value const &jv) {
        auto &jo = jv.as_object();
        return MsgSetReg(jo.at("address").as_int64(),jo.at("value").as_int64());
    };

    class MsgSetRegs:public MsgBase
    {
        public:
            std::pair<int,int> regsSetting;
            MsgSetRegs(std::pair<int,int> regsSetting):
            regsSetting(regsSetting){};

    };

    class MsgGetReg:public MsgBase
    {
        public:
            int address;
            int value;
            MsgGetReg(int address,int value=0):
            address(address),value(value){};
    };

    class MsgGetRegs:public MsgBase
    {
        public:
            std::pair<int,int> regsVal;
            MsgGetRegs(std::pair<int,int> regsVal):
            regsVal(regsVal){};
    };

    class MsgSubReg:public MsgBase
    {
        public:
            int address;
            MsgSubReg(int address):
            address(address){};
    };

    class SubscribeRegs
    {
        public:
            std::map<int,int> subAddrs;
            SubscribeRegs();
            void subReg(int address)
            {
                subAddrs.insert({address,0});
            };
            void unSubReg(int address)
            {
                subAddrs.erase(address);
            };
    };
}