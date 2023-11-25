
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
    constructor(id, title, options, correctAnswer, correctAnswerIndex)
    {
        this.id = id;
        this.title = title;
        this.options = options;
        this.correctAnswer = correctAnswer;
        this.correctAnswerIndex = correctAnswerIndex;
    }
}

// Alternatively, you can still use Module.onRuntimeInitialized if needed
    Module.onRuntimeInitialized = function() {
        // This function will be called once Emscripten is initialized
        console.log('Emscripten initialized');
    };

/* function initialize() {
    try {
        var message = Module.initialize(urlToLoad, urlToSave);
        showToast(message);
        console.log(message);
    } catch(error) {
        showToast(error);
        console.log(error);
        return false;
    }
} */

 function addRound() {
    try {
        var roundName = document.getElementById("addRoundTitle").value; 
        var eventId = document.getElementById("eventId").value;
       
        if (!roundName.trim()) {
            alert("Please enter a valid round name.");
            return false;
        }
        Module.initialize(urlToLoad, urlToSave);
        var message = Module.AddNewRound(roundName, parseInt(eventId));
        showToast(roundName + " added.");
        console.log(message);   

        closeForm('addRoundForm');
        return false;
        
    } catch (error) {
        showToast(error);
         console.log(error);
        return false;
    }
        
}

    function deleteRound() {
        try {
            var roundName = document.getElementById("roundToDelete").value; 
       
        if (!roundName.trim()) {
            alert("Please enter a valid round name.");
            return false;
        }
        
        var message = Module.DeleteRound(urlToLoad, urlToSave ,roundName);
        showToast(message);
        console.log(message);
        
        closeForm('deleteRoundForm');
        return false;
        } catch (error) {
            showToast(error); 
             console.log(error);
            return false;
        }
        
    }

    function updateRoundName() {
        try {
            var oldName = document.getElementById("oldRoundName").value; 
        var newName = document.getElementById("newRoundName").value; 
        
        if (!oldName.trim() || !newName.trim()) {
            alert("Please enter a valid round name.");
            return false;
        }
        var message = Module.UpdateRoundName(urlToLoad, urlToSave , oldName, newName);
        showToast(message);
        console.log(message);
        
        closeForm('updateRoundForm');
        return false;
        } catch (error) {
            showToast(error);
             console.log(error);
            return false;
        }
        
    }

    function ShowRounds() {
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
}


   function addQuestion() {
    try {
        document.getElementById("addQuestionForm").addEventListener("submit", function(event) {
            event.preventDefault();

            // Access the form values
            var roundName = document.getElementById("roundName").value;
            var title = document.getElementById("title").value;
            var options = document.getElementById("options").value;
            var correctAnswer = document.getElementById("correctAnswer").value;
            var correctAnswerIndex = document.getElementById("correctAnswerIndex").value;

            var question = new Question(0, title, options, correctAnswer, correctAnswerIndex);
            console.log("Question Title:", question.title);

            if (!roundName.trim() || !title.trim() || !options.trim() || !correctAnswer.trim() || correctAnswerIndex === "") {
                alert("Please fill in all fields.");
                return false;
            }

            console.log("Submitting form with values:", roundName, title, options, correctAnswer, correctAnswerIndex);

            var message = Module.UpdateRoundQuestions(urlToLoad, urlToSave, question);
            showToast(message);
            console.log("UpdateRoundQuestions message:", message);

            closeForm('addQuestionForm');
        });

        return false;

    } catch (error) {
        showToast("Error: " + error);
        console.error("Error caught in addQuestion:", error);
        return false;
    }
}

function loadJsonServer(){
    
    var jsonData = Module.LoadJsonFromServer(urlToLoad);
    
    console.log("Raw JSON data:", (jsonData));
    console.log("Fetched JSON data:", JSON.parse(jsonData));
    showToast(JSON.stringify(JSON.parse(jsonData)));
}

 
