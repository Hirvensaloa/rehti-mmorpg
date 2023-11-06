#pragma once

#include <boost/asio/awaitable.hpp>

#include <network.h>
#include "graphics.h"
#include "RehtiReader.hpp"
#include "Utils.hpp"

class Client
{
public:
  Client(std::string ip, std::string port);

  /**
   * @brief Connects to the server
   *
   * @return true if connection was successful. False otherwise.
   */
  boost::asio::awaitable<bool> connect();

  boost::asio::awaitable<void> randomWalk();

  /**
   * @brief Sends an attack message to the server
   *
   * @param targetId Id of the target entity
   */
  boost::asio::awaitable<void> attack(const int &targetId);

  /**
   * @brief Sends a move message to the server
   *
   * @param x coordinate to move to
   * @param y coordinate to move to
   */
  boost::asio::awaitable<void> move(const int &x, const int &y);

  /**
   * @brief Sends an object interaction message to the server.
   *
   * @param objectId the id of the object to interact with
   */
  boost::asio::awaitable<void> interactWithObject(const int &objectId);

  void start();

  void test();

  void processMessages();

  /**
   * @brief Starts the graphics library
   */
  void startGraphics();

  /**
   * @brief Handles a mouse click hit on the graphics window
   * @param hit contains hit information. See Hit.
   */
  void handleMouseClick(const Hit &hit);

private:
  boost::asio::io_context ioContextM;
  boost::asio::ip::tcp::resolver resolverM;
  boost::asio::ip::tcp::resolver::results_type endpointsM;
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuardM;

  MessageQueue messagesM;
  std::unique_ptr<Connection> connectionM;

  std::thread graphicsThreadM;
  std::thread connectionThreadM;
  std::thread ioThreadM;

  RehtiGraphics *graphLib;
  std::map<std::string, GameObjectGraphicData> gameObjectsObjDataM; ///< Contains all the game object types and their corresponding vertices and faces
  std::map<std::string, ImageData> textureDataM;         ///< Contains all the texture data 

  Hit lastHitM; ///< Last detected hit from a mouse click
};