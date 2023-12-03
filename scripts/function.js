
var urlToLoad = "http://localhost/quiz/quiz.json";
var urlToSave = "http://localhost/quiz/write.php";
   
    function openForm(formId) {
        document.getElementById(formId).style.display = "block";
        document.getElementById("overlay").style.display = "block";

        if (formId === 'loadRoundsForm') {
            ShowRounds();
        }
        if (formId === 'loadJsonServerForm') {
            loadJsonServer();
        }
    }

    function closeForm(formId) {
        document.getElementById(formId).style.display = "none";
        document.getElementById("overlay").style.display = "none";
    }
    function showToast(message) {
        var toastContainer = document.getElementById("toast-container");
        toastContainer.innerHTML = message;
        toastContainer.style.display = "block";

        setTimeout(function() {
            toastContainer.style.display = "none";
        }, 3000);
    }
    

class Question
{
    constructor(id, title, options, correctAnswerIndex, correctAnswer, type, path)
    {
        this.id = id;
        this.title = title;
        this.options = options;
        this.correctAnswerIndex = correctAnswerIndex;
        this.correctAnswer = correctAnswer;
        this.type = type;
        this.path = path;
    }
}

// Alternatively, you can still use Module.onRuntimeInitialized if needed
Module.onRuntimeInitialized = function() {
    // This function will be called once Emscripten is initialized
    console.log('Emscripten initialized');
    var msg = Module.initialize(urlToLoad,urlToSave);
    console.log(msg);
};

//add Event function
function addEvent()
{
    try
    {
        var eventName = document.getElementById("addEvent_EventName").value;
        var eventID = document.getElementById("addEvent_EventId").value;

        if(!eventName.trim())
        {
            alert("please enter a valid name");
            return false;
        }

        var message = Module.AddNewEvent(eventName, parseInt(eventID));
        showToast(message);
        console.log(message);


        return false;
    }
    catch(error)
    {
        showToast(error);
        console.log(error);
        return false;
    }
}

//update Event function
function updateEvent()
{
    try
    {
        var eventID = document.getElementById("updateEvent_EventId").value;
        var name = document.getElementById("updateEvent_EventName").value;
        console.log(eventID);
        var message = Module.UpdateEventName(name, parseInt(eventID));
        showToast(message);
        console.log(message);

        return false;
    }
    catch(error)
    {
        showToast(error);
        console.log(error);
        return false;
    }
}

//delete Event
function deleteEvent()
{
    try
    {
        var eventID = document.getElementById("deleteEvent_EventId").value;

        var message = Module.DeleteEvent(parseInt(eventID));
        showToast(message);
        console.log(message);

        return false;
    }
    catch(error)
    {
        showToast(error);
        console.log(error);
        return false;
    }
}

//Add Category
function addCategory()
{
    try
    {
        var categoryName = document.getElementById("addCategory_CategoryName").value;
        var categoryID = document.getElementById("addCategory_CategoryId").value;
        var eventID = document.getElementById("addCategory_EventId").value;

        if(!categoryName.trim())
        {
            alert("please enter a valid name");
            return false;
        }

        var message = Module.AddNewCategory(categoryName, parseInt(eventID), parseInt(categoryID));
        showToast(message);
        console.log(message);


        return false;
    }
    catch(error)
    {
        showToast(error);
        console.log(error);
        return false;
    }
}


//update Category
function updateCategory()
{
    try
    {
        var categoryName = document.getElementById("updateCategory_CategoryName").value;
        var categoryID = document.getElementById("updateCategory_CategoryId").value;
        var eventID = document.getElementById("updateCategory_EventId").value;

        if(!categoryName.trim())
        {
            alert("please enter a valid name");
            return false;
        }

        var message = Module.UpdateCategoryName(categoryName, parseInt(eventID), parseInt(categoryID));
        
        showToast(message);
        console.log(message);


        return false;
    }
    catch(error)
    {
        showToast(error);
        console.log(error);
        return false;
    }
}

//delete Category
function deleteCategory()
{
    try
    {
        var categoryID = document.getElementById("deleteCategory_CategoryId").value;
        var eventID = document.getElementById("deleteCategory_EventId").value;

        var message = Module.DeleteCategory(parseInt(eventID), parseInt(categoryID));
        showToast(message);
        console.log(message);
        return false;
    }
    catch(error)
    {
        showToast(error);
        console.log(error);
        return false;
    }
}

//Add Round
function addRound() {
    try {
        var roundName = document.getElementById("addRound_Title").value; 
        var eventId = document.getElementById("addRound_EventId").value;
        var categoryId = document.getElementById("addRound_CategoryId").value;
        var roundId = document.getElementById("addRound_RoundId").value;
       
        if (!roundName.trim()) {
            alert("Please enter a valid round name.");
            return false;
        }
        var message = Module.AddNewRound(roundName, parseInt(eventId), parseInt(categoryId), parseInt(roundId));
        showToast(message);
        console.log(message);   
        return false;
        
    } catch (error) {
        showToast(error);
        console.log(error);
        return false;
    }   
}

//Update Round
function updateRound() {
    try {
        var eventId = document.getElementById("updateRound_EventId").value;
        var categoryId = document.getElementById("updateRound_CategoryId").value;
        var roundId = document.getElementById("updateRound_RoundId").value;
        var name = document.getElementById("updateRound_NewRoundName").value;
        if(!name.trim())
        {
            alert("please enter a valid name");
            return false;
        }

        var message = Module.UpdateRoundName(name, parseInt(eventId), parseInt(categoryId), parseInt(roundId));
        showToast(message);
        console.log(message);
        return false;
    } catch(error) {
        showToast(error);
        console.log(error);
        return false;
    }
}

//Delete Round
function deleteRound() {
    try {
        var eventId = document.getElementById("deleteRound_EventId").value;
        var categoryId = document.getElementById("deleteRound_CategoryId").value;
        var roundId = document.getElementById("deleteRound_RoundId").value;

        var message = Module.DeleteRound(parseInt(eventId), parseInt(categoryId), parseInt(roundId));
        showToast(message);
        console.log(message);

        return false;
    } catch(error) {
        showToast(error);
        console.log(error);
        return false;
    }
}

//Add Question
function addQuestion() {
    try {
        var roundID = document.getElementById("addQuestion_RoundId").value;
        var categoryID = document.getElementById("addQuestion_CategoryId").value;
        var eventID = document.getElementById("addQuestion_EventId").value;

        var title = document.getElementById("addQuestion_Title").value;
       
        var optionsString = document.getElementById("addQuestion_Options").value;
        var options = optionsString.split(',');

        var optionVectorEmcc = new Module.VectorString();  // Allocates std::vector<std::string> which is managed by JS
        // std::string and JavaScript strings are automatically interconverted
       
        options.forEach(option => {
            optionVectorEmcc.push_back(option);
        });

        var correctAnswer = document.getElementById("addQuestion_CorrectAnswer").value;
        var correctAnswerIndex = document.getElementById("addQuestion_CorrectAnswerIndex").value;
        var type = "text"; // needs a field
        var path = "path"; //needs a field

        var question = new Question(0, title, optionVectorEmcc,parseInt(correctAnswerIndex), correctAnswer, type, path);
        var message = Module.AddNewQuestion(parseInt(eventID), parseInt(categoryID), parseInt(roundID), question);
        showToast(message);
        console.log(message);

        return false;

    } catch (error) {
        showToast(error);
        console.log(error);
        return false;
    }
}

//Update Question
function updateQuestion() {
    try {
        var roundID = document.getElementById("updateQuestion_EventId").value;
        var categoryID = document.getElementById("updateQuestion_CategoryId").value;
        var eventID = document.getElementById("updateQuestion_RoundId").value;
        var questionID = document.getElementById("updateQuestion_QuestionId").value;

        var name = document.getElementById("updateQuestion_name").value;
        if(!name.trim())
        {
            alert("please enter a valid name");
            return false;
        }

        var message = Module.UpdateQuestion(name, parseInt(eventID), parseInt(categoryID), parseInt(roundID), parseInt(questionID));
        showToast(message);
        console.log(message);

        return false;

    } catch (error) {
        showToast(error);
        console.log(error);
        return false;
    }
}


//Delete Question
function deleteQuestion() {
    try {
        var eventID = document.getElementById("deleteQuestion_EventId").value;
        var roundID = document.getElementById("deleteQuestion_RoundId").value;
        var categoryID = document.getElementById("deleteQuestion_CategoryId").value;
        var questionID = document.getElementById("deleteQuestion_QuestionId").value;

        var message = Module.DeleteQuestion(parseInt(eventID), parseInt(categoryID), parseInt(roundID), parseInt(questionID));
        showToast(message);
        console.log(message);
        return false;

    } catch (error) {
        showToast(error);
        console.log(error);
        return false;
    }
}

//Load Round data
function loadRound(){

    try{
        var eventID = document.getElementById("loadRound_EventId").value;
        var roundID = document.getElementById("loadRound_RoundId").value;
        var categoryID = document.getElementById("loadRound_CategoryId").value;

        var message = Module.LoadRoundData(parseInt(eventID), parseInt(categoryID), parseInt(roundID));
        showToast(message);
        console.log(message);
        return false;
    }
    catch(error){
        showToast(error);
        console.log(error);
        return false;
    }
}

//Load a Question
function loadQuestion(){
    try {
        var eventID = document.getElementById("loadQuestion_EventId").value;
        var categoryID = document.getElementById("loadQuestion_CategoryId").value;
        var roundID = document.getElementById("loadQuestion_RoundId").value;
        var questionID = document.getElementById("loadQuestion_QuestionId").value;

        var message = Module.LoadQuestionData(parseInt(eventID), parseInt(categoryID), parseInt(roundID), parseInt(questionID));

        showToast(message);
        console.log(message);
        return false;
        
    } catch (error) {
        showToast(error);
        console.log(error);
        return false; 
    }
}