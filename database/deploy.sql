CREATE TABLE player(
    id SERIAL PRIMARY KEY,
    username varchar UNIQUE NOT NULL,
    password varchar NOT NULL,
    position_x int NOT NULL,
    position_y int NOT NULL,
    hp int NOT NULL
);

CREATE TABLE item(
    id int NOT NULL,
    player_id int REFERENCES player(id) NOT NULL,
    amount int NOT NULL,
    is_equipped boolean NOT NULL,
    PRIMARY KEY(id, player_id, is_equipped)
);

CREATE INDEX on item (player_id);

CREATE TABLE skill(
    id int NOT NULL,
    player_id int REFERENCES player(id) NOT NULL,
    exp int NOT NULL,
    PRIMARY KEY(id, player_id)
);

CREATE INDEX on skill (player_id);
