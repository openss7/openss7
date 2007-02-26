##########@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@###################################
#
# Copyright (C) 2005 Intel Corporation. All Rights Reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.      Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2.      Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in
# the documentation and/or other materials provided with the
# distribution.
#
# 3.      Neither the name Intel Corporation nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
###################################@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@##########

#############################################################################
#                   IPSCREEN UTILITY, VERSION 1.02                          #
#       Screens the desired source for the following                        #
#       1.  Removes old and updates source with new copyright notice        #
#       2.  Scans for profanity against a provided profanity list           #
#       3.  Scans for customer Names against a provided customer list       #			
#       4.  Scans for employee Names against a provided employee name list  #
#############################################################################
#############################################################################
#                             REVISION HISTORY                              #
#                                                                           #
# 02/15/2006: G Goutam   Ver1.02:  Added support for automating the update  #
#                                  of the MODULE_LICENSE variable for the   #
#                                  Drivers. Added a "module_license command #
#                                  line option that will take either GPL or #
#                                  on the command line                      #
#                                                                           #
#############################################################################


#!/usr/bin/env tclsh

set FileList [ eval [list exec dir ] ]
set i 0
set var_count 0
set dash -
set removeOption  0
set filesOption   0
set updateOption  0


# COMMAND LINE ARGUMENTS
# puts -nonewline "Enter Copyright Tool command: "

#***************************************************************************************************
#********************  REMOVE AND APPEND NEW COPYRIGHT   *******************************************
#***************************************************************************************************		
proc removeLinesAndAppendNewCopyright { beginLine endLine totalLines numberOfLines fileName enableCopyRightUpdate copyRightFileNameOutput copyRightFoundFlag } {
set index $beginLine

	#CHECK IF OLD COPYRIGHT WAS FOUND IN THE FILE AND REMOVE 

	if { $copyRightFoundFlag != 65535 }  {
		while { $index <= $endLine }  {
			set totalLines [lreplace $totalLines $beginLine $beginLine ]
			incr index
		}
	}
	
	if { $enableCopyRightUpdate == 1} {

		set newFileID [ open $copyRightFileNameOutput "r" ]
		set newFileContent [read -nonewline $newFileID]
		set newFileTotalLines  [ split $newFileContent "\n"]
		set newFileNumberOfLines [ expr [ llength $newFileTotalLines] -1 ]
		close $newFileID
	}

	set updatedFileID [open $fileName "w"] 
	if { $enableCopyRightUpdate == 1} {
		puts $updatedFileID [join $newFileTotalLines "\n"]
	}
			
	puts $updatedFileID [join $totalLines "\n"]
	close $updatedFileID
	return 0	
}

#***************************************************************************************************
#********************  DETERMINE THE BEGINNING LINE OF THE OLD COPYRIGHT TO REMOVE  ****************
#***************************************************************************************************		
proc GetCopyrightBeginLine { currentLineNumber totalLines } {
    global C_StyleCommentRemovalActive
	set C_StyleCommentRemovalActive 0
	set previousIndex 1

		while { $previousIndex <= $currentLineNumber }  {
			set previousLineNumber [ expr ($currentLineNumber - $previousIndex)]
			set currentLine [lindex  $totalLines $previousLineNumber ] 
			set currentLine [string tolower $currentLine]

			# CHECKED FOR 'NEW' CANNED VERSION OF "COPYRIGHT" STRING
			if { [ string first "@@@soft@@@ware@@@copy@@@right@@@" $currentLine 0] >= 0 } {
				# RETURN LINE NUMBER TO THE FIRST COMMENT LINE
				return $previousLineNumber
				break					
			} else {
				incr previousIndex
			}
		}
		# ASSUMING THIS IS A PRE-EXISITNG COMMENT
		# PERFORM ADDITIONAL CHECKS TO REMOVE STRAY COMMENTS
		set tempIndex $currentLineNumber
		incr tempIndex -1
		set currentLine [lindex  $totalLines  $tempIndex ] 
		if { [ string first "/*************************************************" $currentLine 0] >= 0 || \
			 [ string first "//////////////////////////////////////////////////" $currentLine 0] >= 0 } {
			 incr currentLineNumber -1
			 set C_StyleCommentRemovalActive 2
		}
		return $currentLineNumber
}

#***************************************************************************************************
#********************  DETERMINE THE END LINE OF THE OLD COPYRIGHT TO REMOVE  **********************
#***************************************************************************************************	

proc GetCopyrightEndLine { currentLineNumber TotalLines numberOfLines } {
	global commentErrorFound
    global C_StyleCommentRemovalActive

	set forwardIndex 1
		while { $forwardIndex <= $numberOfLines  }  {
			set forwardLineNumber [ expr ($currentLineNumber + $forwardIndex)]
			set currentLine [lindex  $TotalLines $forwardLineNumber] 
			set currentLine [string tolower $currentLine]

			# CHECK FOR CANNED COPYRIGHT HEADERS
			if { [ string first "such source code" $currentLine 0] >= 0 || \
				 [ string first "property of others" $currentLine 0] >= 0 || \
				 [ string first "respective owners" $currentLine 0] >= 0 } {

					 if { [ string first "*/" $currentLine 0] >= 0 } {
						# FLAG COMMENT MISMATCH ERROR
						set commentErrorFound 1
					 }
					 # REMOVE STRAY COMMENT LINES AT THE END
					 set tempIndex $forwardLineNumber
					 incr tempIndex 
					 set currentLine [lindex  $TotalLines $tempIndex ] 
					 if { [ string first "***********************************************************/" $currentLine 0] >= 0 || \
						  [ string first "/////////////////////////////////////////////////////////// " $currentLine 0] >= 0 } {
							if { $C_StyleCommentRemovalActive == 2} {
								incr forwardLineNumber
							}
							incr C_StyleCommentRemovalActive
					 }
					 return $forwardLineNumber 
			   } elseif { [ string first "@@@soft@@@ware@@@copy@@@right@@@" $currentLine 0] >= 0 } {					
					return $forwardLineNumber
			   } else {
					incr forwardIndex
			   }
		}
		return $currentLineNumber
}

#***************************************************************************************************
#********************  UPDATE THE MODULE LICENSE FIELD IN DESIRED FILE  ****************************
#***************************************************************************************************	
proc updateModuleLicenseField { entireFileName } {
global moduleLicense
set currentLineNumber 0
set semicolon ";"

	if { [catch {open $entireFileName r+} FileID] } {
		puts $errorLogID "File Open ERROR, Could not open file; $entireFileName; for writing"
		return 0
	} else {
		set FileContent [read -nonewline $FileID]
		set totalLines  [ split $FileContent "\n"]
		set numberOfLines [ expr [ llength $totalLines] -1 ]
		close $FileID
	}

	while { $currentLineNumber <= $numberOfLines }  {

		set currentLine [lindex  $totalLines $currentLineNumber ] 
		set currentLine [ string tolower $currentLine ]

		if { [ string first "module_license" $currentLine 0] >= 0 } {

			set currentLine MODULE_LICENSE("$moduleLicense")$semicolon
			#UPDATE MODULE LINE WITH APPROPRIATE TAG - BSD/GPL
			set totalLines [lreplace $totalLines $currentLineNumber $currentLineNumber $currentLine ]
			break	
		}		
	    incr currentLineNumber
	}
	set updatedFileID [open $entireFileName "w"] 
	puts $updatedFileID [join $totalLines "\n"]
	close $updatedFileID
	return 0	
}

#***************************************************************************************************
#********************  REMOVE LINES BETWEEN BEGINLINE AND ENDLINE TO REMOVE OLD ********************
#********************  OLD COPYRIGHT NOTICES. APPEND THE NEW COPYRIGHT NOTICE  *********************
#********************  IF REQUESTED ON THE COMMANDLINE VIA THE enableCopyRightUpdate parameter *****
#***************************************************************************************************	
proc removeOldCopyRightAndUpdateNew { entireFileName  enableCopyRightUpdate  copyRightFileNameOutput fileExtension enableModuleLicenseUpdate} {
global startDir
global noCopyrightFoundListID
global noCopyRightFileCounter
global errorLogID
global fileOpenErrorLogID
global commentErrorFound
global C_StyleCommentRemovalActive
global initialReleaseDate
global mostRecentReleaseDate
global copyRightFileNameInput
 				 
set linecount 0
set copyRightFoundFlag 65535
set thirdPartyCopyRightFlag 65535
set currentLineNumber 0
set commentErrorFound 0
set tempIndex 0

	if { [catch {open $entireFileName r+} FileID] } {
		puts $errorLogID "File Open ERROR, Could not open file; $entireFileName; for writing"
		return 0
	} else {
#		set FileID [ open $entireFileName r+ ]
		set FileContent [read -nonewline $FileID]
		set totalLines  [ split $FileContent "\n"]
		set numberOfLines [ expr [ llength $totalLines] -1 ]
		close $FileID
	}
	
	while  { $numberOfLines > 0 }   {
		set currentLine [lindex  $totalLines $currentLineNumber ] 
		set currentLine [ string tolower $currentLine ]

	 # CHECKS for COPYRIGHT word IN LINE
	 if { [ string first "copyright" $currentLine 0] >=0 } {

		# SET COPYRIGHT FLAG TO ZERO INDICATING THAT COPYRIGHT HAS BEEN FOUND 
		set copyRightFoundFlag 0 

		# CHECK WHETHER COPYRIGHT IS NON-INTEL/DIALOGIC; I.E.3RD PARTY - IF SO, LOG AND IGNORE
		if { [ string first "intel" $currentLine 0] >= 0  || \
			 [ string first "dialogic" $currentLine 0] >= 0 } {

			# ADD A CHECK TO DETERMINE WHETHER INITIAL RELEASE DATE IS OVER-RIDDEN WITH
			# COMMAND LINE OPTION

			# CHECK FOR PRESENCE OF INITIAL and MOST RECENT RELEASE DATES IN THE COPRIGHT NOTICE
			set indexVal19 [string first "19" $currentLine 0] 
			set indexVal20 [string first "20" $currentLine 0] 

			# ADD THE NEED TO OVERRIDE DEFAULT INITIAL RELEASE DATE WITH COMMANDLINE OPTION
			if { $initialReleaseDate == "" } {

			if { $indexVal19 >=0 && $indexVal20 >= 0 } {
				set tempIndex $indexVal19 
				incr tempIndex 3 	
				set initialReleaseDate [string range $currentLine $indexVal19 $tempIndex ]				 

				set tempIndex $indexVal20 
				incr tempIndex 3	

				# BELOW LINES MAYNOT EVEN BE NEEDED IF WE ASSUME THAT A DEFAULT MOSTRECENTRELEASEDATE
				# EXISTS AS WELL AS THE ABILITY TO OVERRIDE DEFAULT RELEASE DATE VIA COMMAND LINE
				set tempRecentReleaseDate [string range $currentLine $indexVal20 $tempIndex ]	

			} elseif { $indexVal19 >=0 && $indexVal20 < 0 } {
				# IF OLD DATE FOUND IS 19XX,  THEN ASSUME THAT IT WILL BE INITIAL RELEASE DATE

				set tempIndex $indexVal19 
				incr tempIndex 3 	
				set initialReleaseDate [string range $currentLine $indexVal19 $tempIndex ]				 


			} elseif { $indexVal19 < 0 && $indexVal20 >= 0 } {
				# IF OLD DATE FOUND IS 20XX,  AND IF IT IS LESS THAN the YEAR 2006 
				# THEN IT WILL ALSO BE THE INITIAL RELEASE DATE

				set tempIndex $indexVal20 
				incr tempIndex 3 	
				set initialReleaseDate [string range $currentLine $indexVal20 $tempIndex ]				 

				if { [string first $initialReleaseDate $mostRecentReleaseDate 0] >= 0 } {
					set initialReleaseDate ""
				}
			} elseif { $indexVal19 < 0 && $indexVal20 < 0 } {
#				puts stdout "DEF::InitialDate = $initialReleaseDate; mostRecentDate = $mostRecentReleaseDate"			 
			}			
			
			}

			# PREPARE COPYRIGHT FILE ACCORDINGLY ADDING THE INITIAL AND MOST RECENT RELEASE DATES
			if { $enableCopyRightUpdate == 1} {
				prepareCopyRightFile  $fileExtension $startDir $copyRightFileNameInput $initialReleaseDate $mostRecentReleaseDate
			}

			# 1)  CHECK for comment start '/*' in the same line as the Copyright 
			if { [ string first "/*" $currentLine 0] >= 0 } {

				# CHECK FOR CLOSING COMMENTS IN THE SAME LINE
				if { [ string first "*/" $currentLine 0] >= 0 } {
					puts $errorLogID "Opening and Closing comments in same line; $entireFileName; LineNumber = $currentLineNumber"
					removeLinesAndAppendNewCopyright $currentLineNumber $currentLineNumber $totalLines $numberOfLines $entireFileName $enableCopyRightUpdate $copyRightFileNameOutput $copyRightFoundFlag
				} else {
					set beginLine [GetCopyrightBeginLine $currentLineNumber $totalLines]
					set endLine [GetCopyrightEndLine  $currentLineNumber $totalLines $numberOfLines]
					puts stdout "BEGIN = $beginLine;  END=$endLine"
					if { $beginLine == $endLine } {
						puts $errorLogID "Single line Copyright notice in file; $entireFileName; beginLine = $beginLine; endLine = $endLine; PLEASE CHECK!"
					}
					# ADJUST BEGINLINE TO REFLECT WHETHER OR NOT TO REMOVE COMMENT LINE											 
					if { $C_StyleCommentRemovalActive == 2 } { incr beginLine }
					removeLinesAndAppendNewCopyright $beginLine  $endLine  $totalLines $numberOfLines $entireFileName $enableCopyRightUpdate $copyRightFileNameOutput $copyRightFoundFlag
				}									
			} elseif { [ string first "//" $currentLine 0] >= 0 } {
				set beginLine [GetCopyrightBeginLine $currentLineNumber $totalLines]
				set endLine [GetCopyrightEndLine  $currentLineNumber $totalLines $numberOfLines]
				if { $beginLine == $endLine } {
						puts $errorLogID "Single line Copyright notice in file; $entireFileName; beginLine = $beginLine; endLine = $endLine; PLEASE CHECK!"
				}
				# ADJUST BEGINLINE TO REFLECT WHETHER OR NOT TO REMOVE COMMENT LINE											 
				if { $C_StyleCommentRemovalActive == 2 } { incr beginLine }
				removeLinesAndAppendNewCopyright $beginLine  $endLine  $totalLines $numberOfLines $entireFileName $enableCopyRightUpdate $copyRightFileNameOutput $copyRightFoundFlag

			} else {
				# 2) CHECK FOR THE START OF THE COMMENT IN ANY PREVIOUS LINES
				set beginLine [GetCopyrightBeginLine $currentLineNumber $totalLines]
				set endLine [GetCopyrightEndLine  $currentLineNumber $totalLines $numberOfLines ]
				if { $beginLine == $endLine } {
						puts $errorLogID "Single line Copyright notice in file; $entireFileName; beginLine = $beginLine; endLine = $endLine; PLEASE CHECK!"
				}
				if { ($endLine - $beginLine) == 1 } {
						puts $errorLogID "Single line Copyright notice in file; $entireFileName; beginLine = $beginLine; endLine = $endLine; PLEASE CHECK!"
				}
				# ADJUST BEGINLINE TO REFLECT WHETHER OR NOT TO REMOVE COMMENT LINE											 
				if { $C_StyleCommentRemovalActive == 2 } { incr beginLine }																 
  				removeLinesAndAppendNewCopyright $beginLine  $endLine  $totalLines $numberOfLines $entireFileName $enableCopyRightUpdate $copyRightFileNameOutput $copyRightFoundFlag
			}
			break
		} else {
		   # SET COPYRIGHT FLAG TO ZERO INDICATING THAT POTENTIAL THIRD PARTY COPYRIGHT HAS BEEN FOUND 
		   set thirdPartyCopyRightFlag 0 
		   break
		}
	  } 
	  incr numberOfLines -1
	  incr currentLineNumber
	}

	##########################################################################################
	# ENABLE MODULE_LICENSE UPDATE FOR DRIVERS, IF REQUESTED - ALL *.C FILES WILL BE CHECKED # 
	##########################################################################################

    if { $enableModuleLicenseUpdate == 1}  {

		 # MODULE_LICENSE PRESENT ONLY IN 'C' FILES
		 if { [ string first ".c"  $fileExtension 0] >=0 } {
			updateModuleLicenseField $entireFileName 
		 }
	}

	#LOG ALL ERRORS
	if { $copyRightFoundFlag == 65535 } {
			# LOG THE FACT THAT NO COPRIGHT WAS FOUND IN THE ENTIRE FILE
			puts $noCopyrightFoundListID $entireFileName

			# NEED TO PREPARE COPRIGHT BANNER WITH A DEFAULT DATE
			prepareCopyRightFile  $fileExtension $startDir $copyRightFileNameInput $initialReleaseDate $mostRecentReleaseDate
			removeLinesAndAppendNewCopyright 0  0  $totalLines $numberOfLines $entireFileName $enableCopyRightUpdate $copyRightFileNameOutput $copyRightFoundFlag
	}
	if { $thirdPartyCopyRightFlag == 0 } {
			puts $errorLogID "3rd Party OR Unknown CopyRight notice found in file; $entireFileName; NOT UPDATED!"
	}
	if { $commentErrorFound == 1 } {
			puts $errorLogID "Found mismatched comments removed in file; $entireFileName; LineNumber = $endLine"
	}
	# reset any GLOBALS BACK TO ZERO
	set C_StyleCommentRemovalActive  0
	set copyRightFoundFlag 65535
	set thirdPartyCopyRightFlag 65535
} 


#***************************************************************************************************
#********************  FIND AND LOG STRING NAME MATCH   ********************************************
#***************************************************************************************************	

proc findAndLogStringMatch { fileName stringsList stringFormatType } {
global stringMatchResultLogID
set currentLineNumber 0

	if { [catch {open $fileName r+} fileID ] } {
		puts stdout "Could not open $fileName for writing"
		return 0
	} else {
		set fileContent [read -nonewline $fileID]
		set totalLines  [ split $fileContent "\n"]
		set numberOfLines [ expr [ llength $totalLines] -1 ]
		close $fileID
	}
	puts stdout "FileName = $fileName; NUMBER OF LINES = $numberOfLines"
	while  { $currentLineNumber <= $numberOfLines } {
		set currentLine [lindex  $totalLines $currentLineNumber ]  
		set tempCurrentLine [ string tolower $currentLine ]
		foreach stringName $stringsList {
			set stringName [ string tolower $stringName ]

			# SET STRING FORMAT BASED ON DESIRED FORMAT TYPE
			switch $stringFormatType {
			
				1 { set stringName [format " %s"  $stringName ] }
				2 { set stringName [format " %s " $stringName ] }
				3 { set stringName [format "%s "  $stringName ] }
				4 { set stringName [format "%s"   $stringName ] }
			}
			if { [string first $stringName $tempCurrentLine 0] >= 0 } {
				set realLineNumber $currentLineNumber
				incr realLineNumber				
				puts stdout "'$fileName';line #$realLineNumber;string=**$stringName**;currentLine = $currentLine"				
				puts $stringMatchResultLogID "'$fileName';line #$realLineNumber;string=**$stringName**;currentLine = $currentLine;"
			}
		}
		incr currentLineNumber
	}
}

#***************************************************************************************************
#********************  FIND EACH STRING NAME IN FILE  ********************************************
#***************************************************************************************************	

proc findEachStringEntryInFile { dirName fileExtension stringsList stringFormatType } {
global fileListLogID
set fileCounter 0
set pwd [pwd]	

	if [catch {cd $dirName } err ]  {
		puts stderr $err
		return
	}
	foreach match [glob -nocomplain $fileExtension ] {
			incr fileCounter
			puts stdout "[file join $dirName $match ]"			
			puts $fileListLogID "[file join $dirName $match ]"
			findAndLogStringMatch [file join $dirName $match ]  $stringsList $stringFormatType			
	}
	foreach file [glob -nocomplain * ] {	    
		if [file isdirectory $file ] {
			set returnCount [findEachStringEntryInFile [file join $dirName $file ] $fileExtension $stringsList $stringFormatType ]
			incr fileCounter $returnCount
		}
	}
	cd $pwd
	return $fileCounter
}

#***************************************************************************************************
#********************  MATCH THE DESIRED STRINGS FROM FILE LIST ************************************
#***************************************************************************************************	
proc findStringsInFileList { fileList stringsFile stringFormatType } {
	global startDir
	set dirName [pwd]	
	set totalFilesCounter 0

	if [catch {cd $dirName } err ]  {
		puts stderr $err
		return 0
	}

	# OPEN FILE AND CREATE STRING NAME LIST 
	if { $stringsFile == " " } {
			puts stdout "String List not found - Cannot proceed"
			exit 0
	} else {
		set stringsFileID [ open $stringsFile r+ ]
		set stringsFileContent [read -nonewline $stringsFileID]
		set stringsList  [ split $stringsFileContent "\n"]
		set stringsListNumberOfLines [ expr [ llength $stringsList] -1 ]
		close $stringsFileID
	}

	foreach fileExtension $fileList {
        set returnCount [findEachStringEntryInFile  $dirName $fileExtension $stringsList $stringFormatType] 
		incr totalFilesCounter $returnCount	
	}
	puts stdout "TotalFiles checked = $totalFilesCounter"
}

#***************************************************************************************************
#********************  FIND EACH FILE AND ANY COPYRIGHT NOTICES ************************************
#***************************************************************************************************	
proc findCopyRightNoticeInFile { dirName FileName fileListLogID enableCopyRightUpdate copyRightFileNameOutput enableModuleLicenseUpdate} {
	global startDir
	global fileBackupOption
	set pwd [pwd]	
	set fileCounter 0
	set backupExtension ".ipscreen"
		
	if [catch {cd $dirName } err ]  {
		puts stderr $err
		return
	}
	foreach match [glob -nocomplain $FileName ] {
			incr fileCounter
			puts stdout [file join $dirName $match ]
			puts $fileListLogID "[file join $dirName $match ]"
			if { $fileBackupOption == 1 } {
				set backupFile [concat $match $backupExtension]
				file copy -force [file join $dirName $match] [ file join $dirName $backupFile ]
			}
			removeOldCopyRightAndUpdateNew [file join $dirName $match ] $enableCopyRightUpdate $copyRightFileNameOutput $FileName $enableModuleLicenseUpdate			
	}
	foreach file [glob -nocomplain * ] {	    
		if [file isdirectory $file ] {
			set returnCount [findCopyRightNoticeInFile [file join $dirName $file ] $FileName $fileListLogID $enableCopyRightUpdate $copyRightFileNameOutput $enableModuleLicenseUpdate ]
			incr fileCounter $returnCount
		}
	}
	cd $pwd
	return $fileCounter
}

#***************************************************************************************************
#********************  PREPARE NEW COPYRIGHT FILE WITH THE COMMENTS TO BE UPDATED ******************
#***************************************************************************************************	
proc prepareCopyRightFile { fileExtension  dirName copyRightFileNameInput initialReleaseDate mostRecentReleaseDate } {
	global startDir
	global copyRightFileNameOutput

	set commentChar 0
	set index 0

	# READ IN THE INPUT COPYRIGHT FILE AND ADD COMMENTS BASED ON APPROPRIATE FILE EXTENSION
	set copyRightInputFileID [ open [file join $startDir $copyRightFileNameInput ] r ]	
	set content [read -nonewline $copyRightInputFileID]
	set allLines [ split $content "\n" ]
	set numberOfLines [ expr [ llength $allLines]  ]

	close $copyRightInputFileID

	switch -glob -- $fileExtension {	
			*.c - 
			*.h -
			*.cxx -
			*.cpp -
			*.hxx - 
			*.hpp - 
			*.inc -
			*.cdp {
			 	set commentChar "*"
				set beginLine "/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************"
				set endLine   "***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/"
#				break
			}									

			*.qs -
			*.tcl -			
			*.mak -
			*.sce -
			*.sh -
			mak* -
			Mak* {
				set beginLine "##########@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@###################################"
				set endLine   "###################################@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@##########"
				set commentChar "#"	
#				break
			}

			*.ini {
				set beginLine ";;;;;;;;;;@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;"
				set endLine   ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@;;;;;;;;;;"
				set commentChar ";"	
			}
			default {
				puts stdout "Unsupported extension - $fileExtension "
			}
	}
	
	# ADD APPROPRIATE TYPE OF COMMENTS CHARS IN THE COPYRIGHT HEADER TEXT FILE
	while { $index < $numberOfLines }  {
		set currentLine [lindex  $allLines $index ] 

		# PREPARE COPYRIGHT LINE WITH THE INITIAL RELEASE AND MOST RECENT RELEASE DATES
		# NOTE:THE CASE SENSITIVE
		if { [ string first "Copyright" $currentLine 0] >=0 } {
			if { $initialReleaseDate != "" } {
				set currentLine [concat "Copyright (C) " "$initialReleaseDate-$mostRecentReleaseDate Intel Corporation. All Rights Reserved."]
			} else {
				set currentLine [concat "Copyright (C) " "$mostRecentReleaseDate Intel Corporation. All Rights Reserved."]
			}
		}

		# ADD APPROPRIATE COMMENT CHAR
		set currentLine [concat $commentChar $currentLine ]		
		#UPDATE Line WITH THE UPDATED COMMENTED LINE 
		set allLines [lreplace $allLines $index $index $currentLine ]				 
		incr index
	}

	# ADD BEGINNING COMMENT
	set allLines [linsert $allLines 0 $beginLine ]

	# ADD ENDING COMMENT
	incr numberOfLines 1
	set allLines [linsert $allLines $numberOfLines $endLine ]

	set copyRightOutputFileID [open $copyRightFileNameOutput "w"] 
	puts $copyRightOutputFileID [join $allLines "\n"]
	close $copyRightOutputFileID
}

#***************************************************************************************************
#********************  EXECUTE THE DESIRED COMMAND LINE OPTION *************************************
#***************************************************************************************************	
proc executeCommands { fileList enableCopyRightUpdate copyRightFileNameInput enableStringMatch stringsFile stringFormatType enableModuleLicenseUpdate } {
  global startDir
  global noCopyrightFoundListID 
  global copyRightFileNameOutput
  global errorLogID
  global fileOpenErrorLogID
  global fileListLogID 
  global stringMatchResultLogID

  set fileListLogID [open [ file join $startDir "IPScreenFileList.log"] "w" ]
  puts $fileListLogID "ENTIRE LIST OF FILES FOUND"
  puts $fileListLogID "--------------------------"

  set noCopyrightFoundListID [open [ file join $startDir "IPScreenNoCopyrightFound.log"] "w" ]
  puts $noCopyrightFoundListID "LIST OF FILES FOUND THAT HAVE NO INITIAL COPYRIGHT NOTICES"
  puts $noCopyrightFoundListID "----------------------------------------------------------"

  set errorLogID [ open [ file join $startDir IPScreenError.log] "w" ]
  puts $errorLogID "LIST OF POTENTIAL ERRORS IN THE FOLLOWING FILES - !!!PLEASE REVIEW!!!"
  puts $errorLogID "---------------------------------------------------------------------"

  set fileOpenErrorLogID [ open [ file join $startDir IPScreenFileOpenError.log] "w" ]
  puts $fileOpenErrorLogID "!!!THE FOLLOWING LIST OF FILES COULD NOT BE OPENED!!!"
  puts $fileOpenErrorLogID "-----------------------------------------------------"

  if { $enableStringMatch == 1 } {
	set stringMatchResultLogID [open [ file join $startDir "IPScreenStringMatchResults.log"] "w" ]
	puts $stringMatchResultLogID "THE FOLLOWING IS A LIST OF ALL DESIRED STRING MATCHES FOUND"
	puts $stringMatchResultLogID "-----------------------------------------------------------"
  }
   
  set startDir .
  set pwd [pwd]
  set i 0
  set TotalFilesCounter 0

  if { $enableStringMatch == 1 } {
		findStringsInFileList $fileList $stringsFile $stringFormatType
  } else {
		foreach fileExtension $fileList {
			set returnCount [findCopyRightNoticeInFile  $pwd $fileExtension $fileListLogID $enableCopyRightUpdate $copyRightFileNameOutput $enableModuleLicenseUpdate] 
			incr TotalFilesCounter $returnCount	
		}
  }
  puts  $fileListLogID "Total Files = $TotalFilesCounter"
  close $fileListLogID
  close $noCopyrightFoundListID
  close $errorLogID
  close $fileOpenErrorLogID

  if { $enableStringMatch == 1 } {
		close $stringMatchResultLogID
  }
}


#***************************************************************************************************
#********************  PARSE AND EXECUTE COMMAND LINE OPTIONS **************************************
#***************************************************************************************************
proc parseAndExecuteCommands { copyRightFileNameInput fileList removeOption filesOption updateOption stringFindOption stringFormatType stringsFile enableModuleLicenseUpdate helpOption } {	
set enableCopyRightUpdate 0
set enableStringMatch     0

	#FURTHER VALIDATE THE COMMANDS
	if { $filesOption == 0  && $helpOption == 0} { 
		puts stdout "Mandatory field '-files' missing on  Commandline - Program exiting"
		__exit
	} elseif { $filesOption == 1  && $removeOption == 1 } { 
		set enableCopyRightUpdate 0
	} elseif { $filesOption == 1  && $updateOption == 1  } {
			if { $copyRightFileNameInput == ""} { 
				puts stdout "Need name of new CopyRight File to proceed further - Program exiting"
				__exit
			} else {
				set enableCopyRightUpdate 1
			}
    # NOTE: STRING NAME MATCH CRITERIA - DOES NOT GO ALL THE WAY TO THE 'executeCommands' OPTION
	} elseif { $filesOption == 1  && $stringFindOption == 1  } {
			set enableStringMatch 1
			if { $stringsFile == " "} { 
				puts stdout "Need Strings Names File to perform match criteria - Program exiting"
				__exit
			} 			
	} elseif { $filesOption == 1  && [ $removeOption == 0  &&  $updateOption == 0 ] } {
		puts stdout "Command Line Option INCOMPLETE!  Need to add a '-remove' or a '-update' option"
	} elseif { $helpOption == 1 } {
		puts stdout "HELP MENU"
		puts stdout "Usage:IPScreen -files   < FileList followed by space>"
		puts stdout " -remove"
		puts stdout " -update                < Name of new Copyright Text file>"
		puts stdout " -stringFind            < Name of Strings File>"
		puts stdout " -stringFormat          < 1= space before; 2 = space on either side; 3 = space after String; 4 = no space>"
		puts stdout " -initialReleaseDate    < 4 digit year eg: 19XX or 20xx >"
		puts stdout " -mostRecentReleaseDate < 4 digit year eg: 19XX or 20xx >"
		puts stdout " -enableFileBackup     /* backs up original files with .ipscreen extension"
		puts stdout " -help                 /* prints out this information*/"	
	} else {
		puts stdout "parseAndExecuteCommands:  Unsupported Option"
		exit 0
	}
	executeCommands $fileList $enableCopyRightUpdate  $copyRightFileNameInput $enableStringMatch $stringsFile $stringFormatType $enableModuleLicenseUpdate
}

#************************************************************************************************
# *********************** COPYRIGHT UPDATE TOOL  -  MAIN ENTRY POINT ****************************
# ***********************************************************************************************
# REMOVE OPTION TO TAKE ARGUMENTS DIRECTLY FROM THE COMMAND LINE
##gets stdin argv

global startDir
global copyRightFileNameOutput
global copyRightFileNameInput
global stringsFile
global initialReleaseDate
global mostRecentReleaseDate
global moduleLicense
global fileBackupOption

#########################
# SETUP INITIAL DEFAULTS#
#########################
set startDir [pwd]
set copyRightFileNameOutput [file join $startDir "copyRightFileNameOutput.txt" ]
set removeOption 0
set filesOption 0
set updateOption 0
set stringFindOption 0
set fileBackupOption 0
set moduleLicenseOption 0
set stringFormatType 2
set helpOption 0
set fileList " "
set copyRightFileNameInput ""
set stringsFile " "
set var_count 0
set initialReleaseDate ""
set mostRecentReleaseDate 2006
set moduleLicense BSD

# THESE DATES WILL BE PARSED OUT OF THE ORIGINAL SOURCE FILE

while { $var_count < [llength $argv ]  }  {
		set arg [lindex $argv $var_count]
		switch -- $arg {
			-remove {
				set removeOption 1
				incr var_count
			} 
			-files { 
				set filesOption 1 			
				incr var_count
				set arg [lindex $argv $var_count]

				# INTIALIZE THE FIRST ELEMENT IN THE LIST
				if { [string first $dash $arg 0 ] < 0 } {
					set fileList [list $arg]

					# INITIALIZE OTHER ELEMENTS IN THE LIST
					set numberOfFiles [llength $argv]
					incr var_count
					set arg [lindex $argv $var_count]
					while { [ string first $dash $arg 0] < 0 }  { 
						if { $var_count >= $numberOfFiles-1 } break
						lappend fileList [lindex $argv $var_count]
						incr var_count
						set arg [lindex $argv $var_count]						
					} 
				} else {
					puts stdout "ERROR: File List not provided with '-files' command"  
					continue				
				} 
			} 
			-update { 
				set updateOption 1
				incr var_count
				set arg [lindex $argv $var_count]
				if { [string first $dash $arg 0] < 0 } { 
					set copyRightFileNameInput [file join $startDir [lindex $argv $var_count] ]
					puts stdout "copyRightFileNameInput = $copyRightFileNameInput"
					incr var_count
				} else {
					puts stdout "next arg = $arg"
					continue
				}
			}
			-stringFind {
				set stringFindOption 1
				incr var_count
				set arg [lindex $argv $var_count]
				if { [string first $dash $arg 0] < 0 } { 
					set stringsFile [file join $startDir [lindex $argv $var_count] ]
					puts stdout "stringsFile = $stringsFile; varcount = $var_count"
					incr var_count
				} else {
					puts stdout "next arg = $arg"
					continue
				}
			}
			-stringFormat {
				incr var_count
				set arg [lindex $argv $var_count]
				if { [string first $dash $arg 0] < 0 } { 
					set stringFormatType [lindex $argv $var_count]
					puts stdout "StringFormat = $stringFormatType; varcount = $var_count"
					incr var_count
				} else {
					puts stdout "next arg = $arg"
					continue
				}
			}
			-initialReleaseDate {
				incr var_count
				set arg [lindex $argv $var_count]
				if { [string first $dash $arg 0] < 0 } { 
					set initialReleaseDate [lindex $argv $var_count]
					puts stdout "initialReleaseDate = $initialReleaseDate"
					incr var_count
				} else {
					puts stdout "next arg = $arg"
					continue
				}
			}
			-mostRecentReleaseDate {
				incr var_count
				set arg [lindex $argv $var_count]
				if { [string first $dash $arg 0] < 0 } { 
					set mostRecentReleaseDate [lindex $argv $var_count]
					incr var_count
				} else {
					puts stdout "next arg = $arg"
					continue
				}
			}
			-moduleLicense {
				set moduleLicenseOption 1
				incr var_count
				set arg [lindex $argv $var_count]
				if { [string first $dash $arg 0] < 0 } { 
					set moduleLicense [lindex $argv $var_count]
					puts stdout "MODULE_LICENSE = $moduleLicense"
					incr var_count
				} else {
					puts stdout "next arg = $arg"
					continue
				}
			}

			-enableFileBackup {
				set fileBackupOption 1
				incr var_count
			}
			-help   { 
				set helpOption 1
				incr var_count
			}
			default { 
			    incr var_count
				if { [string first $dash $arg 0 ] < 0 } { 
				} else {
					puts stdout "$arg is an unsupported option" 
				}
			}
		}
}
parseAndExecuteCommands $copyRightFileNameInput $fileList $removeOption $filesOption $updateOption $stringFindOption $stringFormatType $stringsFile $moduleLicenseOption $helpOption



# DEVELOPMENT NOTES
# ------------------
# Combine Begin and End Line Indexes
# Recursively traverse the directory
# Check different variations on Case - make case insenstive
# Need to type in Command Line parameters carefully!
# Pass down the IPL file name
# Need to ensure balance of comments - cannot remove starting comment, but leave ending comment
# and cause a compilation error!
# Add a new Copyright header
# String Compares are not case insensitive
# Remove the /*  and the */ for the 2nd and 2nd to last lines in the Copyright file
# Need to validate that extra comments are not removed causing compilation error
# Remove extra space that is not taken out.....when updating with IPL.txt
# Remove and update doesnot work occasionally
# typing only -files incorrectly causes loop
# Remove #s inside the Makefile while removing Copyright
# Log extensions not touched AT ALL
# "such source code"  vs "SOURCE CODE"
# Make the customer search criteria perform UPPER and LOWER CASE
# REARRANGE SO THAT EXECUTE COMMAND OCCURS FOR COPYRIGHT UPDATES ONLY
# DOES NOT CREATE BACKUP LOG FILES - IF RERUN, NEED TO SAVE DESIRED FILES BEFORE RERUNNING
# NON INTEL COPYRIGHT - DONOT REMOVE
# FIX THE REMOVAL OF SINGLE LINE COPYRIGHTS IN THE CODE
# ALL Checks are done in lower case
# Need to ensure that any copyright banner file is properly formatted AND HAS A "\n"
# at the end of EVERY LINE!!!! - Otherwise it maynot work!
# CLEANUP Presentation of data in STDOUT
# Take InitialReleaseDate from the command Line
# Delete the created backup files
# Need to log the files outside of the ones we already list - will be useful to determine missing files
# Autocreation of a newIPL.txt incase it is not automatically created because of noCopyrightFoundFiles
