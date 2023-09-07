# REHTI MMORPG

## Get started

### Before running anything

- Make sure to fetch git submodules run `git submodule update --recursive --init`. This needs to be run everytime new submodules are added.

### Running

Instructions to launch and run:

- [The client](#client-pre)
- [The server](#server-pre)

## Client

TODO: Lyhyt desc tähän. Ja jos on jotain tärkeetä/poikkeuksellista mitä pitää tietää

### <a name="client-pre"></a>Prerequisites

TODO: Mitä vaaditaan että clienttia voi alkaa pyörittämään.

### Running

TODO: Miten clientin voi ajaa.

### Troubleshooting

TODO: Onko jotain mikä voisi mennä helposti rikki tai jotain yleisiä vinkkejä

## Server & Database

Server runs on C++ and uses Boost.Asio for networking. Database is made with PostgreSQL. Server and database have been containerized using Docker. This aims to prevent any compatibility issues and making deployment easier.

### <a name="server-pre"></a>Prerequisites

To run the server and database, make sure you have `docker-compose` and `docker` installed. Docker-compose installation also instructs or automagically install Docker, depending what do you choose.

- Install docker-compose (https://docs.docker.com/compose/install/)[here].

### Running

- `./scripts/run-backend.sh` to start the server and database containers.

#### Hot reload

- `./scripts/run-backend.sh --watch` to automatically rebuild the server everytime changes occur. NOTE: Watch is experimental docker-compose feature. In our case, this might quickly increase your docker build cache, so make sure to sometimes run `docker system prune` or other clean up commands.

If the script doesn't work, you can just use `docker-compose up` in the root directory to start up the services.

### Troubleshooting

- If there are problems with the libpqxx make sure that the dependencies/libpqxx exists and that it has been checked out to tag `7.7.5`. As of now the `7.8^` doesn't work properly when installed on the Alpine image.
- You can debug the db contents by running `psql -h 127.0.0.1 -p 6543 -U myuser -d mmorpg-database`
