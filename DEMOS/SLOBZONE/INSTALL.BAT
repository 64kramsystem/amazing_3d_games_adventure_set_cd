@echo off

echo Slob Zone Playable Demo
echo зддддддддддддддд©
echo Ё Please Wait...Ё
echo юддддддддддддддды

mkdir c:\slobzone
setd
IF ERRORLEVEL 1 GOTO ABORTED1
setm
IF ERRORLEVEL 1 GOTO ABORTED2
GOTO END
:ABORTED1
echo Digital Sound Driver configuration was not completed.
GOTO FINI
:ABORTED2
echo Midi Music Driver configuration was not completed.
GOTO FINI
:END
cls
echo зддддддддддддддддддддддддддддд©
echo ЁSound Configuration Complete!Ё
echo юддддддддддддддддддддддддддддды
echo Type 'Slob' to run the game.
echo Type 'Slob -s' to run the game without sound.

:FINI
