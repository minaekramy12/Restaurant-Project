@echo off
setlocal enabledelayedexpansion


set "OUTPUT_FILE=flat_project.txt"
if exist "%OUTPUT_FILE%" del "%OUTPUT_FILE%"
set "SCRIPT_DIR=%~dp0"
for /f "delims=" %%F in ('dir /b /s *.cpp *.h ^| sort') do (
    set "FULL_PATH=%%F"
    set "REL_PATH=!FULL_PATH:%SCRIPT_DIR%=!"
    
    echo ______________!REL_PATH!_______________________ >> "%OUTPUT_FILE%"
    
    type "%%F" >> "%OUTPUT_FILE%"
    echo. >> "%OUTPUT_FILE%"
)

echo Project successfully flattened into %OUTPUT_FILE%.
pause