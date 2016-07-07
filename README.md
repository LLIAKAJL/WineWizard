# Wine Wizard

Wine Wizard is a modern GUI for Wine written in Qt.

The main feature of Wine Wizard are solutions the user can create and edit. A solution is a specification that declares what libraries to install and what settings and versions of Wine to use while installing or running MS Windows application. If a solution already exist, the user can choose it and run the installer file, and Wine Wizard will do everything automatically.

Unlike [PlayOnLinux](https://www.playonlinux.com/), Wine Wizard doesn't require any shell scripting knowledge, a solution can be created by using a mouse only. And unlilke shell scripts, solutions are safe and don't require verification and approval, so the solution database can grow quickly.

If shell scripts are required, the user can create a custom one (this feature is disabled by default). Any script will be shown before execution.

Other features:

- Very easy to download and install packages, unlimited count of mirrors
- Both 32 bit and 64 bit Wine is supported
- Program logs based advices
- Sandbox is created automatically
- Different prefixes and versions of Wine for each application
- Automatic creation, simple managing and deletion of prefixes and application shortcuts.
- Automatic cleaning of temporary files after installation

## Install

Arch Linux: you can install Wine Wizard [from AUR](https://aur.archlinux.org/packages/wine-wizard-git/).

Ubuntu: https://launchpad.net/~nightuser/+archive/ubuntu/winewizard

## Build from git

Wine Wizard requires these dependencies:

- Qt5 (core, widgets, network)
- unzip
- cabextract
- bzip2
- tar
- wget

To clone sources from git, run:

```sh
$ git clone https://github.com/LLIAKAJL/WineWizard.git
$ cd WineWizard
```

And then run this to build and install Wine Wizard:

```sh
$ mkdir build && cd build
$ qmake ..
$ make && make install
```

It will install Wine Wizard into `/usr/local/` directory.
