#pragma once

#include <eosiolib/eosio.hpp>

namespace eosio {

    class permissions_test : public contract {
    public:
        using contract::contract;

        //@abi action
        void hasauth(account_name account);

        //@abi action
        void reqauth(account_name account);

        //@abi action
        void reqauth2(account_name account, permission_name permission);

        //@abi action
        void send(account_name sent, permission_name p_sent, account_name req);

        //@abi action
        void send2(account_name sent, permission_name p_sent, account_name req, permission_name p_req);
    };
}
