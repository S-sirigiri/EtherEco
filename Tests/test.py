import posix_ipc

# Create the message queue
mq = posix_ipc.MessageQueue("/test_queue", posix_ipc.O_CREAT )

while True:
    # Receive the message
    message, priority = mq.receive()

    # Decode the message
    message = message.decode('utf-8')

    # Print the message
    print("Received: " + message)

    # If the message is "exit", break the loop
    if message.strip() == 'exit':
        break

# Close the queue
mq.close()
