#include "Jarvis.h"
#include <iostream>
#include <curl/curl.h>
#include "json.hpp"
#include <algorithm>
#include <sstream>
using json = nlohmann::json;
using namespace std;


Jarvis::Jarvis() {

}

void Jarvis::greet() {
    cout << "Good Morning, Sir." << endl;
}

void Jarvis::listen() {
    while(true) {
        string command;
        cout << "Listening for commands..." << endl;
        getline(cin, command);
        if (command == "exit") {
            cout << "Goodbye, Sir." << endl;
            break;
        } else if (command == "weather") {
            cout << "Please enter a location: ";
            string location;
            getline(cin, location);
            cout << getWeather(location) << endl;
        } else if (command == "help") {
            cout << "Available commands: weather, help, exit" << endl;
        } 
        else {
            cout << "Command not recognized. Please try again." << endl;
        }
    }
}


static size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    output->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string Jarvis::getWeather(string location) {
    string encodedLocation = location;
    replace(encodedLocation.begin(), encodedLocation.end(), ' ', '+');
    encodedLocation.erase(remove(encodedLocation.begin(), encodedLocation.end(), ','), encodedLocation.end());
    CURL* curl;
    CURLcode res;
    string response;

    string geoUrl = "https://geocoding-api.open-meteo.com/v1/search?name=" + encodedLocation + "&count=1";

    string url = "https://api.open-meteo.com/v1/forecast?latitude=29.42&longitude=-98.49&current_weather=true";

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, geoUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

    }

    if (res != CURLE_OK) {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return "Error fetching weather data.";
    }

    json geoData = json::parse(response);
    float lat = geoData["results"][0]["latitude"];
    float lon = geoData["results"][0]["longitude"];
    string weatherUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + to_string(lat) + "&longitude=" + to_string(lon) + "&current_weather=true";

    response.clear();
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, weatherUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

    }

    if (res != CURLE_OK) {
        cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
        return "Error fetching weather data.";
    }

    json weatherData = json::parse(response);
    float temp = weatherData["current_weather"]["temperature"];
    float windspeed = weatherData["current_weather"]["windspeed"];
    
    ostringstream result;
    result << "Current weather in " << geoData["results"][0]["name"] << ": " << temp << "°C, Wind Speed: " << windspeed << " km/h";
    return result.str();

}   