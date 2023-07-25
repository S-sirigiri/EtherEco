import socket
import struct
import re
import threading

class SendReceive:
    def __init__(self, server_ip, port):
        # Create socket
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error as e:
            print("Could not create socket")
            raise Exception("CreateSocketException") from e

        # Set up server details
        self.server = (server_ip, port)

        # Connect to remote server
        try:
            self.sock.connect(self.server)
        except socket.error as e:
            print("Connect failed")
            raise Exception("ServerConnectionException") from e

        print("Connected to server")

    def __del__(self):
        self.sock.close()

    def receive_data_once(self, buflen=1024):
        buffer = bytearray(buflen)
        try:
            received = self.sock.recv_into(buffer, buflen - 1)
        except socket.error as e:
            print("Failed to receive or connection closed by server")
            raise Exception("ConnectionClosedException") from e

        if received <= 0:
            print("Failed to receive or connection closed by server")
            raise Exception("ConnectionClosedException")

        data = buffer[:received].decode('utf-8')  # Assuming the data is UTF-8 encoded
        return data

    def send_data_once(self, data):
        try:
            sent_bytes = self.sock.send(data.encode('utf-8'))  # Assuming the data is UTF-8 encoded
        except socket.error as e:
            print("Failed to send the message")
            raise Exception("SendFailedException") from e

        return sent_bytes


class ClientTransaction(SendReceive):
    def __init__(self, server_ip, port, username, password, rcpt_username):
        super().__init__(server_ip, port)

        self.username = username
        self.rcpt_username = rcpt_username
        self.password = password

    def receive_data(self, buflen=1024):
        while True:
            buffer = bytearray(buflen)
            try:
                received = self.sock.recv_into(buffer, buflen - 1)
            except socket.error as e:
                print("Failed to receive or connection closed by server")
                raise Exception("ConnectionClosedException") from e

            if received <= 0:
                print("Failed to receive or connection closed by server")
                raise Exception("ConnectionClosedException")

            data = buffer[:received].decode('utf-8')  # Assuming the data is UTF-8 encoded

            match = re.match(r'<([^>]+)>: .*', data)
            if match:
                sent_username = match.group(1)
                if sent_username == self.rcpt_username:
                    print(data)
                    # Write to file
                    # with open(f'chats/{sent_username}.txt', 'a') as f:
                    #     f.write(data + '\n')
                else:
                    pass
            # Write to file
            # with open(f'chats/{sent_username}.txt', 'a') as f:
            #     f.write(data + '\n')

    def send_data(self, buflen=1024):
        while True:
            input_data = input(f"<{self.username}>: ")
            buffer = f"<{self.username}>: {input_data}"

            try:
                sent_bytes = self.sock.send(buffer.encode('utf-8'))  # Assuming the data is UTF-8 encoded
            except socket.error as e:
                print("Failed to send the message")
                raise Exception("SendFailedException") from e

            # Write to file
            # with open(f'chats/{self.rcpt_username}.txt', 'a') as f:
            #     f.write(buffer + '\n')

    def handshake_transaction(self, buflen=1024):
        buffer = "HELO"
        self.send_data_once(buffer)

        buffer = self.receive_data_once(buflen)
        if buffer != "WHO":
            print("Failed to receive valid command from server")
            raise Exception("FailedHandshakeException")

        elif buffer == "000":
            self.send_data_once(".")
            raise Exception("FailedHandshakeException")

        self.send_data_once(self.username)

        buffer = self.receive_data_once(buflen)
        if buffer != "AUTH":
            print("Failed to receive valid command from server")
            raise Exception("FailedHandshakeException")

        elif buffer == "000":
            self.send_data_once(".")
            raise Exception("FailedHandshakeException")

        self.send_data_once(self.password)

        buffer = self.receive_data_once(buflen)
        if buffer != "TO":
            print("Failed to receive valid command from server")
            return False

        elif buffer == "000":
            self.send_data_once(".")
            raise Exception("FailedHandshakeException")

        self.send_data_once(self.rcpt_username)

        buffer = self.receive_data_once(buflen)
        if buffer != "DATA":
            print("Failed to receive valid command from server")
            return False

        elif buffer == "000":
            self.send_data_once(".")
            raise Exception("FailedHandshakeException")

        return True

    def fetch_buffer_messages(self, buflen=1024):
        OK = "OK"
        buffer = self.receive_data_once(buflen)
        self.send_data_once(OK)

        try:
            num_messages = int(buffer)
        except ValueError:
            raise Exception("FetchBufferMessagesException")

        for _ in range(num_messages):
            buffer = self.receive_data_once(buflen)

            if buffer == "000" or buffer == "202":
                continue

            match = re.match(r'<([^>]+)>: .*', buffer)
            if match:
                rcpt_filename = match.group(1)

                buffer_messages = buffer + '\n'

                # Write to file
                # with open(f'chats/{rcpt_filename}.txt', 'a') as f:
                #     f.write(buffer_messages)

                self.send_data_once(OK)

        return True

    def thread_handler(self):
        # Read file
        # with open(f'chats/{self.rcpt_username}.txt', 'r') as f:
        #     print(f.read())

        # Create send and receive threads
        send_thread = threading.Thread(target=self.send_data)
        receive_thread = threading.Thread(target=self.receive_data)

        # Start the threads
        send_thread.start()
        receive_thread.start()

        # Wait for both threads to finish
        send_thread.join()
        receive_thread.join()

    def update_credentials(self, username, password, rcpt_username):
        self.username = username
        self.password = password
        self.rcpt_username = rcpt_username



#obj = ClientTransaction('localhost', 12346, "Admin", "Password", "User")