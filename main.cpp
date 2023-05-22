//
// Copyright 2023 BMW AG
// SPDX-License-Identifier: MPL-2.0
//
#include <iostream>
#include <filesystem>

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
    string ip = "127.0.0.1";
    string port = "3456";

    if (argc == 1) {
        std::cerr  << "Usage: ./standalone_osi_trace_file_player path/to/the/trace/file.osi ip.address port" << std::endl;
        return 1;
    }

    if (argc == 2) {
        file_path = argv[1];
        std::cout << "Default IP and port: 127.0.0.1:3456" << std::endl;
    }

    if (argc == 3) {
        file_path = argv[1];
        ip = argv[2];
        std::cout << "Default port: 3456" << std::endl;
    }

    if (argc == 4) {
        file_path = argv[1];
        ip = argv[2];
        port = argv[3];
    }

    if (argc > 4) {
        std::cerr  << "Usage: ./standalone_osi_trace_file_player path/to/the/trace/file.osi ip.address port" << std::endl;
        return 1;
    }

    std::chrono::milliseconds start_source_calc = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());

    // Get .osi file
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

    int played_frames = 0;
    int total_length = 0;
    bool end_of_trace = false;
    while (!end_of_trace)
    {
        size_t buf_size = 0;
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
        if (size > buf_size) {
            size_t new_size = size * 2;
            if (ReallocBuffer(&message_buf, new_size) == 0) {
                std::cerr << "Failed to allocate memory." << std::endl;
                return 1;
            }
            buf_size = new_size;
        }

        size_t already_read = 0;
        while (already_read < size) {
            std::cout << "Reading frame " << played_frames << std::endl;
            fseek(trace_file, 4  * (played_frames + 1) + total_length, SEEK_SET);
            size_t res = fread(message_buf + already_read, sizeof(message_buf[0]), size - already_read, trace_file);
            if (res == 0) {
                std::cout << "Unexpected end of file." << std::endl;
                end_of_trace = true;
            }
            else
            {
                already_read += res;
                played_frames++;
                total_length += size;

                std::string message_str(message_buf, message_buf + size);
                //todo: send via TCP
            }
        }
    }

    fclose(trace_file);

    return 0;
}
