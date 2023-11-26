# REHTI MMORPG

## Get started

### Before running the programs

1. Make sure to fetch git submodules. This needs to be run everytime new submodules are added.

```sh
git submodule update --recursive --init
```

2. Make sure you have Conan installed from, for example, [here](https://docs.conan.io/2/installation.html). Run the below after installation

```sh
conan profile detect --force
```

3. Make sure you have Cmake >3.27 installed

4. Generate the assets (map, items and objects).

```sh
./scripts/generate_assets.sh
```

### Running

Instructions to launch and run:

- [The client](#client-pre)

- [The server](#server-pre)

## Client

TODO: Lyhyt desc tähän. Ja jos on jotain tärkeetä/poikkeuksellista mitä pitää tietää

### <a name="client-pre"></a>Prerequisites

- (Optional) Add an environment variable called VK_LAYER_PATH and point it to vulkan-validationlayers bin/json path. (Conan library directory)

### Running

Start the client

```sh
./scripts/run-client.sh
```

### Troubleshooting

TODO: Onko jotain mikä voisi mennä helposti rikki tai jotain yleisiä vinkkejä

## Server & Database

Server runs on C++ and uses Boost.Asio for networking. Database is made with PostgreSQL. Server and database have been containerized using Docker. This aims to prevent any compatibility issues and making deployment easier.

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

## Client-Server Communication (API)

Client-server communication is established over TCP, and all message bodies are in JSON format. RapidJSON library is used for message serialization and deserialization.

### Message Types

Messages exchanged between the client and server are identified by a unique message type and include information about which entity sends the message (server or client). Here's a summary of the available message types:

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
  - **currentActionType**: Current action type. Equals to message id's that represent actions. For example, Attack and Move.

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

#### MoveMessage

Represents a move command with coordinates (x, y).

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **x**: X-coordinate for the move.
- **y**: Y-coordinate for the move.

</details>

#### AttackMessage

Represents an attack command targeting a specific entity by its ID.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **targetId**: ID of the entity to be attacked.

</details>

#### ObjectInteractMessage

Represents an interaction command with a specific game object. For example, a player wanting to interact with a tree.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **objectId**: ID of the object to interact with.

</details>

#### LoginMessage

Represents data needed by the server to perform identification and authentication of the player

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **username**: Username that was input by the user
- **password**: Password that was input by the user

</details>

#### UseItemMessage

Represents a command by the client to use an item. For example, a player wanting to eat food from their inventory

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **itemId**: Instance ID of the item to be used

</details>

#### UnequipMessage

Represents a command by the client to unequip an item.

Sent by: Client

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **itemId**: Instance ID of the item to be used

</details>

To add new messages, check [here](/rehtiLib/network/src/api/MessageApi.hpp)

#### InformativeMessage

Carries some information that the server wants to give to the client (Outside the gamestate context). For example, login failed.

Sent by: Server

<details>
<summary>Expand to see message attributes</summary>

- **id**: Identifies the message type.
- **message**: String of descriptive information.

</details>

## Project practices

### Testing

Tests are implemented with Gtests. Tests only cover the backend.

Run the tests with `./scripts/test-server.sh`.

### General

- Code needs to be documented. The goal is that by reading the docs, a user can start the program and gain a technical overview of the project.

- Use Doxygen format to document every function, class and struct in hpp files. See Doxygen documentation [here](/docs/html/index.html)

- Changes are implemented through pull requests

- Change pattern: your own branch -> dev -> main

## Projektin seuranta (Viedään tää osio lopuks jonnekki muualle)

### Viikko 1

Projektin pystytys ja teknologioihin tutustuminen

#### Aleksi

- Palvelimen alustus 20h
- Kehitysympäristön pystytys 10h
- Suunnittelu 5h
- Kirjastoihin tutustuminen 5h

Yhteensä: 40h

#### Samu

#### Otso

### Viikko 2

Kirjastojen tunkkausta

#### Aleksi

- Palvelimen alustus 10h
- Tietokanta 5h
- Kirjastojen setuppaus 10h

Yhteensä: 25h

#### Samu

#### Otso

### Viikko 3

#### Aleksi

- Kirjastojen asentamista 5h
- Nettikoodi 5h
- Tekninen suunnittelu 5h
- Tietokanta 5h

Yhteensä: 20h

#### Samu

#### Otso

### Viikko 4

#### Aleksi

- Projektin suunnittelu 10h
- Nettikoodin toteutus coroutiineilla 15h

Yhteensä: 25h

#### Samu

#### Otso

### Viikko 5

#### Aleksi

- Projektin suunnittelu 15h

Yhteensä: 15h

#### Samu

#### Otso

### Viikko 6

#### Aleksi

- Projektin suunnittelu 10h
- Map loader toteutus 15h
- Dokumentointi 5h

Yhteensä: 30h

#### Samu

#### Otso
