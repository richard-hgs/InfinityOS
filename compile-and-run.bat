REM Disables the commands output
echo INFO   -^> Disabling echo...
@echo off
REM Define some local variables
echo INFO   -^> Defining the local variables
SET CUR_PATH=%CD%
SET CUR_DRIVE=%CUR_PATH:~0,2%
SET CONTAINER_NAME=infinityos-buildenv
SET DOCKER_BUILD_RESULT=
SET DOCKER_BUILD_ERROR=0
SET DOCKER_BUILD_WARNING=0
SET OS_BIN_PATH=./build/floppy.img
SET OS_FULL_PATH=%CUR_PATH%\build\floppy.img
SET EXTERNAL_DRIVE_PATH=\\?\PhysicalDrive2
SET EXTERNAL_DRIVE_TYPE=2
SET EXTERNAL_DRIVE_SECTORS_TO_COPY=100
REM SET MAKE_BUILD_COMMAND_NAME=all
REM Enter into cur driver letter
echo INFO   -^> Entering into "%CUR_DRIVE%" Driver letter location
%CUR_DRIVE%
REM Enter into project directory folder
echo INFO   -^> Entering into project directory folder "%CUR_PATH%"
cd %CUR_PATH%
REM Enumerate docker containers that are running and save into DOCKER_LS_RESULT VAR
FOR /F "tokens=*" %%g IN ('docker container ls') do (SET DOCKER_LS_RESULT=%%g)
echo INFO   -^> "docker container ls" result: %DOCKER_LS_RESULT%
REM Check if docker container is running if not start it
(echo %DOCKER_LS_RESULT% | findstr /i /c:"%CONTAINER_NAME%" >nul) && (echo INFO   -^> Container is running...) || (echo INFO   -^> Container is not running starting it now in detached mode... && docker start %CONTAINER_NAME% & docker ps -aqf "name=%CONTAINER_NAME%" > docker-container-id.txt)
REM Retrieve docker container id and save into variable
SET /p DOCKER_CONTAINER_ID=<docker-container-id.txt
REM Attach and run command to the running container
echo INFO   -^> Running command in docker container to build the O.S
echo,
docker exec -ti %DOCKER_CONTAINER_ID% sh -c "cd /usr/local && make all && make disk"
echo,
REM echo,
REM FOR /F "tokens=*" %%g IN ('docker exec -ti %DOCKER_CONTAINER_ID% sh -c "cd /usr/local2 && make all && make disk"') do (
REM     echo MAKE   -^> %%g
REM     REM call SET DOCKER_BUILD_RESULT=%%DOCKER_BUILD_RESULT%%&echo,%%g
REM     1>nul (
REM         call set MY_G=%%g
REM         call set "MY_G=%%MY_G:\=/%%"
REM         echo %%MY_G%%|find "Error" && (SET DOCKER_BUILD_ERROR=1) || (SET DOCKER_BUILD_ERROR=0)
REM         echo %%MY_G%%|find "warning" && (SET DOCKER_BUILD_WARNING=1) || (SET DOCKER_BUILD_WARNING=0)
REM     )
REM )
REM REM Check if errors or warnings exists and inform the user
REM If %DOCKER_BUILD_ERROR%==1 (
REM     echo ERROR   -^> Errors while building the O.S check the above output...
REM ) 
REM If %DOCKER_BUILD_WARNING%==1 (
REM     echo WARNING -^> Warnings while building the O.S check the above output...
REM )
REM echo,
REM Run the virtual machine to test the O.S. if no errors found
If %DOCKER_BUILD_ERROR%==0 (
    echo,
    echo INFO   -^> Copying the Operating System to external drive
    imgwrite\tools\PSTools\psexec \\LAPTOP-BQARV16T -accepteula -nobanner -user Administrador -p 91382152 wscript.exe "%CUR_PATH%\imgwrite\tools\invisible.vbs" "%CUR_DRIVE% & cd %CUR_PATH% & imgwrite\tools\InfinityArchiveTools --write --drive %EXTERNAL_DRIVE_PATH% --drive-type %EXTERNAL_DRIVE_TYPE% --input-file %OS_FULL_PATH% --sectors %EXTERNAL_DRIVE_SECTORS_TO_COPY% & exit" "%CUR_PATH%\imgwrite\tools\psexec_output.txt"
    IF EXIST "imgwrite\tools\psexec_output.txt" (
        type imgwrite\tools\psexec_output.txt
        REM powershell -nologo "& "Get-Content -Wait imgwrite\tools\psexec_output.txt -Tail 10"
        REM more imgwrite\tools\psexec_output.txt
        Del imgwrite\tools\psexec_output.txt
        echo,
    )
    echo INFO   -^> Running the qemu virtual machine...
    
    REM Qemu release
    qemu-system-i386 -fda %OS_BIN_PATH% -boot a -s -soundhw pcspk

    REM Qemu debug seriallog.log
    REM qemu-system-i386 -fda %OS_BIN_PATH% -boot a -s -soundhw pcspk -chardev stdio,id=char0,mux=on,logfile=serial.log,signal=off -serial chardev:char0 -mon chardev=char0
    
    REM Qemu debugable
    REM qemu-system-i386 -fda %OS_BIN_PATH% -boot a -s -soundhw pcspk -S
    
    REM Cygwin gdb
    REM gdb -ex "target remote localhost:1234" -ex "symbol-file build/kernel/kernel.elf" -ex "br *0x7e00"
    REM GDB - PRINT REGS: 

    REM start cmd /k "cd 'D:\Programming\VSProjects\OperatingSystems\boot32-barebones-master' & qemu-system-i386 -fda floppy.img -boot a -soundhw pcspk -s -S"
    REM C:\cygwin64\bin\bash --login -c "cd D:/Programming/VSProjects/OperatingSystems/InfinityOS && gdb -ix "gdb_init_real_mode.txt" -ex "set tdesc filename target.xml" -ex "target remote localhost:1234" -ex "br *0x11eee" -ex "c""
    REM qemu-system-x86_64 -fda %OS_BIN_PATH%
    
    REM Pendrive load
    REM qemu-system-i386 -fda \\.\PhysicalDrive2 -boot a -s -soundhw pcspk
    REM gdb -ex "target remote localhost:1234" -ex "symbol-file build/kernel/kernel.elf"
)
REM Enables the echo again
echo INFO   -^> Enabling echo...
@echo on
