# REHTI MMORPG

## Get started

### Before running anything

- Make sure to fetch git submodules run `git submodule update --recursive --init`. This needs to be run everytime new submodules are added.

- Make sure you have Conan installed from (https://docs.conan.io/2/installation.html)[here]. Run `conan profile detect --force` after installation.


### Running

Instructions to launch and run:

- [The client](#client-pre)
Add an environment variable called VK_LAYER_PATH and point it to vulkan-validationlayers bin/json path. (Conan library directory)

- [The server](#server-pre)

## Client

TODO: Lyhyt desc tähän. Ja jos on jotain tärkeetä/poikkeuksellista mitä pitää tietää

### <a name="client-pre"></a>Prerequisites

TODO: Mitä vaaditaan että clienttia voi alkaa pyörittämään.

### Running

- `./scripts/run-client.sh` to start the client

### Troubleshooting

TODO: Onko jotain mikä voisi mennä helposti rikki tai jotain yleisiä vinkkejä

## Server & Database

Server runs on C++ and uses Boost.Asio for networking. Database is made with PostgreSQL. Server and database have been containerized using Docker. This aims to prevent any compatibility issues and making deployment easier.

### <a name="server-pre"></a>Prerequisites

To run the server and database, make sure you have `docker-compose` and `docker` installed. Docker-compose installation also instructs or automagically installs Docker, depending what do you choose.

- Install docker-compose [here](https://docs.docker.com/compose/install/).

### Running

- `./scripts/run-backend.sh` to start the server and database containers.

If the script doesn't work, you can just use `docker-compose up` in the root directory to start up the services.

#### Running with Conan

It is possible to run the server without Docker by using `./scripts/run-backend-conan.sh`. NOTE: This does not start the database. Also not guaranteed to work on every platform.

#### Hot reload

- Docker compose version 2.22 or higher needed

- `./scripts/run-backend.sh --watch` to automatically rebuild the server everytime changes occur. NOTE: Watch is experimental docker-compose feature. In our case, this might quickly increase your docker build cache, so make sure to sometimes run `docker system prune` or other clean up commands.

### Troubleshooting

- You can debug the db contents by running `psql -h 127.0.0.1 -p 6543 -U myuser -d mmorpg-database`

## Project practices

### General

- Code needs to be documented. The goal is that by reading the docs, a user can start the program and gain a technical overview of the project.

- Changes are implemented through pull requests

- Change pattern: your own branch -> dev -> main

- TODO: What else?

### C++ specific

- TODO: what?

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
