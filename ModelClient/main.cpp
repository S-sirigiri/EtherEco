#include <gtkmm.h>
#include <memory>
#include <iostream>

std::shared_ptr<Gtk::Window> pWindowHome;
std::shared_ptr<Gtk::Button> pButtonSignIn;
std::shared_ptr<Gtk::Button> pButtonSignUp;
std::shared_ptr<Gtk::Window> pWindowSignIn;
std::shared_ptr<Gtk::Window> pWindowSignUp;
std::shared_ptr<Gtk::Window> pWindowChat;
std::shared_ptr<Gtk::Entry> pUsernameEntry;
std::shared_ptr<Gtk::TextView> pChatDisplay;
std::shared_ptr<Gtk::Entry> pMessageEntry;
std::shared_ptr<Gtk::Button> pSendButton;


void load_chat_window();

void on_button_signin_clicked() {

    Glib::RefPtr<Gtk::Builder> refBuilderSignIn = Gtk::Builder::create();
    try {
        refBuilderSignIn->add_from_file("signin.glade");
    }
    catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }


    auto signInUsernameEntry = dynamic_cast<Gtk::Entry*>(refBuilderSignIn->get_object("entry_username").operator->());
    auto signInPasswordEntry = dynamic_cast<Gtk::Entry*>(refBuilderSignIn->get_object("entry_password").operator->());
    std::string signin_username = signInUsernameEntry->get_text();
    std::string signin_password = signInPasswordEntry->get_text();
    std::cout << "Sign In - Username: " << signin_username << ", Password: " << signin_password << std::endl;

    auto refBuilder = Gtk::Builder::create();

    try {
        refBuilder->add_from_file("signin.glade");
    }
    catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }

    Gtk::Window* pWin = nullptr;
    refBuilder->get_widget("window", pWin);
    pWindowSignIn = std::shared_ptr<Gtk::Window>(pWin);
    if(pWindowSignIn) {
        pWindowSignIn->signal_hide().connect([=]() { pWindowHome->show(); });
        pWindowHome->hide();
        pWindowSignIn->show();
        load_chat_window();
    }
}

void on_button_signup_clicked() {

    Glib::RefPtr<Gtk::Builder> refBuilderSignUp = Gtk::Builder::create();
    try {
        refBuilderSignUp->add_from_file("signup.glade");
    }
    catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }


    auto signUpUsernameEntry = dynamic_cast<Gtk::Entry*>(refBuilderSignUp->get_object("entry_username").operator->());
    auto signUpPasswordEntry = dynamic_cast<Gtk::Entry*>(refBuilderSignUp->get_object("entry_password").operator->());
    auto signUpConfirmPasswordEntry = dynamic_cast<Gtk::Entry*>(refBuilderSignUp->get_object("entry_confirm_password").operator->());
    std::string signup_username = signUpUsernameEntry->get_text();
    std::string signup_password = signUpPasswordEntry->get_text();
    std::string signup_confirm_password = signUpConfirmPasswordEntry->get_text();
    std::cout << "Sign Up - Username: " << signup_username << ", Password: " << signup_password << ", Confirm Password: " << signup_confirm_password << std::endl;

    auto refBuilder = Gtk::Builder::create();

    try {
        refBuilder->add_from_file("signup.glade");
    }
    catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }

    Gtk::Window* pWin = nullptr;
    refBuilder->get_widget("window", pWin);
    pWindowSignUp = std::shared_ptr<Gtk::Window>(pWin);
    if(pWindowSignUp) {
        pWindowSignUp->signal_hide().connect([=]() { pWindowHome->show(); });
        pWindowHome->hide();
        pWindowSignUp->show();
        load_chat_window();
    }
}

void append_message_to_chat(const Glib::ustring& message) {
    auto buffer = pChatDisplay->get_buffer();
    buffer->insert(buffer->end(), message + "\n");
}

void on_username_updated() {
    pChatDisplay->get_buffer()->set_text("");  // Clear the chat display.
}

void on_send_button_clicked() {
    auto buffer = pChatDisplay->get_buffer();
    Glib::ustring message = pUsernameEntry->get_text() + ": " + pMessageEntry->get_text() + "\n";
    buffer->insert(buffer->end(), message);
    pMessageEntry->set_text("");  // Clear the message entry.

    // do whatever you want with the message
    // some_func(message);
}

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_file("./home.glade");
    }
    catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return 1;
    }

    Gtk::Window* pWin = nullptr;
    refBuilder->get_widget("window", pWin);
    if(!pWin) {
        std::cout << "Could not load window from glade file." << std::endl;
    }
    pWindowHome = std::shared_ptr<Gtk::Window>(pWin);
    Gtk::Button* pBtn = nullptr;
    refBuilder->get_widget("signin_button", pBtn);
    if(!pBtn) {
        std::cout << "Could not load signin_button from glade file." << std::endl;
    }
    pButtonSignIn = std::shared_ptr<Gtk::Button>(pBtn);
    refBuilder->get_widget("signup_button", pBtn);
    if(!pBtn) {
        std::cout << "Could not load signup_button from glade file." << std::endl;
    }
    pButtonSignUp = std::shared_ptr<Gtk::Button>(pBtn);

    if(pButtonSignIn) {
        pButtonSignIn->signal_clicked().connect(sigc::ptr_fun(on_button_signin_clicked));
    }

    if(pButtonSignUp) {
        pButtonSignUp->signal_clicked().connect(sigc::ptr_fun(on_button_signup_clicked));
    }

    if(pWindowHome)
        app->run(*pWindowHome);

    return 0;
}


void load_chat_window() {
    auto refBuilderChat = Gtk::Builder::create();
    try {
        refBuilderChat->add_from_file("chat.glade");
    }
    catch(const Glib::FileError& ex) {
        std::cerr << "FileError: " << ex.what() << std::endl;
        return;
    }

    // Get references to the widgets using temporary raw pointers
    Gtk::Window* raw_chat_window = nullptr;
    Gtk::Entry* raw_username_entry = nullptr;
    Gtk::TextView* raw_chat_display = nullptr;
    Gtk::Entry* raw_message_entry = nullptr;
    Gtk::Button* raw_send_button = nullptr;

    refBuilderChat->get_widget("chat_window", raw_chat_window);
    refBuilderChat->get_widget("username_entry", raw_username_entry);
    refBuilderChat->get_widget("chat_display", raw_chat_display);
    refBuilderChat->get_widget("message_entry", raw_message_entry);
    refBuilderChat->get_widget("send_button", raw_send_button);

    // Assign the raw pointers to shared pointers
    pWindowChat = std::shared_ptr<Gtk::Window>(raw_chat_window);
    pUsernameEntry = std::shared_ptr<Gtk::Entry>(raw_username_entry);
    pChatDisplay = std::shared_ptr<Gtk::TextView>(raw_chat_display);
    pMessageEntry = std::shared_ptr<Gtk::Entry>(raw_message_entry);
    pSendButton = std::shared_ptr<Gtk::Button>(raw_send_button);

    // Connect signals.
    pUsernameEntry->signal_changed().connect(sigc::ptr_fun(on_username_updated));
    pSendButton->signal_clicked().connect(sigc::ptr_fun(on_send_button_clicked));

    // Show the chat window.
    pWindowChat->show_all();
}