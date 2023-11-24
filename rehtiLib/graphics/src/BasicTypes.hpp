#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <vector>

#pragma region Constants

constexpr uint32_t BONES_PER_VERTEX = 4;
constexpr uint32_t MAX_BONES = 50;

constexpr glm::vec3 GAMEOBJECT_MIN = glm::vec3(-0.5f, -0.5f, -0.5f);
constexpr glm::vec3 GAMEOBJECT_MAX = glm::vec3(0.5f, 0.5f, 0.5f);

constexpr size_t OBJECT_TYPE_COUNT = 4;
constexpr size_t ANIMATION_TYPE_COUNT = 5;

enum ObjectType : uint32_t
{
    CHARACTER,
    GAMEOBJECT,
    TESTOBJECT,
    AREA,
    UNDEFINED
};

enum AnimationType : uint32_t
{
    IDLE,
    WALK,
    RUN,
    ATTACK,
    DEATH,
};

std::array<ObjectType, OBJECT_TYPE_COUNT> getObjectTypes();

std::array<AnimationType, ANIMATION_TYPE_COUNT> getAnimationTypes();

#pragma endregion

#pragma region AnimationTypes
// Animation node. The data stored can also represent any kind of transformation.
struct GfxOrientation
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::mat4 getTransformationMatrix() const;
    static GfxOrientation interpolate(GfxOrientation first, GfxOrientation second, float factor);
};

struct AnimationNode
{
    double time; // time of this animation node in ticks
    std::array<GfxOrientation, MAX_BONES> bones;
};

// Immutable data. Animations should be stored somewhere and requested when needed to be stored for a character.
struct Animation
{
    double totalTicks;                         // total ticks in the animation
    double ticksPerSecond;                     // ticks per second
    float duration;                            // duration of the animation in seconds
    std::vector<AnimationNode> animationNodes; // animation nodes
};

struct CharacterAnimationData
{
    AnimationType currentAnimation;
    double currentTicks;
    std::array<Animation, ANIMATION_TYPE_COUNT> animations;
};

struct BoneNode
{
    int parent;                     // index of the parent in bone array.
    std::vector<uint32_t> children; // indices of the children in bone array.
};

struct CharacterData
{
    GfxOrientation characterOrientation;                    // orientation of the character
    std::array<glm::mat4, MAX_BONES> boneTransformations{}; // bone transformation storage data
    std::vector<BoneNode> bones;
    CharacterAnimationData animationData;
    void advanceAnimation(float dt);
};

#pragma endregion

#pragma region GraphicsObjectTypes

typedef AnimationNode ObjOrientation;

struct Hit
{
    int id;
    ObjectType objectType;
    glm::vec3 hitPoint;
};

struct ImageData
{
    unsigned char* pixels;
    int width;
    int height;
};

struct CharacterVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::uvec4 boneIDs;
    glm::vec4 boneWeights;
    glm::vec2 texCoord;
};

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

// TODO Use assimp and checkout the library.
struct SimpleVertex
{
    glm::vec3 pos;
    glm::vec3 color;
};