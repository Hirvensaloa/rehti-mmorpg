# Assets

All the game assets are defined here with instructions on how to add and expand on existing assets. Assets include things such as items, skills and the world map.

In summary, all the game assets can be defined in a JSON format. The goal is to make managing assets as flexible and scalable as possible. Also, managing assets shouldn't require extensive understanding of the code base.

- [Item Types and Attributes](#item-types-and-attributes)
- [Game Objects](#game-objects)
- [Skills](#skills)
- [Map](#map)

## Item Types and Attributes

The game has various type of items which the player can carry in the inventory and possibly equip. Items can also be obtained from NPCs or loot objects (treasure chests etc.). All items are defined [here](/assets/items.json)

**General Item**

General item which the player cannot interact with. The player can only carry such items in the inventory. Defined with:

- **id** Unique identifier for the item.
- **type** "General"
- **name** Name of the item.
- **description** Brief description of the item.
- **isStackable** true if the item can be stacked; otherwise, false.
- **textureFilename** Filename of the texture file under the [textures](/assets/textures/)-folder.
- **objFilename** Filename of the texture file under the [objects](/assets/objects/)-folder.
- **iconFilename** Filename of a icon picture under the [items](/assets/items/)-folder.

**Equippable Item**

Item that the player can equip, giving the player possible stat boosts. Defined with:

- **id** Unique identifier for the item.
- **type** "Equippable"
- **name** Name of the item.
- **description** Brief description of the item.
- **slot** Equippable slot (Choose from: MainHand, OffHand, Head, Top, Bottom, Boots, Gloves, Neck, Ring).
- **stats** ItemStats object. The ItemStats object includes the following attributes:
  - **accuracy** Represents the accuracy bonus of the equippable item.
  - **damage** Indicates the damage dealt by the item.
  - **attackSpeed** Specifies the attack speed of the item. Defined in ms.
  - **range** Denotes the range of the item's attacks in tiles.
  - **armor** Represents the armor provided by the equippable item.
  - **dodge** Indicates the dodge bonus provided by the item.
- **isStackable** true if the item can be stacked; otherwise, false.
- **textureFilename** Filename of the texture file under the [textures](/assets/textures/)-folder.
- **objFilename** Filename of the texture file under the [objects](/assets/objects/)-folder.
- **iconFilename** Filename of a icon picture under the [items](/assets/items/)-folder.

**Food Item**

Item that the player can consume (eat) to get more health. Defined with:

- **id** Unique identifier for the item.
- **type** "Food"
- **name** Name of the food item.
- **description** Brief description of the item.
- **healAmount** Amount of healing provided by the food.
- **isStackable** true if the item can be stacked; otherwise, false.
- **textureFilename** Filename of the texture file under the [textures](/assets/textures/)-folder.
- **objFilename** Filename of the texture file under the [objects](/assets/objects/)-folder.
- **iconFilename** Filename of a icon picture under the [items](/assets/items/)-folder.

### Adding New Items to the Item List

You can add new items [here](/assets/items.json). Best practice is to add the item to the end of the list and increment the id from the last item.

Example:

```javascript
{
  "id": 12, // Increment the ID from the last item
  "name": "New Item Name",
  "description": "A description of the new item.",
  "type": "Equippable", // Choose from General, Equippable, or Food
  "slot": "NewSlot", // For Equippable items, specify the slot
  "stats": {
    // Define item stats as needed
    "accuracy": 10,
    "damage": 20
    // ...
  },
  "isStackable": false,
  "textureFilename": "cool_texture.png",
  "objFile": "nice_object.obj",
  "iconFilename": "icon.png"
}
```

## Game objects

Objects can be categorized into three main types: **General**, **Resource** and **Loot**. Each type has specific attributes and serves different purposes. All the objects are defined [here](/assets/objects.json)

**General Objects**

General objects are basic objects with minimal attributes. They have the following attributes:

- **id:** A unique identifier for the object.
- **type:** "General" to indicate the object type.
- **name:** The name or description of the object.
- **description:** A brief description of the object.
- **tileMap:** A tile map representing the object's visual representation.
- **textureFilename** filename of the corresponding texture file under the [textures](/assets/textures/)-folder.
- **objFilename** filename to the `.obj` under the [objects](/assets/objects/)-folder.

Example general object:

```json
{
  "id": 30,
  "type": "General",
  "name": "Fence",
  "description": "A sturdy fence.",
  "tileMap": [["S", "XS", "S"]],
  "textureFilename": "fence.jpeg",
  "objFilename": "fence.obj"
}
```

**Resource Objects**

Resource objects represent in-game resources that players can interact with, such as trees for woodcutting, ore deposits for mining, and more. They have the following attributes:

- **id:** A unique identifier for the object.
- **type:** "Resource" to indicate the object type.
- **name:** The name or description of the resource.
- **yieldableItemList:** A list of items that can be yielded from this resource, along with the percentage chance to yield each item.
- **xpPerYield:** The amount of experience points (XP) gained when yielding once from this resource.
- **depleteChance:** The chance of depleting the resource when yielded.
- **relatedSkillId:** The skill related to this resource (e.g., 0 (Woodcutting) for a tree).
- **xpRequirement:** The required XP in the related skill to yield from this resource.
- **description:** A brief description of the resource.
- **tileMap:** A tile map representing the resource's visual representation.
- **textureFilename** filename of the corresponding texture file under the [textures](/assets/textures/)-folder.
- **objFilename** filename to the `.obj` under the [objects](/assets/objects/)-folder.

Example resource object:

```json
{
  "id": 1,
  "type": "Resource",
  "name": "Tree",
  "yieldableItemList": [
    {
      "itemId": 0,
      "yieldPercentage": 100
    }
  ],
  "xpPerYield": 10,
  "depleteChance": 20,
  "relatedSkillId": 0,
  "xpRequirement": 0,
  "description": "A tree. It is a tree.",
  "tileMap": [["XB"]],
  "textureFilename": "tree.png",
  "objFilename": "tree.obj"
}
```

**Loot Objects**

Loot objects represent items that can be looted, such as treasure chests or hidden containers. They have the following attributes:

- **id:** A unique identifier for the object.
- **type:** "Loot" to indicate the object type.
- **name:** The name or description of the loot.
- **yieldableItemList:** A list of items that can be looted from this object, along with the percentage chance to loot each item.
- **description:** A brief description of the loot.
- **tileMap:** A tile map representing the loot's visual representation.
- **textureFilename** filename of the corresponding texture file under the [textures](/assets/textures/)-folder.
- **objFilename** filename to the `.obj` under the [objects](/assets/objects/)-folder.

Example loot object:

```json
{
  "id": 500,
  "type": "Loot",
  "name": "Treasure Chest",
  "yieldableItemList": [
    {
      "itemId": 1,
      "yieldPercentage": 30
    },
    {
      "itemId": 2,
      "yieldPercentage": 70
    }
  ],
  "description": "Contains a treasure (If you consider a dirty, rotten shield a treasure)",
  "tileMap": [["XB"]],
  "textureFilename": "chest.png",
  "objFilename": "lootchest.obj"
}
```

### Adding objects to the game

You can add objects to the game with the above definitions. Add the object to the [here](/assets/objects.json) and make sure the ID's do not collide.

NOTE: The provided item IDs should range from 0 to 255 x 255 - 1 (255x255 is non-object id). Best practice is to spread out the ID's as when you draw them in the map definition, the ID's are easier to distinct.

### Tile map

Each object should have a tile map. Object tile map defines which tiles does the object block around itself. For example, a house wants to define all of the tiles which have walls, as blocked. Object's can also block a tile from a certain direction. For example, a fence can block only south direction so that player can still be on the same tile as the fence but the fence will just block southside access.

Object tile map is defined as a matrix. Elements have string values.

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

```json
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

```json
[["S", "XS", "S"]]
```

## Skills

In the game players have skills which allow for extracting resources and fighting monsters. Skills can be trained by doing related activities, training yields xp. Higher xp in a skill allows extracting more and/or better resources and the player will get stronger. All the skills are defined [here](/assets/skills.json).

Skills have the following attributes:

- **id** Unique identifier
- **name** Name of the skill
- **description** A brief description

### Adding a new skill

You can add new skills to the game by following the above definition. Add the new skill [here](/assets/skills.json). Best practice is to add the skill to the end of the list and increment the id from the last skill.

Example:

```json
{
  "id": 4,
  "name": "Blacksmithing",
  "description": "Mastery of crafting weapons and armor."
}
```

## Map

Map consists of areas. Each area has 128x128 tiles.

Tiles cannot be divided further. Tiles map 1 to 1 with the coordinate system.

### Creating an example map

1. **Define the areas in to the [map.json](/assets/map/map.json) file**

In this example we have four areas, Eldertree Grove (EG) and Crystalpeak Citadel (CC).

Adding to json:

```json
[
  ["EG1", "CC1"],
  ["EG2", "CC2"]
]
```

Note: Row sizes don't need to match. For example, this is valid:

```json
[["EG1", "CC1"], ["EG2"], ["EG2", "EG2", "EG2"]]
```

2. **Create the area drawings**

The drawings show where different types of textures are and what is the map height in certain coordinates.
We use the RGB-channels to store the information.

|   Field    | Value |          Notes           |
| :--------: | :---: | :----------------------: |
| Texture id |   R   |            -             |
|   Height   | G x B | First bit of G is a sign |

R-channel stores the texture id. G and B store the height information.
The height value is formed by multiplying G with B, where the first bit of G indicates the sign.

The drawings should be 128x128 and use the PNG-format. They should be under `<root>/assets/map/areas`. Name the drawings `<area name in json>.png`, in our case `EG1.png`, `EG2.png`, `CC1.png` and `CC2.png`.

3. **Add map textures**

- Add texture files for each texture id [here](/assets/textures/)
- Map the id to the texture filename here [here](/assets/textures/map_textures.json).

4. **Create the object drawings**

For each area, there needs to be another drawing that shows where all the objects are and what their rotations are. Again we use RGB-channels.

|   Field   | Value |         Notes         |
| :-------: | :---: | :-------------------: |
| Object id | R x G | 255 x 255 = no object |
| Rotation  |   B   |  B > 3 = no rotation  |

R and G together indicate the object id, R x G, R x G = 65025, means no object (Basically color white). B indicates the rotation. Values 0-3 are reserved for that. 0 = North, 1 = East, 2 = South, 3 = West. If value > 3, default to 0 (== no rotation). Object tile maps default rotation is north.

The drawings should be 128x128 and use the PNG-format. Name the drawings `<area name in json>-obj.png`, in our case `EG1-obj.png`, `EG2-obj.png`, `CC1-obj.png` and `CC2-obj.png`.

5. **Map is done**

Now just run the `generate-assets.sh` under the scripts folder to generate map assets.
