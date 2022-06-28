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

    /*读取bits*/
    class MsgGetBits:public MsgBase
    {
        public:
            int address;
            int size;
            MsgGetBits(int address,int size):
            address(address),size(size){};
    };
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MsgGetBits const &c) {
        auto & jo = jv.emplace_object();
        jo["type"] = c.type;
        jo["address"] = c.address;
        jo["size"] = c.size;
    };

    MsgGetBits tag_invoke(boost::json::value_to_tag<MsgGetBits>, boost::json::value const &jv) {
        auto &jo = jv.as_object();
        return MsgGetBits(jo.at("address").as_int64(),jo.at("size").as_int64());
    };

    /*读取多个寄存器*/
    class MsgGetRegs:public MsgBase
    {
        public:
            int address;
            int size;
            MsgGetRegs(int address,int size):
            address(address),size(size){};
    };
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MsgGetRegs const &c) {
        auto & jo = jv.emplace_object();
        jo["type"] = c.type;
        jo["address"] = c.address;
        jo["size"] = c.size;
    };

    MsgGetRegs tag_invoke(boost::json::value_to_tag<MsgGetRegs>, boost::json::value const &jv) {
        auto &jo = jv.as_object();
        return MsgGetRegs(jo.at("address").as_int64(),jo.at("size").as_int64());
    };

    /*设置单个寄存器*/
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
        jo["type"] = c.type;
        jo["address"] = c.address;
        jo["value"] = c.value;
    };

    MsgSetReg tag_invoke(boost::json::value_to_tag<MsgSetReg>, boost::json::value const &jv) {
        auto &jo = jv.as_object();
        return MsgSetReg(jo.at("address").as_int64(),jo.at("value").as_int64());
    };

    /*设置多个寄存器*/
    class MsgSetRegs:public MsgBase
    {
        public:
            int address;
            int size;
            std::vector<int> value;
            MsgSetRegs(int address,int size,std::vector<int> value):
            address(address),size(size),value(value){};

    };
    void tag_invoke(boost::json::value_from_tag, boost::json::value &jv, MsgSetRegs const &c) {
        auto & jo = jv.emplace_object();
        jo["type"] = c.type;
        jo["address"] = c.address;
        jo["size"] = c.size;
        jo["value"] = value_from(c.value);
    };

    MsgSetRegs tag_invoke(boost::json::value_to_tag<MsgSetRegs>, boost::json::value const &jv) {
        auto &jo = jv.as_object();
        auto vec_v = value_to< std::vector< int > >(jo.at("value"));
        return MsgSetRegs(jo.at("address").as_int64(),jo.at("size").as_int64(),vec_v);
    };

    /////////////////////////////////////////////

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