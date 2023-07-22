import pandas as pd
from sklearn.model_selection import train_test_split
from tensorflow.keras.preprocessing.text import Tokenizer
from tensorflow.keras.preprocessing.sequence import pad_sequences
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, LSTM, Dropout, Embedding
from tensorflow.keras import regularizers
import numpy as np
from preprocess_data import getData

import sys

sys.path.append('../')

from utils.config_reader import read_config_file

maxlen, num_words = read_config_file('../../.config/config.json')


class SpamDetectorTrainer:
    def __init__(self, glove_path, model_path):
        """
        Initialize the SpamDetector class with data, GloVe and model paths.
        Set tokenizer, maxlen and embedding_dim variables.
        """
        self.glove_path = glove_path
        self.model_path = model_path
        self.tokenizer = Tokenizer(num_words=num_words)
        self.maxlen = maxlen
        self.embedding_dim = 200

    def load_and_split_data(self):
        """
        Load and preprocess data, split it into train, validation and test sets.
        """
        df = getData()
        X = df['text']
        y = df['spam']
        X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
        X_train, X_val, y_train, y_val = train_test_split(X_train, y_train, test_size=0.25, random_state=42)

        self.X_train, self.y_train = X_train, y_train
        self.X_val, self.y_val = X_val, y_val
        self.X_test, self.y_test = X_test, y_test

    def process_text(self):
        """
        Fit tokenizer on training texts and convert text into sequences.
        """
        self.tokenizer.fit_on_texts(self.X_train)

        X_train_seq = self.tokenizer.texts_to_sequences(self.X_train)
        X_val_seq = self.tokenizer.texts_to_sequences(self.X_val)

        self.X_train_seq_padded = pad_sequences(X_train_seq, self.maxlen)
        self.X_val_seq_padded = pad_sequences(X_val_seq, self.maxlen)

    def create_embedding_matrix(self):
        """
        Create an embedding matrix using pre-trained GloVe embeddings.
        """
        vocab_size = len(self.tokenizer.word_index) + 1
        embedding_matrix = np.zeros((vocab_size, self.embedding_dim))

        with open(self.glove_path) as f:
            for line in f:
                word, *vector = line.split()
                if word in self.tokenizer.word_index:
                    idx = self.tokenizer.word_index[word]
                    embedding_matrix[idx] = np.array(vector, dtype=np.float32)[:self.embedding_dim]

        self.embedding_matrix = embedding_matrix
        self.vocab_size = vocab_size

    def build_model(self):
        """
        Build LSTM model with embedding, LSTM, and Dense layers.
        """
        self.model = Sequential()
        self.model.add(Embedding(input_dim=self.vocab_size,
                                 output_dim=self.embedding_dim,
                                 weights=[self.embedding_matrix],
                                 input_length=self.maxlen,
                                 trainable=True))
        self.model.add(LSTM(units=50,
                            return_sequences=True,
                            dropout=0.2,
                            recurrent_dropout=0.2))
        self.model.add(Dropout(0.5))
        self.model.add(LSTM(units=50,
                            dropout=0.2,
                            recurrent_dropout=0.2))
        self.model.add(Dropout(0.5))
        self.model.add(Dense(1, activation='sigmoid', kernel_regularizer=regularizers.l2(0.01)))

        self.model.compile(loss='binary_crossentropy',
                           optimizer='adam',
                           metrics=['accuracy'])

    def train_model(self):
        """
        Train the LSTM model with training and validation data.
        """
        self.model.fit(self.X_train_seq_padded,
                       self.y_train,
                       validation_data=(self.X_val_seq_padded, self.y_val),
                       epochs=10,
                       verbose=1)

    def save_model(self):
        """
        Save the trained model to the specified path.
        """
        self.model.save(self.model_path)

    def train(self):
        """
        Execute all the steps to load data, preprocess, build, train, and save the model.
        """
        self.load_and_split_data()
        self.process_text()
        self.create_embedding_matrix()
        self.build_model()
        self.train_model()
        self.save_model()


if __name__ == "__main__":
    spam_detector_trainer = SpamDetectorTrainer(glove_path='../../Data/glove.twitter.27B/glove.twitter.27B.200d.txt',
                                                model_path='../../Model/')
    spam_detector_trainer.train()
