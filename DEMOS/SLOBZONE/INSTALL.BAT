@echo off

echo Slob Zone Playable Demo
echo 旼컴컴컴컴컴컴컴?
echo ? Please Wait...?
echo 읕컴컴컴컴컴컴컴?

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
echo 旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴?
echo 쿞ound Configuration Complete!?
echo 읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴?
echo Type 'Slob' to run the game.
echo Type 'Slob -s' to run the game without sound.

:FINI
