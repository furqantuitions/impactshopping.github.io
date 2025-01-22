# impactshopping.github.io
#include <iostream>
#include <string>
#include "curl/curl.h"
#include "single_include/nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

const string FIREBASE_API_KEY = "AIzaSyA67pl5x3lXSN1dpqg0qsa4VSzOXHgjqUU";
const string FIREBASE_DB_URL = "https://your-database-name.firebaseio.com"; // Replace with your Firebase Realtime Database URL

// Callback function for writing CURL response to a string
size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Function to call OpenAI API
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

        // Set CURL options
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
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

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
