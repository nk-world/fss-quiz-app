#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::ordered_json;

struct Question
{
    int id;
    std::string question;
    std::vector<std::string> options;
    int correctAnswerIndex;
    std::string correctAnswer;
};

struct Round
{
    string name;
    int id;
    vector<Question> Questions;
    bool hasOptions;
};

struct Event
{
    string name;
    int id;
    vector<Round> rounds;
};

void addEvent();
void renameEvent(int EventIndex); // by Nischal (under construction)
void addRound(int EventIndex); // by Nischal
void renameRound(int EventIndex); // by Samyog
void deleteRound(int EventIndex); // by Samyog
void addQuestion(int EventIndex, int index); // by Nischal
void updateQuestion(int EventIndex); // by Vinayak (under construction)
void deleteQuestion(int EventIndex); // by Abhishek
void updateOption(int EventIndex); // by Nischal
void printAllEvents(int EventIndex); // by Nischal (under construction)
void printAll(int EventIndex); // by Nischal
void printRound(int EventIndex); // by Prince (under construction)
void loadFile(); // by Nischal
void writeFile(); // by Nischal

string filename;
vector<Round> rounds;
vector<Event> events;

int EventIndex;
int main()
{
    
    cout << "Enter the filename to work on: ";
    getline(cin, filename);
    loadFile();

    char run_again;
    do
    {
        int choice;
        cout << endl << endl;
        cout << "Fluorescent Secondary School Quiz Application" << endl;
        cout << endl << "Choose what to do" << endl << endl;
        cout << "Round Edits: " << endl;
        cout << "\t1. Create a new round" << endl;
        if(!rounds.empty())
        {
            cout << "\t2. Rename a round" << endl;
            cout << "\t3. Delete a round" << endl;

            cout << endl << "Question Edits: " << endl;
            cout << "\t4. Add question to an existing round" << endl;
            cout << "\t5. Change a specific question of a specific round (under construction)" << endl;
            cout << "\t6. Delete question from an existing round" << endl;

            cout << endl << "Option Edit:" << endl;
            cout << "\t7. Change a specific option from a specific question of a specific round" << endl;

            cout << endl << "Data display:" << endl;
            cout << "\t8. Display all the informations" << endl;
            cout << "\t9. Display informations of a specific round (under construction)" << endl;
        }
        cout << endl << "10. Exit" << endl;

        cout << endl << endl;
        cout << "Enter your choice in number: ";
        cin >> choice;
        cin.ignore();

        if(rounds.empty() && (choice !=1 || choice != 10))
        {
            cout << "The specified file is empty, you can only create a new round.";
            return 1;
        }

        // defining variables to use inside switch cases
        string roundname; 
        bool roundExists = false; 
        int rounds_size = rounds.size();
        int index;

        switch(choice)
        {
            case 1:
                while(true)
                {
                    cout << endl << endl;
                    addRound(EventIndex);
                    cout << "Would you also like to add another round? (y for yes, anything for no): ";
                    char yn_confirm;
                    cin >> yn_confirm;
                    yn_confirm = tolower(yn_confirm);
                    cin.ignore();
                    if(yn_confirm != 'y')
                        break;
                }
                break;

            case 2:
                renameRound(EventIndex);
                break;

            case 3:
                deleteRound(EventIndex);
                break;

            case 4:
                for (int i=0; i<rounds_size; i++)
                {
                    cout << i+1 << ". " << rounds[i].name << endl;
                }

                while(true)
                {
                    cout << endl << "Enter the index of round to add question/s: ";
                    cin >> index;

                    if(index < 1 || index > rounds_size)
                        cout << "Invalid index!" << endl;
                    else
                        break;
                }

                cin.ignore();
                addQuestion(EventIndex, index-1);
                break;

            case 5:
                cout << "This functionality is currently under construction.";
                break;

            case 6:
                deleteQuestion(EventIndex);
                break;

            case 7:
                updateOption(EventIndex);
                break;
            
            case 8:
                printAll(EventIndex);
                break;

            case 9:
                cout << "This functionality is currently under construction.";
                break;

            case 10:
                return 0;
                break;
            
            default:
                cout << "Invalid choice. Please select a number from 1 to 9. Select 10 to exit." << endl;
                break;
        }
        writeFile();
        cout << endl << "Action complete!" << endl;

        cout << "Would you like to perform any more operations? (y for yes, anything for no): ";
        cin >> run_again;
        cout << endl << endl << endl << endl;
        run_again = tolower(run_again);

    } while (run_again == 'y');


    return 0;
}



void addEvent()
{
    Event event;
    cout << "Enter the name of the event: ";
    cin >> event.name;

    event.id = events.size() + 1;

    addRound(EventIndex);
}

void addRound(int EventIndex)
{
    vector<Round> &rounds = events[EventIndex].rounds;
    Round round;
    char yesno;
    cout << "Enter the name of the round: ";
    getline(cin, round.name);

    int i=0;
    while(!(yesno == 'y' || yesno == 'n'))
    {
        if(i != 0)
            cout << "Please enter \'y\' or \'n\'." << endl;
        else
            i++;
        cout << "Does the round have options? (y for yes/n for no): ";
        cin >> yesno;
        yesno = tolower(yesno);
    }
    
    round.hasOptions = (yesno == 'y');
    
    
    cin.ignore();
    rounds.push_back(round);
    addQuestion(EventIndex, rounds.size()-1);
}

void deleteRound(int EventIndex)
{
    int rounds_size = rounds.size(), index;
    char confirmation;
    cout << endl << endl;
    for(int i=0; i<rounds_size; i++)
        cout << i+1 << ". " << rounds[i].name << endl;

    while(true)
    {
        cout << endl << "Enter the index of the round to delete: ";
        cin >> index;
        if(index < 1 || index > rounds_size)
            cout << "Invalid index." << endl;
        else
            break;
    }

    --index;
    cout << "Are you sure you want to permanently delete this round?" << endl;
    cout << "Enter \'y\' to confirm, any other character to abort: ";
    cin >> confirmation;
    confirmation = tolower(confirmation);

    if(confirmation == 'y')
        rounds.erase(rounds.begin()+index);
}

void renameRound(int EventIndex)
{
    int rounds_size = rounds.size(), index;
    char confirmation;
    string new_name;
    cout << endl << endl;
    for(int i=0; i<rounds_size; i++)
        cout << i+1 << ". " << rounds[i].name << endl;

    while(true)
    {
        cout << endl << "Enter the index of the round to rename: ";
        cin >> index;
        if(index < 1 || index > rounds_size)
            cout << "Invalid index." << endl;
        else
            break;
    }
    cin.ignore();

    --index;
    cout << "Enter the new name for this round: ";
    getline(cin, new_name);

    rounds[index].name = new_name;
}

void addQuestion(int EventIndex, int index)
{
    Round round = rounds[index];
    Question question;
    int i = round.Questions.size() + 1;
    char yesno;
    // cout << i << endl << endl;

    while(yesno != 'n')
    {
        cout << "Question number " << i << " for round " << round.name << endl;
        cout << "Enter the question: ";
        getline(cin, question.question);
        question.id = i;
        if (round.hasOptions)
        {
            cout << "How many options are there? ";
            int options_count;
            cin >> options_count;
            cin.ignore();
            for (int a = 0; a < options_count; a++)
            {
                string option;
                cout << "Enter option number " << a+1 << ": ";
                getline(cin, option);
                question.options.push_back(option);
            }
            while(true)
            {
                cout << "Enter the correct option number: ";
                cin >> question.correctAnswerIndex;
                if (question.correctAnswerIndex <= question.options.size())
                    break;
                else
                    cout << "Error: the option number " << question.correctAnswerIndex << " is not avaible.\n";
            }
            question.correctAnswer = question.options[question.correctAnswerIndex - 1];
        }
        else
        {
            cout << "Enter the correct answer: ";
            getline(cin, question.correctAnswer);
        }

        round.Questions.push_back(question);
        question.options.clear();
        i++;

        cout << endl << endl;
        int j=0;
        yesno = 'a'; // ruined the value of yesno so that the below loop runs again
        while(!(yesno == 'n' || yesno == 'y'))
        {
            if (j != 0)
                cout << endl << "Please enter \'y\' or \'n\'" << endl;
            cout << "Add another question? (y for yes / n for no): ";
            cin >> yesno;
            yesno = tolower(yesno);
            j++;
        }
        cin.ignore();
    }
    rounds[index] = round;
}

void deleteQuestion(int EventIndex)
{
    int rounds_size = rounds.size(), round_index;
    cout << endl << endl;
    cout << "Rounds:" << endl;
    for(int i=0; i<rounds_size; i++)
      cout << i+1 << ". " << rounds[i].name << endl;

    while(true)
    {
      cout << endl << "Choose a round to delete it's question: ";
      cin >> round_index;
      if(round_index < 1 || round_index > rounds_size)
          cout << "Invalid selection. Please try again." << endl;
      else
          break;
    }

    --round_index;

    int questions_size = rounds[round_index].Questions.size(), question_index;
    char delete_confirmation;

    cout<< endl;

    cout << "Questions:" << endl;
    for(int i=0; i<questions_size; i++)
      cout << rounds[round_index].Questions[i].id << ". " << rounds[round_index].Questions[i].question << endl;

    while(true)
    {
      cout << endl << "Choose a question number to delete: ";
      cin >> question_index;
      if(question_index < 1 || question_index > questions_size)
          cout << "Invalid selection. Please try again." << endl;
      else
          break;
    }
    --question_index;

    cout << endl;
    cout << "Are you sure you want to permanently delete this question?" << endl;
    cout << "Enter \'y\' to confirm, any other character to abort: ";
    cin >> delete_confirmation;
    delete_confirmation = tolower(delete_confirmation);

    if(delete_confirmation == 'y')
      rounds[round_index].Questions.erase(rounds[round_index].Questions.begin()+question_index);
}

void updateOption(int EventIndex)
{
    int i=1, round_index, question_index, option_index, rounds_size=rounds.size();
    bool present=false;
    string new_option;
    for(auto name : rounds)
    {
        cout << i << ". " << name.name << endl;
        i++;
    }

    cout << endl << "Select the index of round to edit: ";

    try
    {
        cin >> round_index;
    }
    catch(exception& e)
    {
        cerr << "Please enter the index in number.";
        return;
    }

    if(round_index < 1 || round_index > rounds_size)
    {
        cout << "Invalid Index.";
        return;
    }
    --round_index;

    if(!rounds[round_index].hasOptions)
    {
        cout << "The round you selected does not possess any options!";
        return;
    }

    int questions_size = rounds[round_index].Questions.size();
    cout << endl << endl;
    for(auto question : rounds[round_index].Questions)
    {
        cout << question.id << ". " << question.question << endl;
    }

    cout << endl << "Select the question index to edit: ";
    cin >> question_index;

    if (question_index < 1 || question_index > questions_size)
    {
        cout << "Invalid index.";
        return;
    }

    --question_index;

    int options_size = rounds[round_index].Questions[question_index].options.size();
    i=1;
    cout << endl << endl;
    for(auto option : rounds[round_index].Questions[question_index].options)
    {
        cout << i << ". " << option << endl;
        i++;
    }

    cout << endl << "Enter the index of option to edit.";
    cin >> option_index;

    if(option_index < 1 || option_index > options_size)
    {
        cout << "Invalid index.";
        return;
    }

    --option_index;
    cin.ignore();

    cout << endl << endl << endl;
    cout << "Enter the new option in that place: ";
    getline(cin, new_option);

    rounds[round_index].Questions[question_index].options[option_index] = new_option;
}

void printAll(int EventIndex)
{
    vector<Round> rounds = events[EventIndex].rounds;
    int rounds_size = rounds.size();
    for(int i=0; i<rounds_size; i++)
    {
        cout << "For round " << i+1 << endl;
        cout << "Name: " << rounds[i].name << endl;
        cout << "Questions: " << endl;

        int questions_size = rounds[i].Questions.size();
        for (int j=0; j<questions_size; j++)
        {
            cout << endl << "Question number " << j+1 << endl;
            cout << "Question: " << rounds[i].Questions[j].question << endl;
            if(rounds[i].hasOptions)
            {
                int options_size = rounds[i].Questions[j].options.size();
                for (int k=0; k<options_size; k++)
                    cout << "Option " << k+1 << ": " << rounds[i].Questions[j].options[k] << endl;
            }
            cout << "Correct Answer: " << rounds[i].Questions[j].correctAnswer << endl;
        }
        cout << endl << endl;
    }
}

void loadFile()
{
    Event event;
    Round round;
    Question question;
    json main_object = json::object();

    ifstream getFile(filename);
    if(getFile)
    {
        getFile >> main_object;
    }
    else
    {
        cout << "Error: Failed to open file for reading data." << endl;
        return;
    }

    try
    {
        json json_events = main_object["Events"];
        int sizeE = events.size();
        for (int h=0; h<sizeE; h++)
        {
            json event_temp = json_events[h]["Rounds"];
            event.name = event_temp["name"];
            event.id = event_temp["id"];

            int size1 = json_events[h]["Rounds"].size();
            for (int i=0; i<size1; i++)
            {
                json round_temp = event_temp[i];
                round.name = round_temp["name"];
                round.hasOptions = round_temp["hasOptions"];

                int size2 = round_temp["questions"].size();
                for (int j = 0; j<size2; j ++)
                {
                    json question_temp = round_temp["questions"][j];
                    question.question = question_temp["question"];
                    question.correctAnswer = question_temp["correctAnswer"];
                    question.id = question_temp["questionNumber"];

                    if(round.hasOptions)
                    {
                        question.correctAnswerIndex = question_temp["correctAnswerIndex"];

                        int size3 = question_temp["options"].size();
                        for (int k=0; k<size3; k++)
                        {
                            question.options.push_back(question_temp["options"][k]);
                        }
                    }
                    round.Questions.push_back(question);
                    question.options.clear();
                }
                rounds.push_back(round);
                round.Questions.clear();
            }
            events.push_back(event);
            event.rounds.clear();
        }
    }
    catch(exception& e)
    {
        cerr << "There was an error while loading the file. If the file is empty, ignore this error. ";
        cerr << "Otherwise, the file you entered was not made in the format of this application." << endl;
        cerr << e.what();
    }
}

void writeFile()
{
    json main_object = json::object();
    json json_events = main_object["Events"];

    json events_temp = json::array();
    int sizeE = events.size();
    for (int h=0; h<sizeE; h++)
    {
        json event_temp = json::object();
        event_temp["name"] = events[h].name;
        event_temp["id"] = events[h].id;
        
        json rounds_temp = json::array();
        int sz=rounds.size();
        for (int i=0; i<sz; i++)
        {
            json round_temp = json::object();
            round_temp.clear();
            round_temp["name"] = rounds[i].name;
            round_temp["hasOptions"] = rounds[i].hasOptions;
            
            json questions_temp = json::array();
            int ss=rounds[i].Questions.size();
            for (int j=0; j<ss; j++)
            {
                json question_temp = json::object();
                question_temp.clear();
                question_temp["questionNumber"] = j+1;
                question_temp["question"] = rounds[i].Questions[j].question;
                question_temp["correctAnswer"] = rounds[i].Questions[j].correctAnswer;

                if(rounds[i].hasOptions)
                {
                    json options_temp = json::array();
                    for (auto option : rounds[i].Questions[j].options)
                    {
                        options_temp.push_back(option);
                    }
                    question_temp["options"] = options_temp;
                    question_temp["correctAnswerIndex"] = rounds[i].Questions[j].correctAnswerIndex;
                }
                questions_temp.push_back(question_temp);
            }
            round_temp["questions"] = questions_temp;
            rounds_temp.push_back(round_temp);
        }
        event_temp["Rounds"] = rounds_temp;
        events_temp.push_back(event_temp);
    }
    main_object["Events"] = events_temp;

    ofstream saveFile(filename);
    if(saveFile)
    {
        try
        {
            saveFile << main_object;
            cout << "Data saved.";
        }
        catch(exception& e)
        {
            cerr << "There was an error saving the file." << endl;
            cerr << e.what();
        }
    }
    else
    {
        cout << "There was an error trying to open the file to save this information.";
    }
}