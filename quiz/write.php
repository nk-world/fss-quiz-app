<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $jsonData = file_get_contents('php://input');
   
    if (!empty($jsonData)) {
        $data = json_decode($jsonData, true);
        if ($data !== null) {
            // Process the JSON data as needed

            // Save the JSON data to a file
            $filename = 'quiz.json';

            $fileContent = json_encode($data, JSON_PRETTY_PRINT);
            echo $fileContent;
            if (file_put_contents($filename, $fileContent) !== false) {
                echo json_encode(array('message' => 'Data received and saved to file'));
            } else {
                $errorMessage = 'Failed to save data to file: ' . error_get_last()['message'];
                error_log($errorMessage);
                
                http_response_code(500); // Internal Server Error
                echo json_encode(array('error' => $errorMessage));
            }
            
        } else {
            http_response_code(400); // Bad Request
            echo json_encode(array('error' => 'Invalid JSON data'));
        }
    } else {
        http_response_code(400); // Bad Request
        echo json_encode(array('error' => 'No JSON data received'));
    }
} else {
    http_response_code(405); // Method Not Allowed
    echo json_encode(array('error' => 'Invalid request method'));
}
?>
