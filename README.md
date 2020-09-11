
# CAN - Cryptobadge Alliance Network

CAN is a merit-based identity network, powered by CryptoBadge, a blockchain-based certificate system.

CryptoBadgelets users identify themselves through expertise, achievements, and contributions. It captures a variety of certifiable values and lets you build your personal brands. CryptoBadge is a universal, eternal, and verifiable acknowledgement system shared across communities. CryptoBadge divides badge winner’s merits into tokenizable assets and provides the users with diverse opportunities to gain reputation, power, and economic benefit

---

**Note: If you used our scripts to build/install EOSIO, please run the [Uninstall Script](#uninstall-script) before using our prebuilt binary packages.**

---

## Prebuilt Binaries

Prebuilt CANNET software packages are available for the operating systems below. Find and follow the instructions for your OS:

### Mac OS X:

#### Mac OS X Brew Install
```sh
$ brew tap canfoundation/cannet
$ brew install cannet
```
#### Mac OS X Brew Uninstall
```sh
$ brew remove cannet
```

### Ubuntu Linux:

#### Ubuntu 18.04 Package Install
```sh
$ wget https://github.com/canfoundation/CAN/releases/download/can-v1.0.0/cannet_1.0.0-ubuntu-18.04_amd64.deb
$ sudo apt install ./cannet_1.0.0-ubuntu-18.04_amd64.deb
```
#### Ubuntu Package Uninstall
```sh
$ sudo apt remove cannet
```

## Uninstall Script
To uninstall the CANNET built/installed binaries and dependencies, run:
```sh
./scripts/eosio_uninstall.sh
```

If you have previously installed EOSIO using build scripts, you can execute `eosio_uninstall.sh` to uninstall.
- Passing `-y` will answer yes to all prompts (does not remove data directories)
- Passing `-f` will remove data directories (be very careful with this)
- Passing in `-i` allows you to specify where your cannet installation is located

## Supported Operating Systems
CAN currently supports the following operating systems:  

1. Ubuntu 18.04
2. MacOS 10.14 (Mojave)

## Chain API

There're two endpoint for CANchain:
- Mainnet: https://canfoundation.io
- Testnet: https://test.canfoundation.io

Chain API documents can be found at [docs/01_nodeos/03_plugins/chain_api_plugin/swagger.json](docs/01_nodeos/03_plugins/chain_api_plugin/swagger.json)