import json


def read_config_file(file_path):
    with open(file_path, 'r') as file:
        config_data = json.load(file)
    maxlen = config_data["maxlen"]
    num_words = config_data["num_words"]
    return maxlen, num_words
