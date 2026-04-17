@echo off
setlocal EnableDelayedExpansion

echo ==========================================
echo    RESTAURANT TEST FILE GENERATOR
echo ==========================================
echo.

:AskFilename
set "filename="
set /p filename="Enter output file name (e.g., input.txt): "
if "!filename!"=="" echo [ERROR] Filename cannot be empty! & goto AskFilename

echo.
echo --- CHEF PARAMETERS ---
:AskCS
set "numCS="
set /p numCS="Enter number of CS Chefs: "
if "!numCS!"=="" echo [ERROR] Input cannot be empty! & goto AskCS
if !numCS! LSS 0 echo [ERROR] Cannot be negative! & goto AskCS

:AskCN
set "numCN="
set /p numCN="Enter number of CN Chefs: "
if "!numCN!"=="" echo [ERROR] Input cannot be empty! & goto AskCN
if !numCN! LSS 0 echo [ERROR] Cannot be negative! & goto AskCN

:AskSpeedCS
set "speedCS="
set /p speedCS="Enter speed of CS Chefs: "
if "!speedCS!"=="" echo [ERROR] Input cannot be empty! & goto AskSpeedCS
if !speedCS! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskSpeedCS

:AskSpeedCN
set "speedCN="
set /p speedCN="Enter speed of CN Chefs: "
if "!speedCN!"=="" echo [ERROR] Input cannot be empty! & goto AskSpeedCN
if !speedCN! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskSpeedCN

echo.
echo --- SCOOTER PARAMETERS ---
:AskNumScooters
set "numScooters="
set /p numScooters="Enter total number of Scooters: "
if "!numScooters!"=="" echo [ERROR] Input cannot be empty! & goto AskNumScooters
if !numScooters! LSS 0 echo [ERROR] Cannot be negative! & goto AskNumScooters

:AskScooterSpeed
set "speedScooter="
set /p speedScooter="Enter Scooter speed: "
if "!speedScooter!"=="" echo [ERROR] Input cannot be empty! & goto AskScooterSpeed
if !speedScooter! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskScooterSpeed

:AskMOrds
set "mOrds="
set /p mOrds="Enter number of orders before maintenance: "
if "!mOrds!"=="" echo [ERROR] Input cannot be empty! & goto AskMOrds
if !mOrds! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskMOrds

:AskMDur
set "mDur="
set /p mDur="Enter maintenance duration (in timesteps): "
if "!mDur!"=="" echo [ERROR] Input cannot be empty! & goto AskMDur
if !mDur! LSS 0 echo [ERROR] Cannot be negative! & goto AskMDur

echo.
echo --- TABLE PARAMETERS ---
:AskNumTables
set "numTables="
set /p numTables="Enter TOTAL number of tables: "
if "!numTables!"=="" echo [ERROR] Input cannot be empty! & goto AskNumTables
if !numTables! LEQ 0 echo [ERROR] Must have at least 1 table! & goto AskNumTables

:AskMinTable
set "minTableCap="
set /p minTableCap="Enter MIN seats per table: "
if "!minTableCap!"=="" echo [ERROR] Input cannot be empty! & goto AskMinTable
if !minTableCap! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskMinTable

:AskMaxTable
set "maxTableCap="
set /p maxTableCap="Enter MAX seats per table: "
if "!maxTableCap!"=="" echo [ERROR] Input cannot be empty! & goto AskMaxTable
if !maxTableCap! LSS !minTableCap! echo [ERROR] Max must be greater than or equal to Min! & goto AskMaxTable

set /a tablesLeft=!numTables!
set "tableConfig="

:GenerateTableGroups
if !tablesLeft! LEQ 0 goto EndTableGroups
set /a chunk=!RANDOM! %% !tablesLeft! + 1
set /a tableDiff=!maxTableCap! - !minTableCap! + 1
set /a cap=!RANDOM! %% !tableDiff! + !minTableCap!
set tableConfig=!tableConfig!!chunk! !cap! 
set /a tablesLeft-=!chunk!
goto GenerateTableGroups
:EndTableGroups

echo.
echo --- ACTIONS / ORDERS ---
:AskActions
set "numActions="
set /p numActions="Enter TOTAL number of actions (Requests + Cancels): "
if "!numActions!"=="" echo [ERROR] Input cannot be empty! & goto AskActions
if !numActions! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskActions

echo.
echo Generating file "!filename!" Please wait....

echo !numCS! !numCN! > "!filename!"
echo !speedCS! !speedCN! >> "!filename!"
echo !numScooters! !speedScooter! >> "!filename!"
echo !mOrds! !mDur! >> "!filename!"
echo !numTables! >> "!filename!"
echo !tableConfig! >> "!filename!"
echo !numActions! >> "!filename!"

set TS=1
set reqCount=0
set currentAction=0

:ActionLoop
if !currentAction! GEQ !numActions! goto EndActionLoop
set /a currentAction+=1

set /a step=!RANDOM! %% 3
set /a TS=!TS! + !step!

set isCancel=0
if !reqCount! GTR 0 (
    set /a roll=!RANDOM! %% 100
    if !roll! LSS 15 set isCancel=1
)

if !isCancel!==1 goto MakeCancel

:MakeRequest
set /a reqCount+=1

set /a size=!RANDOM! %% 10 + 1
set /a money=!RANDOM! %% 300 + 50
set /a typeRand=!RANDOM! %% 6

set /a diff=!maxTableCap! - !minTableCap! + 1
set /a seats=!RANDOM! %% !diff! + !minTableCap!

set /a dur=!RANDOM! %% 20 + 10

set /a shareRand=!RANDOM! %% 2
if !shareRand!==1 (set share=Y) else (set share=N)

set /a dist=!RANDOM! %% 2000 + 500

if !typeRand!==0 echo Q ODG !TS! !reqCount! !size! !money! !seats! !dur! !share! >> "!filename!"
if !typeRand!==1 echo Q ODN !TS! !reqCount! !size! !money! !seats! !dur! !share! >> "!filename!"
if !typeRand!==2 echo Q OT !TS! !reqCount! !size! !money! >> "!filename!"
if !typeRand!==3 echo Q OVC !TS! !reqCount! !size! !money! !dist! >> "!filename!"
if !typeRand!==4 echo Q OVG !TS! !reqCount! !size! !money! !dist! >> "!filename!"
if !typeRand!==5 echo Q OVN !TS! !reqCount! !size! !money! !dist! >> "!filename!"

goto EndOfLoop

:MakeCancel
set /a cancelID=!RANDOM! %% !reqCount! + 1
echo X !TS! !cancelID! >> "!filename!"

:EndOfLoop
goto ActionLoop

:EndActionLoop
echo.
echo ==========================================
echo SUCCESS: Your file "%filename%" is ready.
echo ==========================================
pause