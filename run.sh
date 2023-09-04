#!/bin/bash

# Determine the operating system
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  # Linux
  terminal_cmd="gnome-terminal"
elif [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
  # Windows (Git Bash)
  terminal_cmd="wt"
else
  echo "Unsupported operating system: $OSTYPE"
  exit 1
fi

# Check if the "--watch" argument is provided
if [[ "$1" == "--watch" ]]; then
  # Start the Docker Compose project with build and wait, and then display logs
  docker compose up --build --wait 

  # Open a new terminal window and run docker compose alpha watch
  "$terminal_cmd" -- docker compose alpha watch

  docker-compose logs -f

  # After the user exits the logs, stop the Docker Compose project
  docker-compose stop
else
  # Start the Docker Compose project
  docker-compose up
fi
