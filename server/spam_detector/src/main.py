from InterprocessCommunication.interprocess_communication import InterprocessCommunicationMessageQueue
from InterprocessCommunication.interprocess_communication import InterprocessCommunicationSharedMemory
from Classify.classify import SpamPredictor

import sys
import signal


class SpamDetector:

    def __init__(self, name):
        try:
            self.mq = InterprocessCommunicationMessageQueue(name)
            self.spamDetector = SpamPredictor('../Model')
        except Exception as e:
            print(f"Error during initialization: {e}")
            raise

    def __del__(self):
        self.mq.closeMessageQueue()

    def run(self):
        while True:
            [shared_memory_name, semaphore_name] = self.mq.read().split('|')
            sm = InterprocessCommunicationSharedMemory(shared_memory_name, semaphore_name)

            text = sm.receive()
            labels = self.spamDetector.run([text])
            sm.send(str(labels[0]))

    def sigint_handler(self, signal, frame):
        self.mq.closeMessageQueue()
        print("\nReceived SIGINT. Halting process...")
        sys.exit(0)


if __name__ == "__main__":
    sd = SpamDetector(sys.argv[1])

    signal.signal(signal.SIGINT, sd.sigint_handler)

    sd.run()
