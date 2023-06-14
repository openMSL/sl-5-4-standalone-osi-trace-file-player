//
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
//
#include <iostream>
#include <filesystem>
#include <zmq.hpp>
#include <chrono>
#include <thread>

using namespace std;

int ReallocBuffer(char **message_buf, size_t new_size) {
    char *new_ptr = *message_buf;
    new_ptr = (char *)realloc(new_ptr, new_size);
    if (new_ptr == nullptr) {
        return 0;
    }
    *message_buf = new_ptr;
    return 1;
}

int main(int argc, char *argv[])
{

    string file_path;
    uint step_size_ms;
    string ip = "127.0.0.1";
    string port = "3456";

    if (argc == 1 || argc == 2 || argc > 5) {
        std::cerr  << "Usage: ./standalone_osi_trace_file_player path/to/the/trace/file.osi step_size_in_ms ip.address port" << std::endl;
        return 1;
    }

    if (argc == 3) {
        file_path = argv[1];
        step_size_ms = stoi(argv[2]);
        std::cout << "Default IP and port: 127.0.0.1:3456" << std::endl;
    }

    if (argc == 4) {
        file_path = argv[1];
        step_size_ms = stoi(argv[2]);
        ip = argv[3];
        std::cout << "Default port: 3456" << std::endl;
    }

    if (argc == 5) {
        file_path = argv[1];
        step_size_ms = stoi(argv[2]);
        ip = argv[3];
        port = argv[4];
    }

    // Get .osi file
    std::cout << "Reading trace file: " << file_path << std::endl;
    if (!std::filesystem::exists(file_path))
    {
        perror("Trace file not found");
        return 1;
    }

    FILE *trace_file = fopen(file_path.c_str(), "rb");
    if (trace_file == nullptr) {
        perror("Open failed");
        return 1;
    }

    // Init TCP socket
    string address = "tcp://" + ip + ":" + port;
    std::cout << "Init TCP socket: " << address << std::endl;
    const char* protocol = address.c_str();
    zmq::context_t context;
    zmq::socket_t socket = zmq::socket_t(context, ZMQ_PUSH);
    socket.bind(protocol);

    uint played_frames = 0;
    uint total_length = 0;
    bool end_of_trace = false;
    std::cout << "Send messages with step size: " << step_size_ms << " ms" << std::endl;
    while (!end_of_trace)
    {
        std::chrono::milliseconds start_read_trace = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
        char *message_buf = nullptr;
        fseek(trace_file, 4 * played_frames + total_length, SEEK_SET);
        uint size = 0;
        size_t ret = fread(&size, sizeof(uint), 1, trace_file);
        if (ret == 0) {
            std::cout << "End of trace" << std::endl;
            end_of_trace = true;
        } else if (ret != 1) {
            std::cerr << "Failed to read the size of the message." << std::endl;
            return 1;
        }
        if (size > 0) {
            size_t new_size = size * 2;
            if (ReallocBuffer(&message_buf, new_size) == 0) {
                std::cerr << "Failed to allocate memory." << std::endl;
                return 1;
            }
        }

        size_t already_read = 0;
        while (already_read < size) {
            std::cout << "Sending frame " << played_frames << std::endl;
            fseek(trace_file, 4  * (played_frames + 1) + total_length, SEEK_SET);
            size_t res = fread(message_buf + already_read, sizeof(message_buf[0]), size - already_read, trace_file);
            if (res == 0) {
                std::cout << "Unexpected end of file." << std::endl;
                end_of_trace = true;
            }
            else
            {
                already_read += res;
            }
        }
        if(end_of_trace)
            break ;
        zmq::message_t send_message(message_buf, size, nullptr);
        socket.send(send_message, zmq::send_flags::none);

        std::chrono::milliseconds stop_read_trace = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
        std::chrono::milliseconds delta_time_ms = std::chrono::milliseconds(step_size_ms) - (stop_read_trace - start_read_trace);
        if (delta_time_ms.count() > 0)
        {
            std::this_thread::sleep_for(delta_time_ms);
        }

        played_frames++;
        total_length += size;
    }

    fclose(trace_file);

    return 0;
}
