# REHTI MMORPG

Welcome to the Rehti MMORPG repository! This project implements a barebones MMORPG game written in C++.

Table of contents:

- [Introduction](#introduction)
- [Gameplay](#gameplay)
- [How to play](#how-to-play)
- [Installation](#installation)
- [Development](#development)
- [Technical overview](#technical-overview)
  - [Client](#client)
  - [Server](#server)
  - [Database](#database)
  - [API](#api)
- [Project practices](#project-practices)

## Introduction

Rehti MMORPG is heavily inspired by [Old School Runescape](https://oldschool.runescape.com/). In the core of the project is the game engine which implements all of the game's functionalities and supporting elements. On top of the engine we have built a highly customisable game (content-wise). To learn more about the game's content and elements, see the [assets document](/assets/README.md).

## Gameplay

In the game, the player controls their own character in a 3D game world. Players can move around the world and explore their surroundings. The game world includes characters controlled by other players as well as non-playable characters (NPCs). Players can attack other players and NPCs. When characters die, they drop their items on the ground. The player's character has skills categorized into resource gathering skills, resource processing skills, and combat skills.

Resource gathering skills include woodcutting, mining, and fishing. The commonality among these is that by developing these skills, players acquire resources. In woodcutting, players can interact with the game world's trees to obtain wood resources. In mining, players can interact with the game world's ore rocks to obtain ore resources. In fishing, players can interact with the game world's bodies of water to obtain raw fish.

Resource processing skills include forging and cooking. In forging, players can utilize the ore resources obtained through mining to craft various equipment that the player can wear and use in combat. With cooking, players can use the raw fish obtained through fishing to cook edible fish. These food resources allow players to recover from combat injuries.

Combat skills include accuracy, strength, and defense. The purpose of these skills is to improve the player's ability to succeed in combat against other characters. Accuracy enhances the likelihood of the character's attacks hitting (with additional considerations for equipment). Strength affects the amount of damage the player deals, and defense increases the likelihood of the player evading attacks. Players utilize combat skills by either attacking other characters or defending against attacks from other characters. Combat can be initiated by clicking on another character, causing the player's character to commence attacking the target.

The game, in its sandbox-style, allows players to set their own goals. This could involve peaceful skill development, enhancing the character's combat strength, or even collecting items, depending on the player's preferences.

## How to play

You can play the game by installing the client, see [here](#client-1).

### Login/Signup

When you start the client, you will be prompted to input your username and password in the console. If you don't have an account, you can simply input a new username and password and you will be automatically signed up. If you already have an account, you can input your username and password and you will be logged in.

### Controls

All the controls happen with the mouse.

**Moving** You can move your character by clicking on the ground (Both left and right click work). You can interact with objects by clicking on them.

**Attacking** You can attack characters (Players and NPC's) by left-clicking on them.

**Talking** You can talk to NPC's by right-clicking on them.

**Interacting with objects** You can interact with objects by clicking on them (Both left and right click work).

**Moving the camera** You can move the camera by holding down the scroll wheel and moving the mouse. You can zoom in and out by scrolling the scroll wheel.

**Managing items** Items are managed from the inventory tab in the bottom right corner. A dropdown menu opens when cliking on the item.

**Managing equipment** Equipment can be equiped from the inventory. You can unequip items by clicking on the item in the equipment tab.

## Installation

### Prerequisites

1. Make sure to fetch git submodules. This needs to be run everytime new submodules are added.

```sh
git submodule update --recursive --init
```

2. Make sure you have Conan installed from, for example, [here](https://docs.conan.io/2/installation.html). Run the below after installation

```sh
conan profile detect --force
```

3. Make sure you have Cmake >3.27 installed

### Running

The command below will build and open the client. On start client will connect to the given ip.

```sh
./scripts/run-client.sh <server-ip>
```

## Development

Instructions to launch and run the client and server for local development:

- [The client](#client-pre)

- [The server](#server-pre)

### Client

#### <a name="client-pre"></a>Prerequisites

- (Optional) Add an environment variable called VK_LAYER_PATH and point it to vulkan-validationlayers bin/json path. (Conan library directory)

#### Running

Start the client

```sh
./scripts/run-client.sh
```

## Server & Database

Server runs on C++ and uses Boost.Asio for networking. Database is made with PostgreSQL. Server and database have been containerized using Docker.

### <a name="server-pre"></a>Prerequisites

To run the server and database, make sure you have `docker-compose` and `docker` installed. Docker-compose installation also instructs or automagically installs Docker, depending what do you choose.

- Install docker-compose [here](https://docs.docker.com/compose/install/).

### Running

Start the server and database containers

```sh
./scripts/run-backend.sh
```

If the script doesn't work, you can just use `docker-compose up` in the root directory to start up the services.

#### Running with Conan

It is possible to run the server without Docker by using `./scripts/run-backend-conan.sh`. NOTE: This does not start the database. Also not guaranteed to work on every platform.

#### Hot reload

- Docker compose version 2.22 or higher needed

- `./scripts/run-backend.sh --watch` to automatically rebuild the server everytime changes occur. NOTE: Watch is experimental docker-compose feature. In our case, this might quickly increase your docker build cache, so make sure to sometimes run `docker system prune` or other clean up commands.

### Troubleshooting

- You can debug the db contents by running `psql -h 127.0.0.1 -p 6543 -U myuser -d mmorpg-database`. Password is `mypassword`.

## Technical overview

The project is divided into three main parts: the client, the server and the database. The client is responsible for rendering the game and sending user input to the server. The server is responsible for handling the game logic and sending the game state to the client. The database is responsible for storing the game state.

![Technical overview](/docs/images/architecture_overview.png)

### Client

The client program is implemented in C++ and follows the object-oriented programming paradigm. Its task is to receive and parse messages coming from the server. The client program directs the graphics library based on these messages.

#### Graphics

The graphical backend, or graphics library, follows a class-based architecture similar to the rest of the client program, where different tasks are divided into different classes. Making the distinction between tasks is not trivial due to the complexity of the used display driver interface. The above image outlines a general architecture, the components of which will be explained in more detail in the subsections of this section. The subsections also clarify the functionality of the classes and possible actions outsourced to libraries.

### Server

The server is implemented in C++. The server's main loop advances the game 32 times per second, updating each character by one iteration each time. The server continuously receives and sends messages in a separate thread.

### Database

The database uses PostgreSQL version 16. The project does not demand much from the database as there are very few read and write operations. Additionally, data sizes are not expected to grow to several hundred, or even tens, of gigabytes. We could have chosen any general database technology for this reason. However, we ended up selecting PostgreSQL because we had previous experience with it, and the C++ ecosystem provided a simple interface library for the database (libpqx).

### API

Client-server communication is established over TCP, and all message bodies are in JSON format. RapidJSON library is used for message serialization and deserialization.

### Message Types

Messages exchanged between the client and server are identified by a unique message type and include information about which entity sends the message (server or client). Here's a summary of the available message types:

---

#### GameStateMessage

Represents the current game state. Represents all the relevant information that client needs to be able to react and interact with the world events.

Sent by: Server

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **entities**: A collection of `GameStateEntity` objects representing various entities in the game world.

  - **id**: Entity ID.
  - **instanceId**: Instance-specific ID.
  - **name**: Entity name.
  - **x**: X-coordinate.
  - **y**: Y-coordinate.
  - **z**: Z-coordinate.
  - **hp**: Health points.
  - **rotation**: Rotation angle. 0-4 (North-South)
  - **currentAction**: Current action
    - **id**: action id (For example, move or attack). -1 means no action
    - **durationMs**: duration for one action iteration
    - **looping**: boolean indicating if the action should loop or not
    - **targetId**: action target id. Intepretation depends on the action id. For example, if attack -> targetId is entityId. If move -> targetId is not defined
    - **targetCoordinate**: target coordinates. Defined if action id is move.
      - **x**: x-coordinate
      - **y**: y-coordinate
      - **z**: z-coordinate

- **objects**: A collection of `GameStateObject` objects representing objects in the game world.
  - **id**: Object ID.
  - **instanceId**: Instance-specific ID.
  - **name**: Object name.
  - **x**: X-coordinate.
  - **y**: Y-coordinate.
  - **z**: Z-coordinate.
  - **rotation**: Rotation angle. 0-4 (North-South).
- **currentPlayer**: A `CurrentPlayer` object e.g. the player that this `GameStateMessage` is being sent to. Contains some info that is only exposed to the player itself.
  - **Inherits all the entity fields** (See above)
  - **skills**: A collection of `Skill` objects.
    - **id**: Skill ID.
    - **name**: Skill name.
    - **xp**: Experience points.
  - **inventory**: A collection of `GameItem` objects.
    - **id**: Item ID.
    - **instanceId**: Instance-specific ID.
    - **name**: Item name.
    - **stackSize**: Stack size (1 for non-stackable items).

</details>

---

#### MoveMessage

Represents a move command with coordinates (x, y).

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **x**: X-coordinate for the move.
- **y**: Y-coordinate for the move.

</details>

---

#### AttackMessage

Represents an attack command targeting a specific entity by its ID.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **targetId**: ID of the entity to be attacked.

</details>

---

#### ObjectInteractMessage

Represents an interaction command with a specific game object. For example, a player wanting to interact with a tree.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **objectId**: ID of the object to interact with.

</details>

---

#### LoginMessage

Represents data needed by the server to perform identification and authentication of the player

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **username**: Username that was input by the user
- **password**: Password that was input by the user

</details>

---

#### UseItemMessage

Represents a command by the client to use an item. For example, a player wanting to eat food from their inventory

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **itemId**: Instance ID of the item to be used

</details>

---

#### DropItemMessage

Represents a command by the client to drop an item from their inventory.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **itemId**: Instance ID of the item to be dropped

</details>

---

#### PickItemMessage

Represents a command by the client to obtain an item from the ground.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **itemId**: Instance ID of the item to be picked up

</details>

---

#### UnequipMessage

Represents a command by the client to unequip an item.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **itemId**: Instance ID of the item to be unequipped

</details>

---

#### TalkMessage

A message to initiate talk to a npc. The response is given in InformativeMessage. Currently, there is no dialogue e.g. player cant say anything to the npc.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **npcId**: Npc id the player wants to talk to

</details>

---

#### InformativeMessage

Carries some information that the server wants to give to the client (Outside the gamestate context). For example, login failed npc said something.

Sent by: Server

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **message**: String of descriptive information.

</details>

---

To add new messages, check [here](/rehtiLib/network/src/api/MessageApi.hpp)

## Project practices

### General

If you wish to develop this project, please follow these basic guidelines.

- Code needs to be documented. The goal is that by reading the docs, a user can start the program and gain a technical overview of the project.

- Use Doxygen format to document every function, class and struct in hpp files. See Doxygen documentation [here](/docs/html/index.html)

- Changes are implemented through pull requests

- Change pattern: your own branch -> dev -> main

### Testing

Tests are implemented with Gtests. Tests only cover the backend.

Run the tests with `./scripts/test-server.sh`.
