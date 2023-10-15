# Map

Map consists of areas. Each area has 128x128 tiles.

Tiles cannot be divided further. Tiles map 1 to 1 with the coordinate system.

## Creating an example map

1. **Define the areas in to the [map.json](/assets/map/map.json) file**

In this example we have four areas, Eldertree Grove (EG) and Crystalpeak Citadel (CC).

Adding to json:

```
[
  ["EG1", "CC1"],
  ["EG2", "CC2"]
]
```

Note: Row sizes don't need to match. For example, this is valid:

```
[
  ["EG1", "CC1"],
  ["EG2"],
  ["EG2", "EG2", "EG2"]
]
```

2. **Create the area drawings**

The drawings shows where different type of textures are and what is the map height in certain coordinates.
We use the RGB-channels to store the information.

|   Field    | Value |          Notes           |
| :--------: | :---: | :----------------------: |
| Texture id |   R   |            -             |
|   Height   | G x B | First bit of G is a sign |

R-channel stores the texture id. G and B store the height information.
The height value is formed by multiplying G with B, where the first bit of G indicates the sign.

The drawings should be 128x128 and use the PNG-format. They should be under `<root>/assets/map/areas`. Name the drawings `<area name in json>.png`, in our case `EG1.png`, `EG2.png`, `CC1.png` and `CC2.png`.

3. **Create the object drawings**

For each area, there needs to be another drawing that shows where all the objects are and what are their rotations. Again we use RGB-channels.

|   Field   | Value |         Notes         |
| :-------: | :---: | :-------------------: |
| Object id | R x G | 255 x 255 = no object |
| Rotation  |   B   |  B > 3 = no rotation  |

R and G together indicate the object id, R x G, R x G = 65025, means no object (Basically color white). B indicates the rotation. Values 0-3 are reserved for that. 0 = North, 1 = East, 2 = South, 3 = West. If value > 3, default to 0 (== no rotation). Object tile maps default rotation is north.

The drawings should be 128x128 and use the PNG-format. Name the drawings `<area name in json>-obj.png`, in our case `EG1-obj.png`, `EG2-obj.png`, `CC1-obj.png` and `CC2-obj.png`.

4. **Map is done**

Now just run the `generate-map.sh` under the scripts folder to generate map assets.

## Object tile maps

Each object should have a JSON-file in [object_tile_maps](/object_tile_maps). Object tile map defines which tiles does the object block around itself. For example, a house wants to define all of the tiles which have walls, as blocked. Object's can also block a tile from a certain direction. For example, a fence can block only south direction so that player can still be on the same tile as the fence but the fence will just block southside access.

Object tile map is named `<id>-<name>.json`. Object tile map is defined as a matrix. Elements have string values.

| String value |                            Meaning                            |
| :----------: | :-----------------------------------------------------------: |
|      S       |                      Blocked from south                       |
|      W       |                       Blocked from west                       |
|      N       |                      Blocked from north                       |
|      E       |                       Blocked from east                       |
|      B       | Blocked from each side, also can be expressed with "B" "SWNE" |
|      X       |                  Objects center, not blocked                  |
|     " "      |                        no-block at all                        |
|      XB      |                Objects center + it is blocked                 |
|     XNW      |    Objects center + it is blocked from the north and west     |

The center ("X") needs to be defined because in the object drawing the object is placed according to it's center.

For example a house object tile map `255-house.json`:

```
[
  ["B", "B", "B", "B", "B", "B", "B"],
  ["B", " ", " ", " ", " ", " ", "B"],
  ["B", " ", " ", "X", " ", " ", "B"],
  ["B", " ", " ", " ", " ", " ", "B"],
  ["B", " ", " ", " ", " ", " ", "B"],
  ["B", " ", "B", "B", "B", "B", "B"]
]
```

Another example with a fence object tile map `30-fence.json`:

```
[
  ["S", "XS", "S"]
]
```
