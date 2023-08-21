# Define the path to the executable file
$filename = "./play.exe"

# Define the custom message to replace the original MZ part
$customMessage = "This only runs on Windows. Sorry :)       "

# Convert the custom message to a byte array
$customBytes = [System.Text.Encoding]::ASCII.GetBytes($customMessage)

# Open the executable file in binary mode and modify the MZ part
$fileStream = [System.IO.File]::OpenWrite($filename)
$fileStream.Seek(78, 'Begin')
$fileStream.Write($customBytes, 0, $customBytes.Length)
$fileStream.Close()
