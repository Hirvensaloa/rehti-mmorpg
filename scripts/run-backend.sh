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
  docker compose up --build --wait --remove-orphans

  # Open a new terminal window and run docker compose alpha watch
  if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    # Windows (Git Bash)
    start "$terminal_cmd" bash -c "docker compose alpha watch"
  else
    # Non-Git Bash on Windows or Linux
    "$terminal_cmd" -- bash -c "docker compose alpha watch"
  fi

  docker-compose logs -f

  # After the user exits the logs, stop the Docker Compose project
  docker-compose stop
else
  # Start the Docker Compose project
  docker-compose up --build --remove-orphans
fi
