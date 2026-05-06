@echo off
setlocal EnableDelayedExpansion

echo ==========================================
echo   RESTAURANT TEST FILE GENERATOR
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
:AskTableConfig
set "rawTables="
set /p rawTables="Enter table config pairs (e.g., 5 6 4 3 6 3): "
if "!rawTables!"=="" echo [ERROR] Input cannot be empty! & goto AskTableConfig

set "tableConfig="
set "numTablesTotal=0"
set "maxTableCap=1"
set "tokenCount=0"
set "prevToken="

for %%A in (!rawTables!) do (
    set /a tokenCount+=1
    set /a isEven=tokenCount %% 2
    if !isEven!==0 (
        rem We are on the capacity number part of the pair
        set tableConfig=!tableConfig! !prevToken! %%A
        set /a numTablesTotal+=!prevToken!
        
        rem Safely evaluate numeric max capacity
        set /a currentCap=%%A
        if !currentCap! GTR !maxTableCap! set "maxTableCap=!currentCap!"
    ) else (
        rem We are on the count number part of the pair
        set "prevToken=%%A"
    )
)

set /a checkOdd=tokenCount %% 2
if !checkOdd!==1 (
    echo.
    echo [WARNING] You entered an odd number of values. 
    echo The last standalone number [!prevToken!] has been truncated to keep valid pairs.
)

if !numTablesTotal! LEQ 0 (
    echo [ERROR] No valid tables detected! Please try again.
    goto AskTableConfig
)

echo.
echo --- SIMULATION MODE ---
:AskBonus
set "isBonusFlag="
set /p isBonusFlag="Is this a BONUS file? (Y/N): "
if /I "!isBonusFlag!"=="Y" (
    set "bonusMode=1"
    goto AskTH
) else if /I "!isBonusFlag!"=="N" (
    set "bonusMode=0"
    goto AskActions
) else (
    echo [ERROR] Invalid input. Please enter Y or N.
    goto AskBonus
)

:AskTH
set "TH="
set /p TH="Enter Overwait Threshold (TH): "
if "!TH!"=="" echo [ERROR] Input cannot be empty! & goto AskTH
if !TH! LSS 0 echo [ERROR] Cannot be negative! & goto AskTH

:AskFailProb
set "failProb="
set /p failProb="Enter Scooter Failure Probability (1-100): "
if "!failProb!"=="" echo [ERROR] Input cannot be empty! & goto AskFailProb
if !failProb! LSS 1 echo [ERROR] Must be between 1 and 100! & goto AskFailProb
if !failProb! GTR 100 echo [ERROR] Must be between 1 and 100! & goto AskFailProb

echo.
echo --- ACTIONS / ORDERS ---
:AskActions
set "numActions="
set /p numActions="Enter TOTAL number of actions (Requests + Cancels): "
if "!numActions!"=="" echo [ERROR] Input cannot be empty! & goto AskActions
if !numActions! LEQ 0 echo [ERROR] Must be greater than 0! & goto AskActions

:AskCancelPercent
set "cancelPercent="
set /p cancelPercent="Enter cancel request percentage (1-100): "
if "!cancelPercent!"=="" echo [ERROR] Input cannot be empty! & goto AskCancelPercent
if !cancelPercent! LSS 1 echo [ERROR] Must be between 1 and 100! & goto AskCancelPercent
if !cancelPercent! GTR 100 echo [ERROR] Must be between 1 and 100! & goto AskCancelPercent

echo.
echo Generating file "!filename!" Please wait....

rem Print header strictly in format: CN CS, then speeds
echo !numCN! !numCS! > "!filename!"
echo !speedCN! !speedCS! >> "!filename!"
echo !numScooters! !speedScooter! >> "!filename!"
echo !mOrds! !mDur! >> "!filename!"
echo !numTablesTotal! >> "!filename!"
echo !tableConfig:~1! >> "!filename!"

rem If Bonus Mode, write the bonus variables
if !bonusMode!==1 (
    echo !TH! >> "!filename!"
    echo !failProb! >> "!filename!"
)

echo !numActions! >> "!filename!"

set TS=1
set reqCount=0
set currentAction=0
set ovc_count=0

:ActionLoop
if !currentAction! GEQ !numActions! goto EndActionLoop
set /a currentAction+=1

set /a step=!RANDOM! %% 3
set /a TS=!TS! + !step!

set isCancel=0
rem Only attempt a cancel if there is at least one OVC order generated
if !ovc_count! GTR 0 (
    set /a roll=!RANDOM! %% 100
    if !roll! LSS !cancelPercent! set isCancel=1
)

if !isCancel!==1 goto MakeCancel

:MakeRequest
set /a reqCount+=1

set /a size=!RANDOM! %% 10 + 1
set /a money=!RANDOM! %% 300 + 50
set /a dist=!RANDOM! %% 2000 + 500
set /a seats=!RANDOM! %% !maxTableCap! + 1
set /a dur=!RANDOM! %% 20 + 10

set /a shareRand=!RANDOM! %% 2
if !shareRand!==1 (set share=Y) else (set share=N)

if !bonusMode!==1 (
    set /a typeRand=!RANDOM! %% 7
) else (
    set /a typeRand=!RANDOM! %% 6
)

if !typeRand!==0 echo Q ODG !TS! !reqCount! !size! !money! !seats! !dur! !share! >> "!filename!"
if !typeRand!==1 echo Q ODN !TS! !reqCount! !size! !money! !seats! !dur! !share! >> "!filename!"
if !typeRand!==2 echo Q OT !TS! !reqCount! !size! !money! >> "!filename!"
if !typeRand!==4 echo Q OVG !TS! !reqCount! !size! !money! !dist! >> "!filename!"
if !typeRand!==5 echo Q OVN !TS! !reqCount! !size! !money! !dist! >> "!filename!"

if !typeRand!==3 (
    echo Q OVC !TS! !reqCount! !size! !money! !dist! >> "!filename!"
    rem Store the OVC order ID in our pseudo-array to use for future cancellations
    set /a ovc_count+=1
    set ovc_list[!ovc_count!]=!reqCount!
)

if !typeRand!==6 (
    rem --- COMBO CHEFS LOGIC ---
    set /a totalChefs=!numCS! + !numCN!
    set maxC=4
    if !totalChefs! LSS 4 set maxC=!totalChefs!
    if !maxC! LSS 1 set maxC=1
    set /a reqC=!RANDOM! %% !maxC! + 1
    
    rem --- COMBO SCOOTER LOGIC ---
    set /a maxS=!numScooters! * 75 / 100
    if !maxS! LSS 2 set maxS=2
    
    rem Calculate range (max - 2 + 1) -> max - 1
    set /a rangeS=!maxS! - 1
    set /a reqS=!RANDOM! %% !rangeS! + 2

    echo Q COMBO !TS! !reqCount! !size! !money! !dist! !reqC! !reqS! >> "!filename!"
)

goto EndOfLoop

:MakeCancel
rem --- 1. Decide which "recent window" to pick from based on probabilities ---
set /a roll=!RANDOM! %% 100 + 1

if !roll! LEQ 50 (
    set poolSize=4
) else if !roll! LEQ 80 (
    set poolSize=8
) else if !roll! LEQ 90 (
    set poolSize=10
) else if !roll! LEQ 95 (
    set poolSize=!ovc_count!
) else (
    set poolSize=1
)

rem --- 2. Cap the pool size if we haven't generated that many OVC orders yet ---
if !poolSize! GTR !ovc_count! set poolSize=!ovc_count!

rem --- 3. Calculate a random index within that recent window ---
set /a startIndex=!ovc_count! - !poolSize! + 1
set /a offset=!RANDOM! %% !poolSize!
set /a randIndex=!startIndex! + !offset!

rem Retrieve the OVC order ID to cancel
for %%i in (!randIndex!) do set cancelID=!ovc_list[%%i]!

rem --- 4. Write the cancel action to the file ---
echo X !TS! !cancelID! >> "!filename!"

rem --- 5. ARRAY SHIFT LOGIC: Remove the ID so it is never picked again ---
set limit=!ovc_count!
set current=!randIndex!

:ShiftLoop
if !current! GEQ !limit! goto EndShift
set /a nextItem=!current! + 1

rem Grab the value of the next item and overwrite the current item
for %%N in (!nextItem!) do set tempVal=!ovc_list[%%N]!
for %%C in (!current!) do set ovc_list[%%C]=!tempVal!

set /a current+=1
goto ShiftLoop

:EndShift
rem Clear the very last duplicate element and shrink the array size tracking
set ovc_list[!ovc_count!]=
set /a ovc_count-=1
goto EndOfLoop

:EndOfLoop
goto ActionLoop

:EndActionLoop
echo.
echo ==========================================
echo SUCCESS: Your file "%filename%" is ready.
echo ==========================================
pause
