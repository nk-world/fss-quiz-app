#include <vector>
#include <string>
#include <algorithm>
#include <random>
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
json data;
std::string LoadJsonFromServer(const std::string &url);
void initialize(const std::string &LoadUrl, const std::string &SaveUrl)
{
    data = json::parse(LoadJsonFromServer(LoadUrl));
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


void fixEventId(json &data)
{
    int events_size = data["Events"].size();
    for(int i=0; i<events_size; i++)
        data["Events"][i]["id"] = i+1;
}

void fixCategoryId(json &data)
{
    for(int i=0; i<data["Events"].size(); i++)
        for(int j=0; j<data["Events"][i]["Categories"].size(); j++)
            data["Events"][i]["Categories"][j] = j+1;
}

void fixRoundId(json &data)
{
    for(int i=0; i<data["Events"].size(); i++)
        for(int j=0; j<data["Events"][i]["Categories"].size(); j++)
            for(int k=0; k<data["Events"][i]["Categories"][j]["Rounds"].size(); k++)
                data["Events"][i]["Categories"][j]["Rounds"][k] = k+1;
}

void fixQuestionId(json &data)
{
    for(int i=0; i<data["Events"].size(); i++)
        for(int j=0; j<data["Events"][i]["Categories"].size(); j++)
            for(int k=0; k<data["Events"][i]["Categories"][j]["Rounds"].size(); k++)
                for(int l=0; l<data["Events"][i]["Categories"][j]["Rounds"][k]["Questions"].size(); l++)
                    data["Events"][i]["Categories"][j]["Rounds"][k]["Questions"][l] = l+1;
}

void Shuffle(json &jsonData)
{
    std::random_device random;
    std::mt19937 generate(random());

    std::shuffle(jsonData.begin(), jsonData.end(), generate);
}

std::string addEvent(const std::string &name)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));

    json event = json::object();
    event["name"] = name;
    event["id"] = data["Events"].size()+1;

    data["Events"].push_back(event);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Event added successfully";
    else
        return "Adding event failed";
}

std::string updateEvent(const std::string &NewName, const int &EventID)
{
    const int EventId = EventID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if(!data.contains("Events"))
        return "Events were not found in the specified JSON";        
    
    if(!data["Events"].is_array())
        return "Events is not an array in the specified JSON";

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Event with ID " + std::to_string(EventId) + " was not found";
    }

    data["Events"][EventId]["name"] = NewName;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Event name updated successfully";
    else
        return "Event name update failed";
}

std::string deleteEvent(const int &EventID)
{
    const int EventId = EventID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    if(!data.contains("Events"))
        return "Events were not found in the specified JSON";        
    
    if(!data["Events"].is_array())
        return "Events is not an array in the specified JSON";

    if (EventId < 0 || EventId > (data["Events"].size()-1))
    {
        return "Event with ID " + std::to_string(EventId) + " was not found";
    }

    data["Events"].erase(EventId);
    fixEventId(data);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Event name updated successfully";
    else
        return "Event name update failed";
}

std::string addCategory(const std::string &categoryName, const int &EventID)
{
    const int EventId = EventID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    json category = json::object();
    category["name"] = categoryName;
    category["id"] = data["Events"][EventId]["Categories"].size()+1;
    data["Events"][EventId]["Categories"].push_back(category);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Category added successfully";
    else
        return "Adding category failed";
}

std::string updateCategory(const std::string &newName, const int &EventID, const int &CategoryID)
{
    const int EventId = EventID-1, CategoryId = CategoryID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";

    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);

    data["Events"][EventId]["Categories"][CategoryId]["name"] = newName;
    
    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Category updated successfully";
    else
        return "Updating category failed";
}

std::string deleteCategory(const int &EventID, int &CategoryID)
{
    const int EventId = EventID-1, CategoryId = CategoryID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";

    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);

    data["Events"][EventId]["Categories"].erase(CategoryId);
    fixCategoryId(data);
    
    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Category deleted successfully";
    else
        return "Deleting category failed";
}

std::string addRound(const std::string &name, const int &EventID, const int &CategoryID)
{
    const int EventId = EventID-1, CategoryId = CategoryID-1;

    json data = json::parse(LoadJsonFromServer(loadUrl));

    json round = json::object();
    round["name"] = name;
    round["id"] = data["Events"][EventId]["Categories"][CategoryId]["Rounds"].size()+1;

    data["Events"][EventId]["Categories"][CategoryId]["Rounds"].push_back(round);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Round added successfully";
    else
        return "Adding round failed";
}

std::string updateRound(const std::string &NewName, const int &EventID, const int &CategoryID, const int &RoundID)
{
    const int EventId = EventID-1, RoundId = RoundID-1, CategoryId = CategoryID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";
    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";
    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);
    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in event ID " + std::to_string(EventId) + " is not an array";
    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);

    // check if there are rounds
    if(!data["Events"][EventId]["Categories"][CategoryId].contains("Rounds"))
        return "Rounds were not found";
    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"].is_array())
        return "Rounds is not an array";
    if(RoundId < 0 || RoundId > (data["Events"][EventId]["Categories"][CategoryId]["Rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist";

    data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["name"] = NewName;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Round name updated successfully";
    else
        return "Round name update failed";
}

std::string deleteRound(const int &EventID, const int &RoundID, const int &CategoryID)
{
    const int EventId = EventID-1, RoundId = RoundID-1, CategoryId = CategoryID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

   // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";

    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);

    // check if there are rounds
    if(!data["Events"][EventId]["Cateogires"][CategoryId].contains("Rounds"))
        return "Rounds were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Cateogires"][CategoryId]["Rounds"].is_array())
        return "Rounds in event ID " + std::to_string(EventId) + " is not an array";

    if(RoundId < 0 || RoundId > (data["Events"][EventId]["Cateogires"][CategoryId]["Rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist in event " + std::to_string(EventId);

    data["Events"][EventId]["Categories"][CategoryId]["Rounds"].erase(RoundId);
    fixRoundId(data);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Round deleted successfully";
    else
        return "Round deletion failed";
}

std::string addQuestion(const int &EventID, const int &RoundID, const int &CategoryID, Question &question)
{
    const int RoundId = RoundID-1, EventId = EventID-1, CategoryId = CategoryID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["Questions"].push_back(question.toJson());

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Question added successfully";
    else
        return "Adding question failed";
}

std::string updateQuestion(const int &RoundID, const int &EventID, const int &CategoryID, const int &QuestionID, const std::string &newName)
{
    const int RoundId = RoundID-1, EventId = EventID-1, CategoryId = CategoryID-1, QuestionId = QuestionID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";

    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);
        
    // check if there are rounds
    if(!data["Events"][EventId]["Categories"][CategoryId].contains("Rounds"))
        return "Rounds were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"].is_array())
        return "Rounds in event ID " + std::to_string(EventId) + " is not an array";

    if(RoundId < 0 || RoundId > (data["Events"][EventId]["Categories"][CategoryId]["Rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist in event " + std::to_string(EventId);

    // check if there are questions
    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId].contains("Questions"))
        return "Questions were not found in the given IDs";

    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["Questions"].is_array())
        return "Questions is not an array in the given IDs";

    if(QuestionId < 0 || QuestionId > (data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["Questions"].size()-1))
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["Questions"][QuestionId]["title"] = newName;

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Question renamed successfully";
    else
        return "Renaming question failed";
}

std::string deleteQuestion(const int &RoundID, const int &EventID, const int &QuestionID, const int &CategoryID, const std::string &newName)
{
    const int RoundId = RoundID-1, EventId = EventID-1, CategoryId = CategoryID-1, QuestionId = QuestionID-1;
    json data = json::parse(LoadJsonFromServer(loadUrl));

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";

    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);
    // check if there are rounds
    if(!data["Events"][EventId]["Categories"][CategoryId].contains("Rounds"))
        return "Rounds were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"].is_array())
        return "Rounds in event ID " + std::to_string(EventId) + " is not an array";

    if(RoundId < 0 || RoundId > (data["Events"][EventId]["Categories"][CategoryId]["Rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist in event " + std::to_string(EventId);

    // check if there are questions
    if(!data["Events"][EventId]["Rounds"][RoundId]["Categories"][CategoryId].contains("Questions"))
        return "Questions were not found in the given IDs";

    if(!data["Events"][EventId]["Rounds"][RoundId]["Categories"][CategoryId]["Questions"].is_array())
        return "Questions is not an array in the given IDs";

    if(QuestionId < 0 || QuestionId > (data["Events"][EventId]["Rounds"][RoundId]["Categories"][CategoryId]["Questions"].size()-1))
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["Questions"].erase(QuestionId);
    fixQuestionId(data);

    bool isSuccess = SaveDataToServer(data);
    if(isSuccess)
        return "Question deleted successfully";
    else
        return "Deleting question failed";
}

std::vector<std::string> showRounds(const int &EventID, const int &CategoryID)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));
    const int EventId = EventID-1, CategoryId = CategoryID-1;
    std::vector<std::string> rounds;

    for(auto round : data["Events"][EventId]["Categories"][CategoryId]["Rounds"])
        rounds.push_back(round["name"]);

    return rounds;
}

json loadRoundData(const int &EventID, const int &CategoryID, const int &RoundID)
{
    json data = json::parse(LoadJsonFromServer(loadUrl));
    const int EventId = EventID-1, CategoryId = CategoryID-1, RoundId = RoundID-1;

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";
    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";
    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);
    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in event ID " + std::to_string(EventId) + " is not an array";
    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);

    // check if there are rounds
    if(!data["Events"][EventId]["Categories"][CategoryId].contains("Rounds"))
        return "Rounds were not found";
    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"].is_array())
        return "Rounds is not an array";
    if(RoundId < 0 || RoundId > (data["Events"][EventId]["Categories"][CategoryId]["Rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist";

    json roundData = data["Events"][EventId]["Category"][CategoryId]["Rounds"][RoundId];
    
    for(int i=0; i<roundData["Questions"].size(); i++)
        Shuffle(roundData["Questions"][i]["options"]);
    return roundData;
}

json loadQuestion(const int &EventID, const int &RoundID, const int &CategoryID, const int &QuestionID)
{
    const int RoundId = RoundID-1, EventId = EventID-1, CategoryId = CategoryID-1, QuestionId = QuestionID-1;

    // check if there are events
    if(!data.contains("Events"))
        return "Events were not found in the JSON";

    if(!data["Events"].is_array())
        return "Events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["Events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["Events"][EventId].contains("Categories"))
        return "Categories were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"].is_array())
        return "Categories in in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["Events"][EventId]["Categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);
    // check if there are rounds
    if(!data["Events"][EventId]["Categories"][CategoryId].contains("Rounds"))
        return "Rounds were not found in event ID " + std::to_string(EventId);

    if(!data["Events"][EventId]["Categories"][CategoryId]["Rounds"].is_array())
        return "Rounds in event ID " + std::to_string(EventId) + " is not an array";

    if(RoundId < 0 || RoundId > (data["Events"][EventId]["Categories"][CategoryId]["Rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist in event " + std::to_string(EventId);

    // check if there are questions
    if(!data["Events"][EventId]["Rounds"][RoundId]["Categories"][CategoryId].contains("Questions"))
        return "Questions were not found in the given IDs";

    if(!data["Events"][EventId]["Rounds"][RoundId]["Categories"][CategoryId]["Questions"].is_array())
        return "Questions is not an array in the given IDs";

    if(QuestionId < 0 || QuestionId > (data["Events"][EventId]["Rounds"][RoundId]["Categories"][CategoryId]["Questions"].size()-1))
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    json question = data["Events"][EventId]["Categories"][CategoryId]["Rounds"][RoundId]["Questions"][QuestionId];
    Shuffle(question["options"]);
    return question;
}


EMSCRIPTEN_BINDINGS(my_module)
{
    register_vector<std::string>("VectorString");

    function("AddNewEvent", &addEvent);
    function("UpdateEventName", &updateEvent);
    function("DeleteEvent", &deleteEvent);

    function("AddNewCategory", &addCategory);
    function("UpdateCategoryName", &updateCategory);
    function("DeleteCategory", &deleteCategory);
    
    function("AddNewRound", &addRound);
    function("UpdateRoundName", &updateRound);
    function("DeleteRound", &deleteRound);

    function("AddNewQuestionToRound", &addQuestion);
    function("UpdateRoundQuestions", &updateQuestion);
    function("DeleteRoundQuestions", &deleteQuestion);

    function("LoadRoundData", &loadRoundData);
    function("ShowRounds", &showRounds);
    function("LoadJsonFromServer", &LoadJsonFromServer);
    function("LoadQuestionData", &loadQuestion);

    function("initialize", &initialize);
}
