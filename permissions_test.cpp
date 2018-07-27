#include "permissions_test.hpp"
#include "eosiolib/action.hpp"

namespace eosio {

    void permissions_test::hasauth(account_name account) {
        eosio::print(has_auth(account));
    }

    void permissions_test::reqauth(account_name account) {
        require_auth(account);
    }

    void permissions_test::reqauth2(account_name account, permission_name permission) {
        require_auth2(account, permission);
        eosio::print(name{account});
        eosio::print('@');
        eosio::print(name{permission});
    }

    void permissions_test::send(account_name sent, permission_name p_sent, account_name req) {
        action(permission_level{sent, p_sent},
               N(test), N(reqauth),
               std::make_tuple(req)
        ).send();
    }

    void permissions_test::send2(account_name sent, permission_name p_sent, account_name req, permission_name p_req) {
        action(permission_level{sent, p_sent},
               N(test), N(reqauth2),
               std::make_tuple(req, p_req)
        ).send();
    }
}

EOSIO_ABI(eosio::permissions_test, (hasauth)(reqauth)(reqauth2)(send)(send2))
