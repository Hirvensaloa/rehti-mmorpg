# REHTI MMORPG

## Get started

### Before running anything

- Make sure to fetch git submodules run `git submodule update --recursive --init`. This needs to be run everytime new submodules are added.
- Make sure you have Conan installed from (https://docs.conan.io/2/installation.html)[here]. Run `conan profile detect --force` after installation.
- Make sure you have Cmake version >= 3.26

### Running

Instructions to launch and run:

- [The client](#client-pre)
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

- Install docker-compose (https://docs.docker.com/compose/install/)[here].

### Running

- `./scripts/run-backend.sh` to start the server and database containers.

#### Running with Conan

It is possible to run the server without Docker by using `./scripts/run-backend-conan.sh`. NOTE: This does not start the database. Also not guaranteed to work on every platform.

#### Hot reload

- `./scripts/run-backend.sh --watch` to automatically rebuild the server everytime changes occur. NOTE: Watch is experimental docker-compose feature. In our case, this might quickly increase your docker build cache, so make sure to sometimes run `docker system prune` or other clean up commands.

If the script doesn't work, you can just use `docker-compose up` in the root directory to start up the services.

### Troubleshooting

- If there are problems with the libpqxx make sure that the dependencies/libpqxx exists and that it has been checked out to tag `7.7.5`. As of now the `7.8^` doesn't work properly when installed on the Alpine image.

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
