# Microsoft Developer Studio Project File - Name="iperf" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=iperf - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "iperf.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "iperf.mak" CFG="iperf - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "iperf - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "iperf - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "iperf - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "temp"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "iperf - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "temp"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".\cfg\\" /I ".\lib\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Advapi32.lib kernel32.lib shell32.lib Ws2_32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "iperf - Win32 Release"
# Name "iperf - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Client.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\delay.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\endian.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\error.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Extractor.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\gettimeofday.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\gnu_getopt.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\gnu_getopt_long.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\inet_aton.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Listener.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\main.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\PerfSocket.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\PerfSocket_TCP.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\PerfSocket_UDP.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Queue.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Server.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\service.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Settings.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\signal.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\snprintf.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Socket.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\SocketAddr.cpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\sockets.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\stdio.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\string.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\tcp_window_size.c"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Thread.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Client.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Condition.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\cfg\config.win32.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\delay.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Extractor.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\gettimeofday.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\gnu_getopt.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\headers.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\cfg\headers_slim.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\inet_aton.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Listener.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Locale.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Mutex.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\PerfSocket.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Queue.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Server.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\service.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\Settings.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\snprintf.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Socket.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\SocketAddr.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Thread.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\Timestamp.hpp"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\lib\util.h"
# End Source File
# Begin Source File

SOURCE="C:\Documents and Settings\qfeng\My Documents\RA\SourceCode\iperf\src\version.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
