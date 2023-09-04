# REHTI MMORPG

## Server & Database

Server and database have been containerized using Docker. This aims to prevent any compatibility issues and making deployment easier.

### Prerequisites

To run the server and database, make sure you have `docker-compose` and `docker` installed. Docker-compose installation also instructs or automagically install Docker, depending what do you choose.

- Install docker-compose (https://docs.docker.com/compose/install/)[here].

### Running

- `./run.sh` to start the server and database containers. Use `--watch` flag to automatically rebuild the server everytime changes occur.

If the script doesn't work, you can just use `docker-compose up` in the root directory to start up the services.
