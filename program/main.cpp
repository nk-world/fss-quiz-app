#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include "json.hpp"
#include <chrono>
#include <thread>
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
    std::string type;
    std::string path; // for audio visual

    json toJson() const
    {
        return json
        {
            {"id", id},
            {"title", title},
            {"options", options},
            {"correctAnswerIndex", correctAnswerIndex},
            {"correctAnswer", correctAnswer},
            {"type", type},
            {"path", path}
        };
    }
};

std::string loadUrl, saveUrl;
json data;
std::string LoadJsonFromServer();
void initialize(const std::string &LoadUrl, const std::string &SaveUrl)
{
    loadUrl = LoadUrl;
    saveUrl = SaveUrl;
    data = json::parse(LoadJsonFromServer());
}

bool SaveDataToServer()
{
   // std::cout<<jsonData.dump();
    bool success = false;

   // Create fetch attributes
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.requestData = data.dump().c_str();
    attr.requestDataSize = data.dump().length();

    // Use emscripten_fetch_create instead of emscripten_fetch
    emscripten_fetch_t *fetch = emscripten_fetch(&attr, saveUrl.c_str());

    if (!fetch)
    {
        // Handle fetch creation failure
        return false;
    }

    // Wait for the fetch to complete
   while (fetch->numBytes != fetch->totalBytes)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // Process the result (handle success or error)
    if (fetch->status == 200)
    {
        // Handle successful response, if needed
        printf("Fetch successful!\n");
        success = true;
    }
    else
    {
        // Handle other HTTP status codes or errors
        // Log the status and status text for debugging
        printf("HTTP Status: %d - %s\n", fetch->status, fetch->statusText);
    }

    // Clean up the fetch object
    emscripten_fetch_close(fetch);

    return success;  // Indicate that the fetch request has been initiated
}

std::string LoadJsonFromServer()
{
    std::string url = loadUrl;
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


/* void fixEventId()
{
    int events_size = data["events"].size();
    for(int i=0; i<events_size; i++)
        data["events"][i]["id"] = i+1;
}

void fixCategoryId()
{
    for(int i=0; i<data["events"].size(); i++)
        for(int j=0; j<data["events"][i]["categories"].size(); j++)
            data["events"][i]["categories"][j] = j+1;
}

void fixRoundId()
{
    for(int i=0; i<data["events"].size(); i++)
        for(int j=0; j<data["events"][i]["categories"].size(); j++)
            for(int k=0; k<data["events"][i]["categories"][j]["rounds"].size(); k++)
                data["events"][i]["categories"][j]["rounds"][k] = k+1;
}

void fixQuestionId()
{
    for(int i=0; i<data["events"].size(); i++)
        for(int j=0; j<data["events"][i]["categories"].size(); j++)
            for(int k=0; k<data["events"][i]["categories"][j]["rounds"].size(); k++)
                for(int l=0; l<data["events"][i]["categories"][j]["rounds"][k]["questions"].size(); l++)
                    data["events"][i]["categories"][j]["rounds"][k]["questions"][l] = l+1;
} */

void Shuffle(json &jsonData)
{
    std::random_device random;
    std::mt19937 generate(random());

    std::shuffle(jsonData.begin(), jsonData.end(), generate);
}

std::string addEvent(const std::string &name, const int &EventId)
{
    json event = json::object();

    for(auto i : data["events"])
        if(i["id"] == EventId)
            return "Event with ID " + std::to_string(EventId) + " already exists";

    event["name"] = name;
    event["id"] = EventId;

    data["events"].push_back(event);

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Event added successfully";
    else
        return "Adding event failed";
}

std::string updateEvent(const std::string &NewName, const int &EventId)
{

    bool found=false;

    if(!data.contains("events"))
        return "events were not found in the specified JSON";        
    
    if(!data["events"].is_array())
        return "events is not an array in the specified JSON";

    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    data["events"][EventId-1]["name"] = NewName;

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Event name updated successfully";
    else
        return "Event name update failed";
}

std::string deleteEvent(const int &EventId)
{

    bool found=false;

    if(!data.contains("events"))
        return "events were not found in the specified JSON";        
    
    if(!data["events"].is_array())
        return "events is not an array in the specified JSON";

   for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    try
    {
        data["events"].erase(EventId-1);
    }
    catch(const std::exception& e)
    {
        return e.what();
    }
    
    // fixEventId();

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Event deleted successfully";
    else
        return "Event deletion failed";
}

std::string addCategory(const std::string &categoryName, const int &EventId, const int &CategoryId)
{

    

    json category = json::object();


    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            return "Category with ID " + std::to_string(CategoryId) + " already exists";

    category["name"] = categoryName;
    category["id"] = CategoryId;
    data["events"][EventId-1]["categories"].push_back(category);

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Category added successfully";
    else
        return "Adding category failed";
}

std::string updateCategory(const std::string &newName, const int &EventId, const int &CategoryId)
{

    bool found=false;
    bool category_found=false;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";

    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";

    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    // check if there are categories
    if(!data["events"][EventId-1].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"].is_array())
        return "categories in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            category_found=true;
    if(!category_found)
        return "Category with ID " + std::to_string(CategoryId) + " was not found";

    data["events"][EventId-1]["categories"][CategoryId-1]["name"] = newName;
    
    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Category updated successfully";
    else
        return "Updating category failed";
}

std::string deleteCategory(const int &EventId, const int &CategoryId)
{

    bool found=false, category_found=false;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";

    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";

    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    // check if there are categories
    if(!data["events"][EventId-1].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"].is_array())
        return "categories in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            category_found=true;
    if(!category_found)
        return "Category with ID " + std::to_string(CategoryId) + " was not found";

    data["events"][EventId-1]["categories"].erase(CategoryId-1);
    // fixCategoryId();
    
    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Category deleted successfully";
    else
        return "Deleting category failed";
}

std::string addRound(const std::string &name, const int &EventId, const int &CategoryId, const int &RoundId)
{


    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"])
        if(i["id"] == RoundId)
            return "Round with ID " + std::to_string(RoundId) + " already exists";

    json round = json::object();
    round["name"] = name;
    round["id"] = RoundId;

    data["events"][EventId-1]["categories"][CategoryId-1]["rounds"].push_back(round);

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Round added successfully";
    else
        return "Adding round failed";
}

std::string updateRound(const std::string &NewName, const int &EventId, const int &CategoryId, const int &RoundId)
{

    bool found=false, category_found=false, round_found=false;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";
    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";
    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    // check if there are categories
    if(!data["events"][EventId-1].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);
    if(!data["events"][EventId-1]["categories"].is_array())
        return "categories in event ID " + std::to_string(EventId) + " is not an array";
    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            category_found=true;
    if(!category_found)
        return "Category with ID " + std::to_string(CategoryId) + " was not found";

    // check if there are rounds
    if(!data["events"][EventId-1]["categories"][CategoryId-1].contains("rounds"))
        return "rounds were not found";
    if(!data["events"][EventId-1]["categories"][CategoryId-1]["rounds"].is_array())
        return "rounds is not an array";
    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"])
        if(i["id"] == RoundId)
            round_found=true;
    if(!round_found)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["name"] = NewName;

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Round name updated successfully";
    else
        return "Round name update failed";
}

std::string deleteRound(const int &EventId, const int &CategoryId, const int &RoundId)
{

    bool found=false, category_found=false, round_found=false;

   // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";

    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";

    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    // check if there are categories
    if(!data["events"][EventId-1].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"].is_array())
        return "categories in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            category_found=true;
    if(!category_found)
        return "Category with ID " + std::to_string(CategoryId) + " was not found";

    // check if there are rounds
    if(!data["events"][EventId-1]["categories"][CategoryId-1].contains("rounds"))
        return "rounds were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"][CategoryId-1]["rounds"].is_array())
        return "rounds in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"])
        if(i["id"] == RoundId)
            round_found=true;
    if(!round_found)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    data["events"][EventId-1]["categories"][CategoryId-1]["rounds"].erase(RoundId-1);
    // fixRoundId();

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Round deleted successfully";
    else
        return "Round deletion failed";
}

std::string addQuestion(const int &EventId, const int &CategoryId, const int &RoundId, const Question &question)
{


    data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"].push_back(question.toJson());

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Question added successfully";
    else
        return "Adding question failed";
}

std::string updateQuestion(const std::string &newName, const int &EventId, const int &CategoryId, const int &RoundId, const int &QuestionId)
{

    bool found=false, category_found=false, round_found=false, question_found=false;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";

    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";

    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    // check if there are categories
    if(!data["events"][EventId-1].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"].is_array())
        return "categories in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            category_found=true;
    if(!category_found)
        return "Category with ID " + std::to_string(CategoryId) + " was not found";
        
    // check if there are rounds
    if(!data["events"][EventId-1]["categories"][CategoryId-1].contains("rounds"))
        return "rounds were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"][CategoryId-1]["rounds"].is_array())
        return "rounds in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"])
        if(i["id"] == RoundId)
            round_found=true;
    if(!round_found)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    // check if there are questions
    if(!data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1].contains("questions"))
        return "questions were not found in the given IDs";

    if(!data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"].is_array())
        return "questions is not an array in the given IDs";

    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"])
        if(i["id"] == RoundId)
            question_found=true;
    if(!question_found)
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"][QuestionId-1]["title"] = newName;

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Question renamed successfully";
    else
        return "Renaming question failed";
}

std::string deleteQuestion(const int &EventId, const int &CategoryId, const int &RoundId, const int &QuestionId)
{

    bool found=false, category_found=false, round_found=false, question_found=false;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";

    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";

    for(auto i : data["events"])
        if(i["id"] == EventId)
            found=true;
    if(!found)
        return "Event with ID " + std::to_string(EventId) + " was not found";

    // check if there are categories
    if(!data["events"][EventId-1].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"].is_array())
        return "categories in in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"])
        if(i["id"] == CategoryId)
            category_found=true;
    if(!category_found)
        return "Category with ID " + std::to_string(CategoryId) + " was not found";

    // check if there are rounds
    if(!data["events"][EventId-1]["categories"][CategoryId-1].contains("rounds"))
        return "rounds were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId-1]["categories"][CategoryId-1]["rounds"].is_array())
        return "rounds in event ID " + std::to_string(EventId) + " is not an array";

    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"])
        if(i["id"] == RoundId)
            round_found=true;
    if(!round_found)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    // check if there are questions
    if(!(data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1].contains("questions")))
        return "questions were not found in the given IDs";

    if(!(data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"].is_array()))
        return "questions is not an array in the given IDs";

    for(auto i : data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"])
        if(i["id"] == RoundId)
            question_found=true;
    if(!question_found)
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    data["events"][EventId-1]["categories"][CategoryId-1]["rounds"][RoundId-1]["questions"].erase(QuestionId);
    // fixQuestionId();

    bool isSuccess = SaveDataToServer();
    if(isSuccess)
        return "Question deleted successfully";
    else
        return "Deleting question failed";
}


std::vector<std::string> showRounds(const int &EventID, const int &CategoryID)
{
    
    const int EventId = EventID-1, CategoryId = CategoryID-1;
    std::vector<std::string> rounds;

    for(auto round : data["events"][EventId]["categories"][CategoryId]["rounds"])
        rounds.push_back(round["name"]);

    return rounds;
}

json loadRoundData(const int &EventID, const int &CategoryID, const int &RoundID)
{
    
    const int EventId = EventID-1, CategoryId = CategoryID-1, RoundId = RoundID-1;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";
    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";
    if(EventId < 0 || EventId > (data["events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["events"][EventId].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);
    if(!data["events"][EventId]["categories"].is_array())
        return "categories in event ID " + std::to_string(EventId) + " is not an array";
    if(CategoryId < 0 || CategoryId > (data["events"][EventId]["categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);

    // check if there are rounds
    if(!data["events"][EventId]["categories"][CategoryId].contains("rounds"))
        return "rounds were not found";
    if(!data["events"][EventId]["categories"][CategoryId]["rounds"].is_array())
        return "rounds is not an array";
    if(RoundId < 0 || RoundId > (data["events"][EventId]["categories"][CategoryId]["rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist";

    json roundData = data["events"][EventId]["Category"][CategoryId]["rounds"][RoundId];
    
    for(int i=0; i<roundData["questions"].size(); i++)
        Shuffle(roundData["questions"][i]["options"]);
    return roundData;
}

json loadQuestion(const int &EventID, const int &CategoryID, const int &RoundID, const int &QuestionID)
{
    const int RoundId = RoundID-1, EventId = EventID-1, CategoryId = CategoryID-1, QuestionId = QuestionID-1;

    // check if there are events
    if(!data.contains("events"))
        return "events were not found in the JSON";

    if(!data["events"].is_array())
        return "events is not an arrray in the JSON";

    if(EventId < 0 || EventId > (data["events"].size()-1))
        return "Event ID " + std::to_string(EventId) + " does not exist";

    // check if there are categories
    if(!data["events"][EventId].contains("categories"))
        return "categories were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId]["categories"].is_array())
        return "categories in in event ID " + std::to_string(EventId) + " is not an array";

    if(CategoryId < 0 || CategoryId > (data["events"][EventId]["categories"].size()-1))
        return "Category " + std::to_string(CategoryId) + " does not exist in event " + std::to_string(EventId);
    // check if there are rounds
    if(!data["events"][EventId]["categories"][CategoryId].contains("rounds"))
        return "rounds were not found in event ID " + std::to_string(EventId);

    if(!data["events"][EventId]["categories"][CategoryId]["rounds"].is_array())
        return "rounds in event ID " + std::to_string(EventId) + " is not an array";

    if(RoundId < 0 || RoundId > (data["events"][EventId]["categories"][CategoryId]["rounds"].size()-1))
        return "Round ID " + std::to_string(RoundId) + " does not exist in event " + std::to_string(EventId);

    // check if there are questions
    if(!data["events"][EventId]["categories"][CategoryId]["rounds"][RoundId].contains("questions"))
        return "questions were not found in the given IDs";

    if(!data["events"][EventId]["categories"][CategoryId]["rounds"][RoundId]["questions"].is_array())
        return "questions is not an array in the given IDs";

    if(QuestionId < 0 || QuestionId > (data["events"][EventId]["rounds"][RoundId]["categories"][CategoryId]["questions"].size()-1))
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    json question = data["events"][EventId]["categories"][CategoryId]["rounds"][RoundId]["questions"][QuestionId];
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

        function("AddNewQuestion", &addQuestion);
        function("UpdateQuestion", &updateQuestion);
        function("DeleteQuestion", &deleteQuestion);

        function("LoadRoundData", &loadRoundData);
        function("ShowRounds", &showRounds);
        function("LoadQuestionData", &loadQuestion);

        function("initialize", &initialize);

    value_object<Question>("Question")
        .field("id", &Question::id)
        .field("title", &Question::title)
        .field("options", &Question::options)
        .field("correctAnswerIndex", &Question::correctAnswerIndex)
        .field("correctAnswer", &Question::correctAnswer);
}