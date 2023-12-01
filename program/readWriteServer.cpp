#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Function prototype
json LoadJsonFromServer(CURL *curl, string url);
void SaveJsonToServer(CURL *curl, string url, json data);

size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char *>(contents), totalSize);
    return totalSize;
}

int main() {
    CURL *curl = curl_easy_init();
    if (curl) {
        string loadUrl = "http://localhost/quiz/quiz.json";
        string saveUrl = "http://localhost/quiz/write.php";

        // Load JSON data from the server
        json jsonData = LoadJsonFromServer(curl, loadUrl);

        if (!jsonData.empty()) {
            // Access individual fields
            cout << "Loaded Data:\n" << jsonData.dump(4) << endl;

            // Save the updated JSON data to the server
            // add some modification to save (for test case)

            
            SaveJsonToServer(curl, saveUrl, jsonData);
        }

        // Cleanup libcurl
        curl_easy_cleanup(curl);
    } else {
        cerr << "Failed to initialize libcurl." << endl;
    }

    return 0;
}

json LoadJsonFromServer(CURL *curl, string url) {
    json jsonData;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    string response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);

    if (res == CURLE_OK) {
        try {
            jsonData = json::parse(response);
        } catch (const json::exception &e) {
            cerr << "Error parsing JSON from server: " << e.what() << endl;
        }
    } else {
        cerr << "Failed to fetch data from the server: " << curl_easy_strerror(res) << endl;
    }

    return jsonData;
}

void SaveJsonToServer(CURL *curl, string url, json data) {
    // Serialize the JSON data to a string
    string jsonData = data.dump();

    // Set the appropriate HTTP headers for the POST request
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // Configure libcurl for the HTTP POST request
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.length());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

    // Perform the HTTP POST request
    CURLcode res = curl_easy_perform(curl);

    // Check if the request was successful
    if (res == CURLE_OK) {
        cout << "Data updated and sent to the server." << endl;
    } else {
        cerr << "HTTP request to save JSON failed: " << curl_easy_strerror(res) << endl;
    }

    // Clean up libcurl resources
    curl_slist_free_all(headers);
}
