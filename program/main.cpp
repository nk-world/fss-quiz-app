#include <vector>
#include <string>
#include "json.hpp"

#include <emscripten/bind.h>
#include <emscripten/fetch.h>

using namespace emscripten;
using json = nlohmann::json;

struct Question
{
    int id;
    std::string title;
    std::vector<std::string> options;
    int correctAnswerIndex;
    std::string correctAnswer;

    json toJson()
    {
        return json
        {
            {"id", id},
            {"title", title},
            {"options", options},
            {"correctAnswerIndex", correctAnswerIndex},
            {"correctAnswer", correctAnswer}
        };
    }
};

std::string loadUrl, saveUrl; 
void initialize(const std::string &LoadUrl, const std::string &SaveUrl)
{
    loadUrl = LoadUrl;
    saveUrl = SaveUrl;
}

bool SaveDataToServer(const json &data)
{
    bool success = false;

    // Create an object to configure the fetch request
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.requestData = data.dump().c_str(); // Convert JSON to string
    attr.requestDataSize = data.dump().length();

    // Use emscripten_fetch_create instead of emscripten_fetch
    emscripten_fetch_t *fetch = emscripten_fetch(&attr, saveUrl.c_str());

    if (!fetch)
    {
        // Handle fetch creation failure
        return false;
    }

    // Use a valid timeout value (e.g., -1 for indefinite wait)
    emscripten_fetch_wait(fetch, -1);

    // Process the result (handle success or error)
    if (fetch->status == 200)
    {
        success = true;
        // Handle successful response, if needed
    }
    else
    {
        // Handle other HTTP status codes or errors
        // Log the status and status text for debugging
        printf("HTTP Status: %d - %s\n", fetch->status, fetch->statusText);
    }

    // Clean up
    emscripten_fetch_close(fetch);

    return success;
}

std::string LoadJsonFromServer(const std::string &url)
{
    int resultSize;
    char *resultData = nullptr;

    EM_ASM_({
        // Implement your logic to fetch data from the server using JavaScript
        var url = UTF8ToString($0);
        var xhr = new XMLHttpRequest();
        xhr.open("GET", url, false); // Synchronous request
        xhr.send();
        if (xhr.status === 200)
        {
            var responseData = xhr.responseText;
            // Allocate memory for the result
            var resultPtr = _malloc(responseData.length + 1);
            // Copy the data to Emscripten memory
            stringToUTF8(responseData, resultPtr, responseData.length + 1);
            setValue($1, resultPtr, 'i32');           // Set the result pointer
            setValue($2, responseData.length, 'i32'); // Set the result size
        }
        else
        {
            setValue($1, 0, 'i32'); // Set an error flag
        }
    },
            url.c_str(), &resultData, &resultSize);

    if (resultSize > 0)
    {
        std::string result(resultData, resultSize);
        // Free the allocated memory
        free(resultData);
        return result;
    }
    else
    {
        return "Failed to fetch data from the server.";
    }
}

std::string addEvent(const std::string &name)
{
    std::string success = "Event added successfully";
    std::string error = "Failed to add event";

    json data = json::parse(LoadJsonFromServer(loadUrl));

    json event = json::object();
    event["name"] = name;
    event["id"] = data["Events"].size()+1;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return success;
    else
        return error;
}

std::string updateEvent(const std::string &NewName, const int &EventId)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Invalid id";
    }

    data["Events"][EventId]["name"] = NewName;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Event name updated successfully";
    else
        return "Event name update failed";
}

std::string deleteEvent(const int &EventId)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Invalid id";
    }

    data["Events"].erase(EventId);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Event name updated successfully";
    else
        return "Event name update failed";
}

std::string addRound(const std::string &name, const int &EventId)
{
    std::string success = "Round added successfully";
    std::string error = "Failed to add round";

    json data = json::parse(LoadJsonFromServer(loadUrl));

    json round = json::object();
    round["name"] = name;

    data["Events"][EventId]["Rounds"].push_back(round);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return success;
    else
        return error;
}

std::string updateRound(const std::string &NewName, const int &EventId, const int &RoundId)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Invalid event id";
    }

    if (RoundId < 0 || RoundId > (data["Events"][EventId]["Rounds"].size()-1))
    {
        return "Invalid round id";
    }

    data["Events"][EventId]["Rounds"][RoundId]["name"] = NewName;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Round name updated successfully";
    else
        return "Round name update failed";
}

std::string deleteRound(const std::string &NewName, const int &EventId, const int &RoundId)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Invalid event id";
    }

    if (RoundId < 0 || RoundId > (data["Events"][EventId]["Rounds"].size()-1))
    {
        return "Invalid round id";
    }

    data["Events"][EventId]["Rounds"].erase(RoundId);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Round deleted successfully";
    else
        return "Round deletion failed";
}

std::string addQuestion(const int &RoundId, const int &EventId, Question &question)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Invalid event id";
    }

    if (RoundId < 0 || RoundId > (data["Events"][EventId]["Rounds"].size()-1))
    {
        return "Invalid round id";
    }

    data["Events"][EventId]["Rounds"][RoundId]["Questions"].push_back(question.toJson());

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Question added successfully";
    else
        return "Adding question failed";
}

std::string updateQuestion(const int &RoundId, const int &EventId, const int &QuestionId, const std::string &newName)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Invalid event id";
    }

    if (RoundId < 0 || RoundId > (data["Events"][EventId]["Rounds"].size()-1))
    {
        return "Invalid round id";
    }

    if (QuestionId < 0 || QuestionId > (data["Events"][EventId]["Rounds"][RoundId]["Questions"].size()-1))
    {
        return "Invalid question id";
    }

    data["Events"][EventId]["Rounds"][RoundId]["Questions"][QuestionId]["title"] = newName;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Question renamed successfully";
    else
        return "Renaming question failed";
}

std::string deleteQuestion(const int &RoundId, const int &EventId, const int &QuestionId, const std::string &newName)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Event with ID " + std::to_string(EventId) + " was not found.";
    }

    if (RoundId < 0 || RoundId > (data["Events"][EventId]["Rounds"].size()-1))
    {
        return "Round with ID " + std::to_string(RoundId) + " was not found.";
    }

    if (QuestionId < 0 || QuestionId > (data["Events"][EventId]["Rounds"][RoundId]["Questions"].size()-1))
    {
        return "Question with ID " + std::to_string(QuestionId) + " was not found.";
    }

    if(!data.contains("Events"))
        return "Events were not found in the specified JSON";
    
    if(!data["Events"].is_array())
        return "Events is not an array in the specified JSON";

    if(!data["Events"][EventId].contains("Rounds"))
        return "Rounds were not found in the event with ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Rounds"].is_array())
        return "Rounds in event ID " + std::to_string(EventId) + " is not an array in the specified JSON";

    data["Events"][EventId]["Rounds"][RoundId]["Questions"].erase(QuestionId);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Question deleted successfully";
    else
        return "Deleting question failed";
}

EMSCRIPTEN_BINDINGS(my_module)
{
    register_vector<std::string>("VectorString");

    function("AddNewRound", &addRound);
    function("UpdateRoundName", &updateRound);
    function("DeleteRound", &deleteRound);

    function("AddNewQuestionToRound", &addQuestion);
    function("UpdateRoundQuestions", &updateQuestion);
    function("DeleteRoundQuestions", &deleteQuestion);

    //function("LoadRoundData", &LoadRoundData);
    //function("ShowRounds", &ShowRounds);
    function("LoadJsonFromServer", &LoadJsonFromServer);

    function("initialize", &initialize);
}