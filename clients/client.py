#!/usr/bin/env python3
import socket, select, struct, sys

SERVER_HOST = 'localhost'
SERVER_PORT = 6317


def print_usage():
    print("Usage:\n"
          "[required] argv[1] key\n"
          "[required] argv[2] input uri"
          "[optional] argv[3] host\n"
          "[optional] argv[4] port\n")


cmd_array = ['0 1234 activate_request {"license_key":"%s"}\r\n',
             '0 1235 stop_service {"license_key":"%s", "delay":1}\r\n',
             '0 1236 state_service {"license_key":"%s","jobs_directory":"/home/sasha", "timeshifts_directory":"/root", "hls_directory":"/home/sasha/1", "ads_directory":"/home/sasha/2", "playlists_directory":"/home/sasha/3", "dvb_directory":"/home/sasha/4", "capture_card_directory":"/home/sasha/5"}\r\n',
             '0 1237 start_stream {"license_key":"%s", "command_line": "feedback_dir=~/test/1 log_level=6","config": {"id": "test_1","audio_bitrate": 92,"audio_codec": "faac", "delay_time": 0,"input": {"urls": [{"id": 170,"uri": "%s"}]},"output": {"urls": [{"id": 80,"uri": "tcp://localhost:1935"}]},"type": "encoding","video_bitrate": 1700,"video_codec": "x264enc","volume": 1.0}}\r\n',
             '0 1238 start_stream {"license_key":"%s", "command_line": "feedback_dir=~/test/2 log_level=6","config": {"id": "test_1","input": {"urls": [{"id": 1,"uri": "%s"}]},"timeshift_dir": "/var/www/html/live/14","type": "timeshift_recorder"}}\r\n',
             '0 1239 stop_stream {"license_key":"%s", "id": "test_1"}\r\n',
             '0 1240 restart_stream {"license_key":"%s", "id": "test_1"}\r\n']


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
    else:
        print_usage()
        sys.exit(1)

    if argc > 2:
        input_uri = sys.argv[2]
    else:
        print_usage()
        sys.exit(1)

    host = SERVER_HOST
    if argc > 3:
        host = sys.argv[3]

    port = SERVER_PORT
    if argc > 4:
        port = int(sys.argv[4])

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(2)

    # connect to remote host
    try:
        s.connect((host, port))
    except:
        print('Unable to connect')
        sys.exit()

    print('Connected to remote host')
    print('digit commands:\n'
          '0 - activate client\n'
          '1 - stop daemon\n'
          '2 - state daemon\n'
          '3 - start stream\n'
          '4 - start stream time\n'
          '5 - stop stream\n'
          '6 - restart stream\n\n'
          'text commands:\n'
          'quit - exit from client')

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
                    sys.exit(0)
                else:
                    # print data
                    var = data[4:]
                    sys.stdout.write(var.decode())

            # user entered a message
            else:
                msg = sys.stdin.readline()
                data = msg.encode()
                if not isdigit(data):
                    if msg == 'quit\n':
                        s.close()
                        print('Bye')
                        sys.exit(0)
                    continue

                cmd_number = int(data)
                cmd_template = cmd_array[cmd_number]
                if cmd_number == 3 or cmd_number == 4:
                    data = cmd_template % (key, input_uri)
                else:
                    data = cmd_template % key

                data_len = socket.ntohl(len(data))
                array = struct.pack("I", data_len)
                data_to_send_bytes = array + data.encode()
                s.send(data_to_send_bytes)
