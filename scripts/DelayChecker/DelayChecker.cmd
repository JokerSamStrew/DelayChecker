IF EXIST ".\DelayChecker.exe" (
    call ".\DelayChecker.exe" "rtsp://localhost:5555/stream" 
) ELSE (
    call "D:\Projects\DelayChecker\x64\Release\DelayChecker.exe" "rtsp://localhost:5555/stream" 
)
