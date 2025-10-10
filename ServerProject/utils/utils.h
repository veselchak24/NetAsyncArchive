/**
 * @file utils.h
 *
 * @brief Contains supporting functions
 *
 * @fn getAllFiles(const std::string& path)
 * @fn processingInput(int argc, const char** argv, char*& host, int& port, moodycamel::ConcurrentQueue<std::string>& queue)
 * @fn handleClient(const Server* server, const SOCKET& client, moodycamel::ConcurrentQueue<std::string>* socketQueue)
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

#include "../SwitchConcurrentQueue.h"
#include "../Server/Server.h"

#include <winsock2.h>

/**
 * @brief Returns a vector of paths to all files in a folder and subfolders
 *
 * @param[in] path path to folder for processing
 *
 * @throws std::invalid_argument if path is invalid
 */
std::vector<std::string> getAllFiles(const std::string& path);

/**
 * @brief Processing parameters from command line and initialization of variables
 *
 * @param[in] argc count of parameters in command line
 * @param[in] argv array of parameters in command line
 * @param[out] host address for server connection
 * @param[out] port port for server connection
 * @param[out] queue concurrent queue to store file paths
 *
 * @throws std::invalid_argument if input parameters are incorrect
 */
void processingInput(int argc, const char** argv, char*& host, int& port,
                     concurrentQueue<std::string>& queue);

/**
 * Processing client requests in a loop.
 *
 * Sending file data to client for compression.
 *
 * Receiving compressed file data from client.
 *
 * Make the compressed file .
 *
 * @param server the server object
 * @param client client socket
 * @param socketQueue concurrent queue with file paths
 * @param clientsCount number of connected clients
 *
 * @throws std::invalid_argument if parameters are incorrect
 */
void handleClient(const Server* server, const SOCKET& client,
                  concurrentQueue<std::string>* socketQueue, std::atomic<unsigned int>& clientsCount);

#endif //UTILS_H
