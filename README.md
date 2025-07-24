# GAMEDB

## Description

This application is build to ease archiving the game names and their info in an universal manner. This application saves all the data in a JSON file caled info.json. Furthermore there is an logindata.json file which contains all the login details. I already know its not as secure as keeping it in a database. However this file contains hashed passwords which is moderate amount of security for this small of an application which doesnt rely on an internet connection.

## Compilation & Usage

Just copy the following commands and paste them in your terminal

### Linux & macOS

**STEP 1: Install required dependencies and development packages**
>NOTE: Im assuming you are using fedora based linux distro if not adjust the following command to your distro

```bash
sudo dnf install openssl-devel cjson-devel ncurses-devel cmake make clang gcc && sudo dnf install @development-tools
```

**STEP 2: Clone the repo**

```bash
git clone https://github.com/daniaru6340/GameDB.git
```

**STEP 3: Create the make file using cmake**

```bash
mkdir build && cd build && cmake ..
```

**STEP 4: Build the program using make**

```bash
make
```

**STEP 5: Running the program**

```bash
cd output/bin && ./GameDB
```

### Windows

Im sorry that there is no native windows version however you could run the program using wsl, cygwin, msys, or any other linux terminal emulators.

if you dont wish to go through the hassle of porting the application for windows you may download the port i have created using msys2. [Click here](https://bit.ly/GameDBwindowsport) to download the installer. If that does not work try [this.](https://bit.ly/44KI9Vt)