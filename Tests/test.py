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