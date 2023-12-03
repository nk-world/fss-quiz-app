#include "json.hpp"
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>
#include <emscripten/bind.h>
#include <emscripten/fetch.h>
#include <emscripten/val.h>

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
        return json{
            {"id", id},
            {"title", title},
            {"options", options},
            {"correctAnswerIndex", correctAnswerIndex},
            {"correctAnswer", correctAnswer},
            {"type", type},
            {"path", path}};
    }
};

std::string loadUrl, saveUrl;
json data;
std::string LoadJsonFromServer();
std::string initialize(const std::string &LoadUrl, const std::string &SaveUrl)
{
    data = json::object();
    loadUrl = LoadUrl;
    saveUrl = SaveUrl;
    data = json::parse(LoadJsonFromServer());
    return "initialized";
}

EM_ASYNC_JS(bool, saveDataToServer, (const char* saveUrl, const char* data), {
    try {
        let jsonData = UTF8ToString(data);
        let url = UTF8ToString(saveUrl);
        const response = await fetch(url, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: jsonData,
        });
        
        if (response.ok) {
            out('Data saved successfully!');
            return true;
        } else {
            out('Failed to save data. Status: ' + response.status);
            return false;
        }
    } catch (error) {
        out('Error during data save: ' + error.toString());
        return false;
    }
});


bool SaveDataToServer()
{
   return saveDataToServer(saveUrl.c_str(), data.dump().c_str());
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

void fixQuestionId()
{
    for(int i=0; i<data["events"].size(); i++)
        for(int j=0; j<data["events"][i]["categories"].size(); j++)
            for(int k=0; k<data["events"][i]["categories"][j]["rounds"].size(); k++)
                for(int l=0; l<data["events"][i]["categories"][j]["rounds"][k]["questions"].size(); l++)
                    data["events"][i]["categories"][j]["rounds"][k]["questions"][l] = l+1;
}

void Shuffle(json &jsonData)
{
    std::random_device random;
    std::mt19937 generate(random());

    std::shuffle(jsonData.begin(), jsonData.end(), generate);
}

int FindEventId(int eventId)
{
    int index=0;
    for(auto i: data["events"])
    {
        if(i["id"] == eventId)
            return index;
        else
            index++;
    }
    return -1;
}

int FindCategoryId(int eventId, int categoryId)
{
    int index=0;
    for(auto i: data["events"][eventId]["categories"])
    {
        if(i["id"] == categoryId)
            return index;
        else
            index++;
    }
    return -1;
}

int FindRoundId(int eventId, int categoryId, int roundId)
{

    int index=0;
    for(auto i: data["events"][eventId]["categories"][categoryId]["rounds"])
    {
        if(i["id"] == roundId)
            return index;
        else
            index++;
    }
    return -1;
}

int FindQuestionId(int eventId, int categoryId, int roundId, int questionId)
{
    int index=0;
    for(auto i: data["events"][eventId]["categories"][categoryId]["rounds"][roundId]["questions"])
    {
        if(i["id"] == questionId)
            return index;
        else
            index++;
    }
    return -1;
}

std::string addEvent(std::string name, int EventId)
{
    json event = json::object();

    int eventId = FindEventId(EventId);

    if(!data.empty())
        if(eventId != -1)
            return "Event with ID " + std::to_string(EventId) + " already exists";

    event["name"] = name;
    event["id"] = EventId;

    data["events"].push_back(event);

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Event added successfully";
    else
        return "Adding event failed";
}

std::string updateEvent(std::string NewName, int EventId)
{
    int eventId = FindEventId(EventId);

    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    data["events"][eventId]["name"] = NewName;

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Event name updated successfully";
    else
        return "Event name update failed";
}

std::string deleteEvent(int EventId)
{
    int eventId = FindEventId(EventId);

    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    try
    {
        data["events"].erase(eventId);
    }
    catch (const std::exception &e)
    {
        return e.what();
    }

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Event deleted successfully";
    else
        return "Event deletion failed";
}

std::string addCategory(std::string categoryName, int EventId, int CategoryId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    json category = json::object();

    if (!data.empty())
        if (categoryId != -1)
            return "Category with ID " + std::to_string(CategoryId) + " already exists";

    category["name"] = categoryName;
    category["id"] = CategoryId;
    data["events"][eventId]["categories"].push_back(category);

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Category added successfully";
    else
        return "Adding category failed";
}

std::string updateCategory(std::string newName, int EventId, int CategoryId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
   
    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    if (categoryId == -1)
        return "Category with ID " + std::to_string(CategoryId) + " does not exist";

    data["events"][eventId ]["categories"][categoryId]["name"] = newName;

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Category updated successfully";
    else
        return "Updating category failed";
}

std::string deleteCategory(int EventId, int CategoryId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
   
    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    if (categoryId == -1)
        return "Category with ID " + std::to_string(CategoryId) + " does not exist";

    data["events"][eventId]["categories"].erase(categoryId);

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Category deleted successfully";
    else
        return "Deleting category failed";
}

std::string addRound(std::string name, int EventId, int CategoryId, int RoundId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);

    if (!data.empty())
        if (roundId != -1)
            return "Round with ID " + std::to_string(RoundId) + " already exists";

    json round = json::object();
    round["name"] = name;
    round["id"] = RoundId;

    data["events"][eventId ]["categories"][categoryId ]["rounds"].push_back(round);

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Round added successfully";
    else
        return "Adding round failed";
}

std::string updateRound(std::string NewName, int EventId, int CategoryId, int RoundId)
{
    int eventId = EventId - 1;
    int categoryId = CategoryId - 1;
    int roundId = RoundId -1;

    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    if (categoryId == -1)
        return "Category with ID " + std::to_string(CategoryId) + " does not exist";

    if (roundId == -1)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    data["events"][eventId]["categories"][categoryId]["rounds"][roundId]["name"] = NewName;

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Round name updated successfully";
    else
        return "Round name update failed";
}

std::string deleteRound(int EventId, int CategoryId, int RoundId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);

    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    if (categoryId == -1)
        return "Category with ID " + std::to_string(CategoryId) + " does not exist";

    if (roundId == -1)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    data["events"][eventId]["categories"][categoryId]["rounds"].erase(roundId);

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Round deleted successfully";
    else
        return "Round deletion failed";
}

std::string addQuestion(int EventId, int CategoryId, int RoundId, Question question)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);
    
    data["events"][eventId]["categories"][categoryId]["rounds"][roundId]["questions"].push_back(question.toJson());
    int size = data["events"][eventId]["categories"][categoryId]["rounds"][roundId]["questions"].size();
    data["events"][eventId]["categories"][categoryId]["rounds"][roundId]["questions"][size-1]["id"] = size;

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Question added successfully";
    else
        return "Adding question failed";
}

std::string updateQuestion(std::string newName, int EventId, int CategoryId, int RoundId, int QuestionId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);
    int questionId = FindQuestionId(eventId, categoryId, roundId, QuestionId);

    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    if (categoryId == -1)
        return "Category with ID " + std::to_string(CategoryId) + " does not exist";

    if (roundId == -1)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    if (questionId == -1)
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    data["events"][eventId]["categories"][categoryId]["rounds"][roundId]["questions"][questionId]["title"] = newName;

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Question renamed successfully";
    else
        return "Renaming question failed";
}

std::string deleteQuestion(int EventId, int CategoryId, int RoundId, int QuestionId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);
    int questionId = FindQuestionId(eventId, categoryId, roundId, QuestionId);
  
    if (eventId == -1)
        return "Event with ID " + std::to_string(EventId) + " does not exist.";

    if (categoryId == -1)
        return "Category with ID " + std::to_string(CategoryId) + " does not exist";

    if (roundId == -1)
        return "Round with ID " + std::to_string(RoundId) + " was not found";

    if (questionId == -1)
        return "Question with ID " + std::to_string(QuestionId) + " was not found";

    data["events"][eventId]["categories"][categoryId ]["rounds"][roundId]["questions"].erase(questionId);
    fixQuestionId();

    bool isSuccess = SaveDataToServer();
    if (isSuccess)
        return "Question deleted successfully";
    else
        return "Deleting question failed";
}

/* no need as per now
std::vector<std::string> showRounds(const int &EventID, const int &CategoryID)
{
    const int eventId = EventID - 1, categoryId = CategoryID - 1;
    std::vector<std::string> rounds;

    for (auto round : data["events"][eventId]["categories"][categoryId]["rounds"])
        rounds.push_back(round["name"]);

    return rounds;
}
*/

val loadRoundData(int EventId, int CategoryId, int RoundId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);

    if (eventId == -1)
        return val("Event with ID " + std::to_string(EventId) + " does not exist.");

    if (categoryId == -1)
        return val("Category with ID " + std::to_string(CategoryId) + " does not exist");

    if (roundId == -1)
        return val("Round with ID " + std::to_string(RoundId) + " was not found");

    json roundData = data["events"][EventId]["categories"][CategoryId]["rounds"][RoundId];

    for (int i = 0; i < roundData["questions"].size(); i++)
        Shuffle(roundData["questions"][i]["options"]);

    return val(roundData.dump());
}

val loadQuestion(int EventId, int CategoryId, int RoundId, int QuestionId)
{
    int eventId = FindEventId(EventId);
    int categoryId = FindCategoryId(eventId, CategoryId);
    int roundId = FindRoundId(eventId, categoryId, RoundId);
    int questionId = FindQuestionId(eventId, categoryId, roundId, QuestionId);

    if (eventId == -1)
        return val("Event with ID " + std::to_string(EventId) + " does not exist.");

    if (categoryId == -1)
        return val("Category with ID " + std::to_string(CategoryId) + " does not exist");

    if (roundId == -1)
        return val("Round with ID " + std::to_string(RoundId) + " was not found");

    if (questionId == -1)
        return val("Question with ID " + std::to_string(QuestionId) + " was not found");

    json question = data["events"][EventId]["categories"][CategoryId]["rounds"][RoundId]["questions"][QuestionId];
    Shuffle(question["options"]);
    return val(question.dump());
}

EMSCRIPTEN_BINDINGS(my_module)
{
    register_vector<std::string>("VectorString");
    value_object<Question>("Question")
        .field("id", &Question::id)
        .field("title", &Question::title)
        .field("options", &Question::options)
        .field("correctAnswerIndex", &Question::correctAnswerIndex)
        .field("correctAnswer", &Question::correctAnswer)
        .field("type", &Question::type)
        .field("path", &Question::path);

    function("initialize", &initialize);
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
    //function("ShowRounds", &showRounds); // no need as per now
    function("LoadQuestionData", &loadQuestion);
}