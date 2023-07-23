from flask import Flask, render_template, request, redirect, url_for
from flask_socketio import SocketIO, send, emit

app = Flask(__name__)
app.config['SECRET_KEY'] = 'your-secret-key'  # Use a secret key for secure communication
socketio = SocketIO(app)


@app.route('/')
def home():
    return render_template('index.html')

@app.route('/signin', methods=['GET', 'POST'])
def signin():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        # Note: Here, you would typically add the logic for checking the username and password.
        # If valid, you can log the user in and then redirect them to the chat_app.
        # For the sake of simplicity, we're not adding that logic and assuming the credentials are valid.
        return redirect(url_for('chat_app'))
    return render_template('signin.html')

@app.route('/signup', methods=['GET', 'POST'])
def signup():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        confirm_password = request.form['confirmPassword']
        # Add your logic here to validate the submitted data and create a new user in your system
        # For example, you might want to check that the username isn't already taken, and that password and confirmPassword match
        # For now, we'll assume the data is valid and redirect to the chat_app
        return redirect(url_for('chat_app'))
    return render_template('signup.html')

@app.route('/about')
def about():
    return render_template('about.html')

@app.route('/chat')
def chat():
    return render_template('chat.html')

@app.route('/chat_app')
def chat_app():
    return render_template('chat_app.html')

@app.route('/signout')
def signout():
    return redirect(url_for('home'))


@socketio.on('sendmessage')
def handle_message(data):
    print('Received message: ' + str(data))
    recipient = data['recipient']
    message = data['message']

    # Process the message (send it to the recipient, store in DB, etc.)
    # In this example, we simply echo the message back to the sender
    emit('newmessage', message, broadcast=True)

@socketio.on('updateRecipient')
def update_recipient(username):
    print('Recipient username updated to ', username)
    # Do something with the new username...



if __name__ == '__main__':
    socketio.run(app, debug=True)