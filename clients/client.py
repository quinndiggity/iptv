#!/usr/bin/env python3
import socket, select, struct, sys

SERVER_HOST = 'localhost'
SERVER_PORT = 6317


def print_usage():
    print("Usage:\n"
          "[required] argv[1] key\n"
          "[optional] argv[2] host\n"
          "[optional] argv[3] port\n")


cmd_array = ['0 1234 activate_request {"license_key":"%s"}\r\n',
             '0 1235 stop_service {"license_key":"%s", "delay":1}\r\n',
             '0 1236 state_service {"license_key":"%s","jobs_directory":"/home/sasha", "timeshifts_directory":"/root", "hls_directory":"/home/sasha/1", "ads_directory":"/home/sasha/2", "playlists_directory":"/home/sasha/3", "dvb_directory":"/home/sasha/4", "capture_card_directory":"/home/sasha/5"}\r\n',
             '0 1237 start_stream {"license_key":"%s", "command_line": "feedback_dir=/home/sasha/encodings/121 log_level=6","config": {"audio_bitrate": 92,"audio_codec": "faac", "delay_time": 0,"id": "encoding_121","input": {"urls": [{"id": 170,"uri": "http://i-ptv.com/wp-content/uploads/2018/04/IsPRO100IPTV.m3u.m3u"}]},"output": {"urls": [{"id": 80,"uri": "tcp://localhost:1935"}]},"type": "encoding","video_bitrate": 1700,"video_codec": "x264enc","volume": 1.0}}\r\n',
             '0 1238 stop_stream {"license_key":"%s", "id": "encoding_121"}\r\n',
             '0 1239 restart_stream {"license_key":"%s", "id": "encoding_121"}\r\n']


def isdigit(value):
    try:
        int(value)
        return True
    except ValueError:
        return False


# main function
if __name__ == "__main__":
    argc = len(sys.argv)

    if argc > 1:
        key = sys.argv[1]

    host = SERVER_HOST
    if argc > 2:
        host = sys.argv[2]

    port = SERVER_PORT
    if argc > 3:
        port = int(sys.argv[3])

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(2)

    # connect to remote host
    try:
        s.connect((host, port))
    except:
        print('Unable to connect')
        sys.exit()

    print('Connected to remote host')
    print(
        'digit command:\n0 - activate client\n1 - stop daemon\n2 - state daemon\n3 - start stream\n4 - stop stream\n5 - restart stream')
    while 1:
        socket_list = [sys.stdin, s]

        # Get the list sockets which are readable
        read_sockets, write_sockets, error_sockets = select.select(socket_list, [], [])

        for sock in read_sockets:
            # incoming message from remote server
            if sock == s:
                data = sock.recv(4096)
                if not data:
                    print('Connection closed')
                    sys.exit()
                else:
                    # print data
                    var = data[4:]
                    sys.stdout.write(var.decode())

            # user entered a message
            else:
                msg = sys.stdin.readline()
                data = msg.encode()
                if not isdigit(data):
                    continue

                cmd_template = cmd_array[int(data)]
                data = cmd_template % key

                data_len = socket.ntohl(len(data))
                array = struct.pack("I", data_len)
                data_to_send_bytes = array + data.encode()
                s.send(data_to_send_bytes)
