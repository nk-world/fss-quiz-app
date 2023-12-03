
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

function addEvent()
{
    try
    {
        var eventName = document.getElementById("eventName0").value;
        var eventID = document.getElementById("eventID0").value;

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

function updateEvent()
{
    try
    {
        var eventID = document.getElementById("eventID1").value;
        var name = document.getElementById("eventName1").value;

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

function deleteEvent()
{
    try
    {
        var eventID = document.getElementById("eventID2").value;

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


function addCategory()
{
    try
    {
        var categoryName = document.getElementById("categoryName_c0").value;
        var categoryID = document.getElementById("categoryID_c0").value;
        var eventID = document.getElementById("eventID_c0").value;

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

function updateCategory()
{
    try
    {
        var categoryName = document.getElementById("categoryName_c1").value;
        var categoryID = document.getElementById("categoryID_c1").value;
        var eventID = document.getElementById("eventID_c1").value;

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

function deleteCategory()
{
    try
    {
        var categoryID = document.getElementById("categoryID_c2").value;
        var eventID = document.getElementById("eventID_c2").value;

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


function addRound() {
    try {
        var roundName = document.getElementById("addRoundTitle_r0").value; 
        var eventId = document.getElementById("eventId_r0").value;
        var categoryId = document.getElementById("categoryID_r0").value;
        var roundId = document.getElementById("roundID_r0").value;
       
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

function updateRound() {
    try {
        var eventId = document.getElementById("eventID_r1").value;
        var categoryId = document.getElementById("categoryID_r1").value;
        var roundId = document.getElementById("RoundID_r1").value;
        var name = document.getElementById("newRoundName_r1").value;

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

function deleteRound() {
    try {
        var eventId = document.getElementById("eventID_r2").value;
        var categoryId = document.getElementById("categoryID_r2").value;
        var roundId = document.getElementById("RoundID_r2").value;

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



    


/* function addQuestion() {
    try {
        var roundID = document.getElementById("roundID_q0").value;
        var categoryID = document.getElementById("categoryID_q0").value;
        var eventID = document.getElementById("eventID_q0").value;

        var title = document.getElementById("title_q0").value;
        var options = document.getElementById("options_q0").value;
        var correctAnswer = document.getElementById("correctAnswer_q0").value;
        var correctAnswerIndex = document.getElementById("correctAnswerIndex_q0").value;

        var question = new Question(0, title, options, parseInt(correctAnswerIndex), correctAnswer);


        var message = Module.AddNewQuestion(parseInt(eventID), parseInt(categoryID), parseInt(roundID), question);
        showToast(message);
        console.log(message);

        return false;

    } catch (error) {
        showToast(error);
        console.log(error);
        return false;
    }
} */

function addQuestion() {
    try {
        var roundID = document.getElementById("roundID_q0").value;
        var categoryID = document.getElementById("categoryID_q0").value;
        var eventID = document.getElementById("eventID_q0").value;

        var title = document.getElementById("title_q0").value;
       
        var optionsString = document.getElementById("options_q0").value;
        var options = optionsString.split(',');

        var optionVectorEmcc = new Module.VectorString();  // Allocates std::vector<std::string> which is managed by JS
        // std::string and JavaScript strings are automatically interconverted
       
        options.forEach(option => {
            optionVectorEmcc.push_back(option);
        });

        var correctAnswer = document.getElementById("correctAnswer_q0").value;
        var correctAnswerIndex = document.getElementById("correctAnswerIndex_q0").value;
        var type = "text"; // needs a field
        var path = "path"; //needs a field
        
        var question = new Question(0, parseInt(0), title, optionVectorEmcc, parseInt(correctAnswerIndex), correctAnswer, type, path);
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

function updateQuestion() {
    try {
        var roundID = document.getElementById("roundID_q1").value;
        var categoryID = document.getElementById("categoryID_q1").value;
        var eventID = document.getElementById("eventID_q1").value;
        var questionID = document.getElementById("questionID_q1").value;

        var name = document.getElementById("name_q1").value;

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

function deleteQuestion() {
    try {
        var roundID = document.getElementById("roundID_q2").value;
        var categoryID = document.getElementById("categoryID_q2").value;
        var eventID = document.getElementById("eventID_q2").value;
        var questionID = document.getElementById("questionID_q2").value;

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



/* function ShowRounds() {
    try {
        var roundsData = Module.ShowRounds(urlToLoad);

        // Convert the VectorString object to a JavaScript array
        var rounds = [];
        var size = roundsData.size();
        for (var i = 0; i < size; ++i) {
            rounds.push(roundsData.get(i));
        }

        if (rounds.length > 0) {
            var buttonsContainer = document.getElementById("roundButtonsContainer");
            buttonsContainer.innerHTML = "";

            rounds.forEach(function (round) {
                var button = document.createElement("button");
                button.className = "round-button";
                button.innerText = round;
                button.onclick = function () {
                    showToast(`loading ${round} round.`);
                };
                buttonsContainer.appendChild(button);
            });
        } else {
            closeForm('loadRoundsForm');
            showToast("No rounds to show. Add new ones.");
            console.log("No rounds to show. Add new ones.");
        }
    } catch (error) {
        showToast(error);
        console.log(error);
        return false;
    }
} */