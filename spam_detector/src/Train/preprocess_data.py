import pandas as pd


def preprocessSpamData(path: str, delimiter: str) -> pd.DataFrame:
    # Specify the file path
    file_path = path

    # Read the file into a pandas DataFrame
    df = pd.read_csv(file_path, delimiter=delimiter, header=None, names=['spam', 'text'])

    # Replace 'spam' with 1 and 'ham' with 0
    df['spam'] = df['spam'].replace({'spam': 1, 'ham': 0})

    # Drop rows where the 'spam' column is not equal to 0 or 1
    df = df[df['spam'].isin([0, 1])]

    # return the DataFrame
    return df

def preprocessSpamDataExtra(path: str, delimiter: str) -> pd.DataFrame:
    # Specify the file path
    file_path = path

    # Define column names, assuming there are 5 columns in total
    col_names = ['spam', 'text', 'extra1', 'extra2', 'extra3']

    # Read the file into a pandas DataFrame
    df = pd.read_csv(file_path, delimiter=delimiter, header=None, names=col_names)

    # Replace 'spam' with 1 and 'ham' with 0
    df['spam'] = df['spam'].replace({'spam': 1, 'ham': 0})

    # Drop rows where the 'spam' column is not equal to 0 or 1
    df = df[df['spam'].isin([0, 1])]

    # Drop the extra columns
    df.drop(['extra1', 'extra2', 'extra3'], axis=1, inplace=True)

    # return the DataFrame
    return df


def getData():
    df1 = preprocessSpamData("../../Data/sms+spam+collection/SMSSpamCollection", '\t')
    df2 = preprocessSpamDataExtra("../../Data/archive/spam.csv", ',')

    df_combined = pd.concat([df1, df2], ignore_index=True)

    return df_combined
