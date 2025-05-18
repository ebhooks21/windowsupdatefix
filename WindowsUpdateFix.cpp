/**
* File Name: WindowsUpdateFix.cpp
* Created by: Eric Hooks
* Date of Creation: 1/10/2013
* Purpose: To attempt to fix the windows update process.
*
* Microsoft and Windows are property of the Microsoft Corporation.
* All other tools are property of thier respective owners.
*/
//Preprocessor Directive
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cstring>

//Using standard namespace
using namespace std;

//Structure Definition
struct System
{
	//Variable to store the Processor Type
	string procType;
	
	//Variable to store the OS version
	string osVer;
};

//Function Prototypes
void rebootSystem(); //Function to reboot the system
System getSystemInfo(); //Function to get the system info
System infoCheck(System sys);
void sysFileCheck(); //Function to do a system file check
void loadSysInfo(); //Function to load the system info
void pause(); //Function to pause the program
void outputSysInfo(System sys); //Function to output system information
void reregisterDlls(); //Function to re-register windows updates dlls

/**
* Main Function for the WindowsUpdateFixProgram.
*/
int main()
{
	//Variable for the system information
	System sys;
	char u_choice = 'y';
	
	//Change the command environment
	system("title Windows Update Fixer");
	system("color 3F");
	
	//Get the system information
	loadSysInfo();
	
	//Output message to the user
	cout << "Attempting to fix Windows Updates." << endl;
	cout << "If you didn't run this as Administrator, I would suggest closing this window" << endl;
	cout << "and running as Administrator now.  Press Enter if you wish to proceed...";
	
	//Wait for the user to press enter
	pause();
	
	//Output message to the user
	cout << endl << "Getting some information about your computer, please wait..." << endl;
	
	//Parse the system information and output to the user
	sys = getSystemInfo();
	outputSysInfo(sys);
	cout << "Done." << endl << endl;

	//Make sure the information is correct, 64-bit fix
	sys = infoCheck(sys);
	
	//Restart the Windows Update Service
	cout << "Restarting the Windows Update Service." << endl;
	system("net stop wuauserv");
	system("net start wuauserv");
	
	//Restart BITS
	cout << endl << "Restarting BITS." << endl;
	system("net stop bits");
	system("net start bits");
	
	//Re-register important DLLS
	reregisterDlls();
	
	//Stop Windows Updates
	cout << "Stopping Windows Updates Service." << endl;
	system("net stop wuauserv");
	
	//Fix Software distribution service
	cout << "Fixing Software Distribution Folder." << endl;
	system("ren %systemroot%\\SoftwareDistribution SD_OLD");
	cout << "Done." << endl << endl;
	
	//Start Windows Updates
	cout << "Starting Windows Update Service" << endl;
	system("net start wuauserv");
	
	//Delete Registry keys that go along with Windows Updates
	cout << "Removing Keys from the Registry... Ignore any error messages, these" << endl;
	cout << "keys may not exist in the Registry." << endl;
	system("reg delete HKLM\\Components\\PendingXmlIdentifier < y");
	system("reg delete HKLM\\Componetns\\XmlIdnetifier < y");
	system("reg delete HKLM\\Components\\AdvancedInstallersNeedResolving < y");
	cout << "Done." << endl << endl;
	
	//Check to see if the user wants to do a system file check
	cout << "Do you wish to perform a System File Check(Recommended)<y/n>?";
	cin >> u_choice;
	if(u_choice == 'y' || u_choice == 'Y')
	{
		cout << "Performing a System File Check..." << endl;
		if(sys.osVer == "5.1")
		{
			cout << "You may be asked to enter the Windows XP Service Pack 3 Disc. If so, " << endl;
			cout << "follow the on-screen instructions." << endl;
		}
		
		//Do the file check
		system("sfc /scannow");
		cout << "Press Enter when complete..." << endl;
		cin.ignore();
		pause();
	}
	
	//Check to see if the next steps are necessary
	cout << "Checking to see if the next steps are necessary..." << endl << endl;
	if(sys.osVer == "5.0" || sys.osVer == "5.1" || sys.osVer == "5.2")
	{
		//Reboot the system
		rebootSystem();

		//Return 0 to the OS, it is going to reboot anyways
		return 0;
	}
	
	//Disable anti-virus
	cout << "Please temporarily disable any Anti-Virus or Firewalls that may be running" << endl;
	cout << "and press Enter when ready..." << endl;
	cin.ignore();
	pause();
	
	//Disable UAC
	cout << "Please disable User Account Control." << endl;
	system("control nusrmgr.cpl");
	cout << "Press Enter when ready..." << endl;
	pause();
	
	//Get a new copy of the Windows Update Agent
	cout << "Downloading a new copy of the Windows Update Agent, when prompted please save to the desktop." << endl;
	if(sys.procType == "x86" || sys.procType == "X86")
	{
		system("explorer http://download.windowsupdate.com/v7/windowsupdate/redist/standalone/WindowsUpdateAgent30-x86.exe");
	}
	
	else if(sys.procType == "AMD64" || sys.procType == "x64" || sys.procType == "X64")
	{
		system("explorer http://download.windowsupdate.com/v7/windowsupdate/redist/standalone/WindowsUpdateAgent30-x64.exe");
	}
	cout << "Press Enter when the download is complete..." << endl;
	pause();
	
	//Reboot the system
	rebootSystem();
	
	//Return 0 to the OS
	return 0;
}

//Function to reboot the system
void rebootSystem()
{
	//Reboot the system
	cout << "Your computer will now restart, once it has finished please run the file" << endl;
	cout << "(If Any)downloaded in the last step." << endl;
	cout << "Press Enter when ready..." << endl;
	pause();
	system("shutdown -r -t 0");
}

//Function to re-register dlls
void reregisterDlls()
{
	//Re-register important DLLS
	cout << "Attempting re-registering Windows Update DLLS." << endl;
	
	system("regsvr32 /s wuapi.dll");
	system("regsvr32 /s wuaueng.dll");
	system("regsvr32 /s wuaueng1.dll");
	system("regsvr32 /s atl.dll");
	system("regsvr32 /s wucltui.dll");
	system("regsvr32 /s wups.dll");
	system("regsvr32 /s wups2.dll");
	system("regsvr32 /s wuweb.dll");
	
	//Output done to the user
	cout << "Done." << endl << endl;
}

//Function to output the system information
void outputSysInfo(System sys)
{
	//Output the information to the user
	if(sys.procType == "x86" || sys.procType == "X86")
	{
		cout << "Detected 32-bit processor." << endl;
	}
	
	else if(sys.procType == "AMD64" || sys.procType == "x64" || sys.procType == "X64")
	{
		cout << "Detected 64-bit processor." << endl;
	}
	
	if(sys.osVer == "5.0")
	{
		cout << "Detected Windows 2000." << endl;
	}
	
	else if(sys.osVer == "5.1")
	{
		cout << "Detected Windows XP." << endl;
	}
	
	else if(sys.osVer == "5.2")
	{
		cout << "Detected Windows Server 2003." << endl;
	}
	
	else if(sys.osVer == "6.0")
	{
		cout << "Detected Windows Vista or Windows Server 2008." << endl;
	}
	
	else if(sys.osVer == "6.1")
	{
		cout << "Detected Windows 7 or Windows Server 2008 R2." << endl;
	}
	
	else if(sys.osVer == "6.2")
	{
		cout << "Detected Windows 8 or Windows Server 2012." << endl;
	}
}

//Function to check the system information
System infoCheck(System sys)
{
	//Check the system information
	char u_choice;
	bool kLoop = true;

	cout << "Is this information correct<y/n>?";
	cin >> u_choice;
	cout << endl;

	if(u_choice == 'n' || u_choice == 'N')
	{
		cout << "Is the processor information correct<y/n>?";
		cin >> u_choice;
		cout << endl;

		if(u_choice == 'n' || u_choice == 'N')
		{
			while(kLoop)
			{
				cout << "Is this system 32-bit or 64-bit?" << endl;
				cout << "a. 32-bit" << endl;
				cout << "b. 64-bit" << endl;
				cout << "Choice: ";
				cin >> u_choice;
				cout << endl;
				
				if(u_choice == 'a' || u_choice == 'b' || u_choice == 'A' || u_choice == 'B')
				{
					kLoop = false;
					//Set the correct processor info
					if(u_choice == 'a' || u_choice == 'A')
					{
						sys.procType = "x86";
					}

					else
					{
						sys.procType = "AMD64";
					}
				}
			}
		}

		cout << "Is the operating system information correct<y/n>?";
		cin >> u_choice;
		cout << endl;

		if(u_choice == 'n' || u_choice == 'N')
		{
			kLoop = true;
			while(kLoop)
			{
				cout << "Please select the correct operating system:" << endl;
				cout << "a. Windows 2000" << endl;
				cout << "b. Windows XP" << endl;
				cout << "c. Windows Server 2003" << endl;
				cout << "d. Windows Vista or Windows Server 2008" << endl;
				cout << "e. Windows 7 or Windows Server 2008 R2" << endl;
				cout << "f. Windows 8 or Windows Server 2012" << endl;
				cout << "Choice: ";
				cin >> u_choice;
				cout << endl;
				
				if(u_choice == 'a' || u_choice == 'b' || u_choice == 'A' || u_choice == 'B'
				  || u_choice == 'c' || u_choice == 'C' || u_choice == 'd' || u_choice == 'D'
				  || u_choice == 'e' || u_choice == 'E' || u_choice == 'f' || u_choice == 'E')
				{
					kLoop = false;
					//Set the correct processor info
					if(u_choice == 'a' || u_choice == 'A')
					{
						sys.osVer = "5.0";
					}

					else if(u_choice == 'b' || u_choice == 'B')
					{
						sys.osVer = "5.1";
					}

					else if(u_choice == 'c' || u_choice == 'C')
					{
						sys.osVer = "5.2";
					}

					else if(u_choice == 'd' || u_choice == 'D')
					{
						sys.osVer = "6.0";
					}
				
					else if(u_choice == 'e' || u_choice == 'E')
					{
						sys.osVer = "6.1";
					}

					else if(u_choice == 'f' || u_choice == 'F')
					{
						sys.osVer = "6.2";
					}
				}
			}
		}
	}

	//Return the System
	return sys;		
}

//Function to pause the program
void pause()
{
	cin.get();
}

//Function to load the system info
void loadSysInfo()
{
	//Get the system info
	system("echo %processor_architecture% > type.txt");
	system("ver > ver.txt");
}

/**
* Function to get the system information.
*/
System getSystemInfo()
{	
	//Parse the information
	System sys;
	string ver;
	char buffer[60];
	fstream file1("type.txt", fstream::in);
	fstream file2("ver.txt", fstream::in);
	
	//Make sure that the first file is open
	//Get the processor information
	if(file1.is_open())
	{
		//Get the info from the file
		file1.getline(buffer, 60);
		ver = string(buffer);
		sys.procType = ver.substr(0, (ver.length() - 1));
	}
	
	else
	{
		cout << "An error has occured, please try again." << endl;
		exit(-1);
	}
	
	//Make sure the second file is open
	//Get the os version
	if(file2.is_open())
	{
		//Get the info from the file
		//Get and skip the first line, it is blank
		file2.getline(buffer, 60);
		memset(buffer, ' ', 60);
		file2.getline(buffer, 60);
		ver = string(buffer);
		
		//Parse out the info
		sys.osVer = ver.substr((ver.length() - 9), 3);
	}
	
	else
	{
		cout << "An error has occured, please try again." << endl;
		exit(-1);
	}

	//Close and delete the files
	file1.close();
	file2.close();
	system("del type.txt");
	system("del ver.txt");
	
	//Return the System structure
	return sys;
}
