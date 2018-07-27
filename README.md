# EOSIO Permissions System Test

An EOSIO smart contract for testing different permissions and authorities.

I used this smart contract to teach myself how the permissions system actually
works. At the time, there was a lack of documentation on the subject and
most people seemed confused. It was hard to find a definitive guide, so I
wrote this simple contract to test and understand better the inner workings
of the powerful EOSIO permissions system.

Most of all I wanted to understand the workings of the `eosio.code` permission
when sending inline actions to other contracts. This is a recap of what I
understood, in the context of a Stack Exchange question:

> More info here: https://eosio.stackexchange.com/a/1718/1834

## EOS Permission Model - Overview

- An account can have various permissions (like owner and active), which are represented by an account@permission pair.
- Permissions (which are like roles) can be linked to specific actions of specific contracts to allow those permissions to execute those actions (`linkauth`). By default, the `owner` and `active` permissions can do anything except `active` can't change the owner permission.
- Permissions are controlled by an "authority", which is the multisig configuration of who can give that permission (in other words, who can act under that role).
- Within this multisig configuration, you can have a combination of public keys and other permissions (account@permission pairs), which makes permissions an intrinsically recursive construct.

## Acting as another account

Contracts in general should use `require_auth(account)` and not `require_auth2(account, permission)` unless there is a very specific reason to do so. Using `require_auth2` to require a specific permission of an account can hinder the configurability of the EOSIO permission system. This is because in general, if actions simply require the auth of an account, then that means they are implicitly requiring one of the following permissions of that account:

1. The `owner` permission.
2. The `active` permission.
3. Any other custom permission that the user decided to create for their account in order to give granular authorization to specific contract actions.

Point 3 means that a user can create a permission (as I mentioned, it can be seen as a "role") called for example `ramtrader` and then use `linkauth` to authorize that permission to use the `eosio::buyram` and `eosio::sellram` system contract actions. This can work with any contract, not only the system contract. This way, when defining the `ramtrader` permission, users will need to specify an *authority* for it (a multisig configuration), and this authority could specify that the only object that can act under this permission is `accountb@active`, for example, giving access to the `accountb` account to buy and sell RAM for `account`.

## Contract code acting as another account

Now that we understand how to act as another account, we can figure out how to allow a contract's code to act as another account, be it for transferring funds (`eosio.token::transfer` action) or just calling another contract's actions.

When contracts call inline actions, they are supposed to send the right permissions for that action. If for instance a contract that lives in the `contract` account would try to buy RAM for `account` using the funds of `account` itself, it would need to provide the same permissions that `account` is required to provide when they buy RAM for themselves manually. If we use the `account@active` permission, then the contract would need to send that permission in the inline action, and not `contract@eosio.code` as many of us could end up thinking (the documentation on this is very scarce and confusing). In order for the code in the `contract` account to be able to provide that permission, first `account` would have to add authorization for the code of `contract` to the authority (multisig config) that rules it's `account@active` permission.

This can be achieved by adding the `contract@eosio.code` permission to the authority, which is a special permission defined by the EOSIO software to specify that **only the contract code of the `contract` account** will be able to act under the permission (role) ruled by that authority. This means that the `account@active` authority would contain the public key that the owner of that account controls, as well as the `contract@eosio.code` permission.

This effectively implements what you were looking for: Authorizing a contract's code to act as another account, but not letting the contract's account act as the other account.

If you wanted to let a contract's account act as yourself but not the contract's code, you would have to do the same thing but instead of setting `contract@eosio.code` you would set `contract@active` or some other more specific (limited) permission.

## Setting up the permission authority

To configure your account to allow `contract@eosio.code` to act on your behalf, you would need to issue a transaction to the `eosio::updateauth` action with the properly formatted authority data. One way to do it using `cleos` is what @confused00 showed in his example:

    cleos set account permission <YOUR_ACCOUNT> active '{"threshold": 1,"keys": [{"key": "<YOUR_PUBLIC_KEY>","weight": 1}],"accounts": [{"permission":{"actor":"<CONTRACT_ACCOUNT>","permission":"eosio.code"},"weight":1}]}' owner -p <YOUR_ACCOUNT>

It might be easier to save a `data.json` file and then put the payload in there and point `cleos` to it:

> data.json

    {
      "threshold": 1,
      "keys": [
        {
          "key": "<YOUR_PUBLIC_KEY>",
          "weight": 1
        }
      ],
      "accounts": [
        {
          "permission": {
            "actor": "<CONTRACT_ACCOUNT>",
            "permission": "eosio.code"
          },
          "weight": 1
        }
      ]
    }

and then:

    cleos set account permission <YOUR_ACCOUNT> active data.json owner -p <YOUR_ACCOUNT>
