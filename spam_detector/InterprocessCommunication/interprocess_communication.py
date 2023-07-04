import posix_ipc
import mmap
import os

"""
Class: InterprocessCommunicationSharedMemory
       This class encapsulates all the methods that handle inter process communication using shared memory
"""


class InterprocessCommunicationSharedMemory:
    """class constructor"""

    def __init__(self, name: str):
        self.sharedMemoryName = name

    """
    Method: receive
            This method reads a string from the shared memory location
            
    Arguments: None
    
    Returns : string -> string read from the shared memory location
    """

    def receive(self) -> str:
        # Create a new memory object
        memory = posix_ipc.SharedMemory(self.sharedMemoryName)

        # Create a new mmap object with the memory object
        mm = mmap.mmap(memory.fd, memory.size)

        # Now that mm is pointing to the shared memory, we can close the memory
        # object's file descriptor
        memory.close_fd()

        # Read from shared memory until a null byte is found
        message = []
        while True:
            byte = mm.read(1)
            if byte == b'\x00':
                break
            message.append(byte)

        # Combine the bytes into a single string and decode it
        readString = b''.join(message).decode('utf-8')

        # Remove the shared memory segment
        posix_ipc.unlink_shared_memory(self.sharedMemoryName)

        return readString

    """
    Method: send
            This method writes a string into the shared memory location
            
    Arguments: string -> message
               String to be written into the shared memory location
    
    Returns : None
    """

    def send(self, message: str) -> None:
        # Create a new shared memory object
        memory = posix_ipc.SharedMemory(self.sharedMemoryName, posix_ipc.O_CREAT, size=os.sysconf('SC_PAGE_SIZE'))

        # Map the shared memory
        mapFile = mmap.mmap(memory.fd, memory.size)

        # Now that mmap has been created, we can close the memory object's file descriptor
        memory.close_fd()

        # Write to shared memory
        mapFile.write(message.encode('utf-8'))

        # Close the mapfile. The shared memory segment will persist until it's deleted.
        mapFile.close()


"""
Class: InterprocessCommunicationMessageQueue
       This class encapsulates all the methods that handle inter process communication using message queues
"""


class InterprocessCommunicationMessageQueue:
    """class constructor"""

    def __init__(self, name: str):
        self.mq = posix_ipc.MessageQueue(name, posix_ipc.O_CREAT)

    """
    Method: read
            This method reads a string from the message queue
            
    Arguments: None
    
    Returns : String -> the message read from the queue
    """

    def read(self) -> str:
        # Receive the message
        message, priority = self.mq.receive()

        # Decode the message
        message = message.decode('utf-8')

        return message

    """
    Method: closeQueue
            This method closes the queue
            
    Arguments: None
    
    Returns : None
    """

    def closeQueue(self) -> None:
        # Close the queue
        self.mq.close()
