#include <iostream>
#include <string>
#include "curl/curl.h"
#include "single_include/nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

const string FIREBASE_API_KEY = "AIzaSyArP--v6nWJs3D_4zshWA-tIErCs_UVMdA";
const string FIREBASE_DB_URL = "https://impactshopping-3f088-default-rtdb.firebaseio.com"; // Replace with your Firebase Realtime Database URL

// Callback function for writing CURL response to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}
// Function to make a Firebase Authentication request
string makeFirebaseRequest(const std::string& endpoint, const json& payload) {
     string userId = "null";
        int check=0;
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
       
        // Construct the full URL
        std::string url = "https://identitytoolkit.googleapis.com/v1/accounts:" + endpoint + "?key=" + FIREBASE_API_KEY;

        // Convert JSON payload to string
        std::string postData = payload.dump();

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set HTTP headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            std::cout << "HTTP POST Response:\n" << response << "\n";

            // Parse JSON response
            try {
                json jsonResponse = json::parse(response);
                std::cout << "\nParsed JSON Response:\n";

                // Print relevant fields
                if (jsonResponse.contains("idToken")) {
                    std::cout << "ID Token: " << jsonResponse["idToken"] << "\n";
                }
                if (jsonResponse.contains("email")) {
                    std::cout << "Email: " << jsonResponse["email"] << "\n";
                 check=1;}
                if (jsonResponse.contains("localId")) {
                    std::cout << "User ID: " << jsonResponse["localId"] << "\n";
                      userId = jsonResponse["localId"];
               check=1;  }
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "JSON Parsing Error: " << e.what() << "\n";
            }
           
        } else {
            std::cerr << "CURL Error: " << curl_easy_strerror(res) << "\n";
        } 

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    return userId;
}
// Function to write data to Firebase Realtime Database
void writeToDatabase(string userId,string name,string email) {
    string nodeName = "users", jsonData ="{ \"" + userId + "\": {"
                       "\"userId\": \"" + userId + "\", "
                       "\"name\": \"" + name + "\", "
                       "\"email\": \"" + email + "\""
                       "} }";


  
    CURL* curl;
    CURLcode res;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        string url = FIREBASE_DB_URL + "/" + nodeName + ".json?auth=" + FIREBASE_API_KEY;

        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set HTTP headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Data successfully written to Firebase:\n" << response << "\n";
        } else {
            cerr << "CURL Error: " << curl_easy_strerror(res) << "\n";
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}

// Function to read data from Firebase Realtime Database
void readFromDatabase(string userId) {
    string nodeName = "users";

    CURL* curl;
    CURLcode res;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        string url = FIREBASE_DB_URL + "/" + nodeName + "/" + userId + ".json?auth=" + FIREBASE_API_KEY;
        // Set CURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        // Perform the request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Data successfully read from Firebase:\n" << response << "\n";
        } else {
            cerr << "CURL Error: " << curl_easy_strerror(res) << "\n";
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}
// Function to handle user sign-up
void signUp() {
    std::string email, password,name;
    std::cout << "Enter email for sign-up: ";
    std::cin >> email;
    std::cout << "Enter password: ";
    std::cin >> password;
    cout <<"Enter name:";
   cin >> name;

    // JSON payload for sign-up
    json payload = {
        {"email", email},
        {"password", password},
        {"returnSecureToken", true}
    };
string userId = makeFirebaseRequest("signUp", payload);
  if( userId != "null"){
   writeToDatabase(userId,name,email);
  }
    
}

// Function to handle user login
void signIn() {
    std::string email, password;
    std::cout << "Enter email for sign-in: ";
    std::cin >> email;
    std::cout << "Enter password: ";
    std::cin >> password;

    // JSON payload for sign-in
    json payload = {
        {"email", email},
        {"password", password},
        {"returnSecureToken", true}
    };

    string userId =  makeFirebaseRequest("signInWithPassword", payload);
    readFromDatabase(userId);
}

void callOpenAIAPI() {
    string apiKey = "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";
    string prompt;
    cout << "Enter the prompt for OpenAI: ";
    getline(cin, prompt);

    json payload = {
        {"model", "text-davinci-003"},
        {"prompt", prompt},
        {"max_tokens", 100},
        {"temperature", 0.7}
    };

    CURL* curl;
    CURLcode res;
    string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        string url = "https://api.openai.com/v1/completions";

        // Set cURL options
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.dump().c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set HTTP headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apiKey).c_str());
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Enable SSL and certificate verification (do not disable)
        
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_DEFAULT); // Default SSL version

        // Perform the request
        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            cout << "Response from OpenAI API:\n" << response << "\n";
        } else {
            cerr << "CURL Error: " << curl_easy_strerror(res) << "\n";
        }

        // Cleanup
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
}


int main() {
    int choice;

    std::cout << "Firebase Authentication Menu:\n";
    std::cout << "1. Sign Up\n";
    std::cout << "2. Sign In\n3. Write\n4. Read";
    std::cout << "Enter your choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1:
            signUp();
            break;
        case 2:
            signIn();
            break;
            case 3:
            //writeToDatabase();
            break;
            case 4:
           // readFromDatabase();
            break;
            case 5:
            callOpenAIAPI();
        default:
            std::cout << "Invalid choice. Exiting.\n";
            break;
    }

    return 0;
}
