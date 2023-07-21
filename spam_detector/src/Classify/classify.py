import tensorflow as tf
from tensorflow.keras.preprocessing.sequence import pad_sequences
from tensorflow.keras.preprocessing.text import Tokenizer

import sys
sys.path.append("../")

from utils.config_reader import read_config_file

maxlen, num_words = read_config_file('../.config/config.json')

class SpamPredictor:
    def __init__(self, model_path):
        """
        Initialize the SpamPredictor class with model path, texts to classify,
        number of words for tokenizer, and maximum length for sequences.
        """
        self.model_path = model_path
        self.texts = []
        self.num_words = num_words
        self.maxlen = maxlen

    def load_model(self):
        """
        Load the pre-trained model from the specified path.
        """
        self.model = tf.keras.models.load_model(self.model_path)

    def preprocess_texts(self, texts):
        """
        Convert input texts into padded sequences of integers.
        Use the same tokenizer used in the training phase.
        """
        self.texts = texts
        self.tokenizer = Tokenizer(num_words=self.num_words)
        new_sequences = self.tokenizer.texts_to_sequences(self.texts)
        self.new_padded_sequences = pad_sequences(new_sequences, maxlen=self.maxlen)

    def predict(self):
        """
        Use the loaded model to predict the class of each text.
        Output from the model are probabilities of belonging to spam class.
        """
        self.predictions = self.model.predict(self.new_padded_sequences)

    def convert_to_labels(self):
        """
        Convert prediction probabilities into class labels.
        Use a threshold of 0.5 to classify a text as spam or not.
        """
        self.labels = [1 if p > 0.5 else 0 for p in self.predictions]

    def run(self, texts):
        """
        Execute all the steps to load model, preprocess texts, predict and convert to labels.
        """
        self.load_model()
        self.preprocess_texts(texts)
        self.predict()
        self.convert_to_labels()
        return self.labels

if __name__ == "__main__":
    # Example texts for classification
    texts = ["Congratulations, you've won a $1000 gift card. Click here to claim!",
             "Hey, are you coming to the gym tomorrow?"]

    # Instantiate and run the SpamPredictor
    spam_predictor = SpamPredictor(model_path='../../Model')
    labels = spam_predictor.run(texts)
    print(labels)  # Prints the predicted labels (1 for spam, 0 for not spam)
