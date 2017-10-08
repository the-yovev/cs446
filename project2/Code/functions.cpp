#include<cstdlib>
#include<iostream>
#include<fstream>
#include<string>
#include<map>
#include<vector>
#include<chrono>
#include<iomanip>
#include<thread>

#include<windows.h>

#include"functions.hpp"
#include"MemoryFunction.h"

using namespace std;
using namespace std::chrono;

const int MONITOR = 0;
const int OUTPUT_FILE = 1;
const int MONITOR_AND_OUTPUT_FILE = 2;
const char SPACE = ' ';
const char NEWLINE = '\n';
const char LEFT_PARENTHESE = '(';
const char RIGHT_PARENTHESE = ')';
const char HYPHEN = '-';

const int START = 0;
const int READY = 1;
const int RUNNING = 2;
const int WAITING = 3;
const int EXIT = 4;

// calculate the metadata metrics by "mapping" metadata descriptors to their
// corresponding components
int calculateCycleTime(map<string, int>& cycleTimes, vector<string>& mdd, 
                       vector<int>& mdc, const int index)
{
   if(mdd[index] == "run")
   {
      return mdc[index] * cycleTimes["Processor"];
   }
   else if(mdd[index] == "hard drive")
   {
      return mdc[index] * cycleTimes["Hard drive"];
   }
   else if(mdd[index] == "keyboard")
   {
      return mdc[index] * cycleTimes["Keyboard"];
   }
   else if(mdd[index] == "mouse")
   {
      return mdc[index] * cycleTimes["Mouse"];
   }
   else if(mdd[index] == "monitor")
   {
      return mdc[index] * cycleTimes["Monitor"];
   }
   else if(mdd[index] == "speaker")
   {
      return mdc[index] * cycleTimes["Speaker"];
   }
   else if(mdd[index] == "block")
   {
      return mdc[index] * cycleTimes["Memory"];
   }
   else if(mdd[index] == "allocate")
   {
      return mdc[index] * cycleTimes["Memory"];
   }
   else if(mdd[index] == "printer")
   {
      return mdc[index] * cycleTimes["Printer"];
   }
   else
   {
      return EXIT_FAILURE;
   }
}

// calculate the value for sleep / wait function
double calculateSleepTime(map<string, int>& cycleTimes, vector<string>& mdd, 
                    vector<int>& mdc, const int index)
{
   if(mdd[index] == "start" || mdd[index] == "end")
   {
      return (double)0;
   }
   else if(mdd[index] == "run")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Processor"]);
   }
   else if(mdd[index] == "hard drive")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Hard drive"]);
   }
   else if(mdd[index] == "keyboard")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Keyboard"]);
   }
   else if(mdd[index] == "mouse")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Mouse"]);
   }
   else if(mdd[index] == "monitor")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Monitor"]);
   }
   else if(mdd[index] == "speaker")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Speaker"]);
   }
   else if(mdd[index] == "block")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Memory"]);
   }
   else if(mdd[index] == "allocate")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Memory"]);
   }
   else if(mdd[index] == "printer")
   {
      return (double)((double)mdc[index] * (double)cycleTimes["Printer"]);
   }
   else
   {
      return EXIT_FAILURE;
   }
}

// checks the configuration file for any potential errors
void checkConfigurationFile(ifstream& fin, const char *argv[])
{
   // declare variables
   string s = argv[1];
   int found = s.find(".conf");
   
   // checks extension of configuration file
   if(found == -1)
   {
      throw -1;
   }
   // checks for a valid filename (open file)
   if(fin.is_open() == false)
   {
      throw 0;
   }
   // checks if the file is empty
   if(!(fin >> s))
   {
      throw -3;
   }
}

// checks the metadata file for any potential errors
void checkMetadataFile(ifstream& fin, string mdfp)
{
   // declare variables
   string s;
   int found = mdfp.find(".mdf");
   
   // checks extension of metadata file
   if(found == -1)
   {
      throw -2;
   }
   // checks for a valid filename (open file)
   if(fin.is_open() == false)
   {
      throw 0;
   }
   // checks if the file is empty
   if(!(fin >> s))
   {
      throw -4;
   }
}

// gets the various component cycle times from the configuration file
void getComponentCycleTimes(ifstream& fin, map<string, int>& cycleTimes)
{
   // declare variables
   string component;
   int cycleTime;
   
   // prime while loop
   fin >> component;
      
   while(component != "System")
   {
      // check if the component is a hard drive
      if(component == "Hard")
      {
         // append "drive" to key / component name
         string drive;
         fin >> drive;
         component.append(" ");
         component.append(drive);
      }
      
      // prime filestream and get the cycle time for the current component
      fin.ignore(256, ':');
      fin >> cycleTime;
      
      // add element to vector
      cycleTimes.emplace(component, cycleTime);
      
      // get next component
      fin >> component;
   }
}

// gets log type from configuration file
void getLogType(ifstream& fin, int& lt)
{
   // declare variables
   string s, temp;
   
   // build string
   fin >> s;
   s.append(" ");
   fin >> temp;
   s.append(temp);
   s.append(" ");
   fin >> temp;
   s.append(temp);
   
   // check string against possible options to determine log type
   if(s == "Log to Monitor")
   {
      lt = 0;
   }
   else if(s == "Log to File")
   {
      lt = 1;
   }
   else if(s == "Log to Both")
   {
      lt = 2;
   }
}

// gets the path that the log file should be output to
void getLogFilepath(ifstream& fin, string& lfp)
{
   // prime filestream
   fin.ignore(256, ':');
   
   fin >> lfp;
}

// uses modular functions to get log type and filepath from configuration file
void getLogTypeAndFilepath(ifstream& fin, string& lfp, int& lt)
{
   getLogType(fin, lt);
   
   getLogFilepath(fin, lfp);
}

// gets the filepath of the metadata file from the configuration file
void getMetadataFilepath(ifstream& fin, string& mdfp)
{
   // declare variable
   char c;
   
   // prime filestream
   fin.ignore(256, ':');
   fin.ignore(256, ':');
   fin.get(c);
   
   fin >> mdfp;
}

void getSystemMemory(ifstream& fin, int& sm, string& units)
{
   // declare variables
   char c;
   
   // read character by character up to left parenthese
   while(c != LEFT_PARENTHESE)
   {
      fin >> c;
   }
   
   // read character by character until right parenthese
   while(fin.peek() != RIGHT_PARENTHESE)
   {
      // read in character and concatenate to string to build unit
      fin >> c;
      units += c;
   }
   // garbage (left parenthese, colon)
   fin >> c;
   fin >> c;
   
   // read in memory size
   fin >> sm;
}

// handles all errors given the error code by displaying a corresponding
// message and terminating the program
int handleErrors(const int e)
{
   if(e == 0)
   {
      cout << "ERROR CODE 0; FILE NOT FOUND" << endl;
      return EXIT_FAILURE;
   }
   if(e == -1)
   {
      cout << "ERROR CODE -1; INVALID CONFIGURATION FILE EXTENSION" << endl;
      return EXIT_FAILURE;
   }
   if(e == -2)
   {
      cout << "ERROR CODE -2; INVALID METADATA FILE EXTENSION" << endl;
      return EXIT_FAILURE;
   }
   if(e == -3)
   {      
      cout << "ERROR CODE -3, EMPTY CONFIGURATION FILE" << endl;
      return EXIT_FAILURE;
   }
   if(e == -4)
   {  
      cout << "ERROR CODE -4, EMPTY METADATA FILE" << endl;
      return EXIT_FAILURE;
   }
   if(e == -5)
   {
      cout << "ERROR CODE -5; INVALID(LOWERCASE) OR MISSING METADATA CODE" << endl;
      return EXIT_FAILURE;
   }
   if(e == -6)
   {
      cout << "ERROR CODE -6; INVALID(TYPO) OR MISSING METADATA DESCRIPTOR" << endl;
      return EXIT_FAILURE;
   }
   if(e == -7)
   {
      cout << "ERROR CODE -7; INVALID(NEGATIVE) OR MISSING METADATA CYCLES" << endl;      
      return EXIT_FAILURE;
   }
}

// checks the logtype variable and uses modular functions to log accordingly
void log(map<string, int>& cycleTimes, vector<string>& mdd, vector<char>& mdco,
         vector<int>& mdcy, const string logFilepath, const int logType, 
         const int count, const int sm, const int i,
         high_resolution_clock::time_point t1, high_resolution_clock::time_point t2,
         duration<double> time_span)
{
   if(logType == MONITOR)
   {
      logToMonitor(cycleTimes, mdd, mdco, mdcy,
                   logFilepath, logType, count, sm, i, t1, t2, time_span);
   }
   else if(logType == OUTPUT_FILE)
   {
      logToFile(cycleTimes, mdd, mdco, mdcy,
                logFilepath, logType, count, sm);
   }
   else if(logType == MONITOR_AND_OUTPUT_FILE)
   {
      logToMonitor(cycleTimes, mdd, mdco, mdcy,
                   logFilepath, logType, count, sm, i, t1, t2, time_span);
                   
      logToFile(cycleTimes, mdd, mdco, mdcy,
                logFilepath, logType, count, sm);
   }
}

// logs all data to the given file in the prescribed example format
void logToFile(map<string, int>& cycleTimes, vector<string>& mdd, vector<char>& mdco,
               vector<int>& mdcy, const string logFilepath, const int logType, 
               const int count, const int sm)
{
   
}

// logs all data to the monitor in the prescribed example format
void logToMonitor(map<string, int>& cycleTimes, vector<string>& mdd, vector<char>& mdco,
                  vector<int>& mdcy, const string lfp, const int lt, 
                  const int count, const int sm, const int i,
                  high_resolution_clock::time_point t1, high_resolution_clock::time_point t2,
                  duration<double> time_span)
{
   printTime(t1, t2, time_span);
   
   if(mdco[i] == 'S')
   {
      cout << "Simulator program ";
      
      if(mdd[i] == "start")
      {
         cout << "starting" << endl;
      }
      else if(mdd[i] == "end")
      {
         cout << "ending";
      }
   }
   
   // check metadata code and output data accordingly
   if(mdco[i] == 'A')
   {
      if(mdd[i] == "start")
      {
         for(int j = 0; j < 2; j++)
         {
            if(j == 0)
            {
               cout << "OS: preparing process 1" << endl;
            }
            else if(j == 1)
            {
               Sleep(calculateSleepTime(cycleTimes, mdd, mdcy, i));
               
               printTime(t1, t2, time_span);
               
               cout << "OS: starting process 1" << endl;
            }
         }
      }
      else if(mdd[i] == "end")
      {
         cout << "OS: removing process 1" << endl;
      }
   }
   
   // check metadata code and output data accordingly
   if(mdco[i] == 'P')
   {
      for(int j = 0; j < 2; j++)
      {
         if(j == 0)
         {
            cout << "Process 1: start processing action" << endl;
         }
         else if(j == 1)
         {
            Sleep(calculateSleepTime(cycleTimes, mdd, mdcy, i));
            
            printTime(t1, t2, time_span);
            
            cout << "Process 1: end processing action" << endl;
         }
      }
   }
   
   // check metadata code and output data accordingly
   if(mdco[i] == 'M')
   {
      if(mdd[i] == "allocate")
      {
         for(int j = 0; j < 2; j++)
         {
            if(j == 0)
            {
               cout << "Process 1: allocating memory" << endl;
            }
            else if(j == 1)
            {
               Sleep(calculateSleepTime(cycleTimes, mdd, mdcy, i));
               
               printTime(t1, t2, time_span);
               
               cout << "memory allocated at 0x" 
                    << setfill('0') << setw(8) 
                    << allocateMemory(sm) << endl;
            }
         }
      }
      else if(mdd[i] == "block")
      {
         for(int j = 0; j < 2; j++)
         {            
            if(j == 0)
            {
               cout << "Process 1: start memory blocking" << endl;
            }
            else if(j == 1)
            {
               Sleep(calculateSleepTime(cycleTimes, mdd, mdcy, i));
               
               printTime(t1, t2, time_span);
               
               cout << "Process 1: end memory blocking" << endl;
            }
         }
      }
   }
   
   // check metadata code and output data accordingly
   if(mdco[i] == 'O' || mdco[i] == 'I')
   {
      for(int j = 0; j < 2; j++)
      {         
         if(j == 0)
         {
            cout << "Process 1: start " << mdd[i];
            if(mdco[i] == 'O')
            {
               cout << " output" << endl;
            }
            else if(mdco[i] == 'I')
            {
               cout << " input" << endl;
            }
         }
         else if(j == 1)
         {
            Sleep(calculateSleepTime(cycleTimes, mdd, mdcy, i));
            
            printTime(t1, t2, time_span);
            
            cout << "Process 1: end " << mdd[i];
            if(mdco[i] == 'O')
            {
               cout << " output" << endl;
            }
            else if(mdco[i] == 'I')
            {
               cout << " input" << endl;
            }
         }
      }
   }
}

void myWait(int ms)
{
//    std::this_thread::sleep_for(milliseconds(ms));
}

void printTime(high_resolution_clock::time_point t1, 
               high_resolution_clock::time_point t2,
               duration<double> time_span)
{
   t2 = chrono::high_resolution_clock::now();
   
   time_span = duration_cast<duration<double>>(t2 - t1);
   
   cout << fixed << setprecision(6) << time_span.count() << SPACE << HYPHEN << SPACE;
}

// uses modular functions to read the entire configuration file
void readConfigurationFile(ifstream& fin, map<string, int>& cycleTimes,
                           string& mdfp, string& lfp, int& lt, int& sm, string& units)
{
   getMetadataFilepath(fin, mdfp);
   
   getComponentCycleTimes(fin, cycleTimes);
   
   getSystemMemory(fin, sm, units);
   
   getLogTypeAndFilepath(fin, lfp, lt);
}

// reads in a single piece of metadata from the metadata file
void readOneMeta(ifstream& fin, vector<string>& mdd, vector<char>& mdc, 
                 vector<int>& cycles)
{
   // declare variables
   string mddTemp;
   char mdcTemp, lp, rp, mddAppend;
   int cyclesTemp = -999;
   
   // get metadata code
   fin >> mdcTemp >> lp;
   
   // check if the metadata code is lowercase or an invalid character
   if(mdcTemp < 'A' || mdcTemp > 'Z')
   {
      throw -5;
   }
   
   // add element to vector
   mdc.push_back(mdcTemp);
   
   // prime while loop
   fin >> mddAppend;
   while(mddAppend != ')')
   {
      // construct the metadata descriptor string character by character 
      mddTemp += mddAppend;
      
      // check if the component is a hard drive
      if(mddTemp == "hard")
      {
         mddTemp += SPACE;
      }
      fin >> mddAppend;
   }
   
   // check for an invalid metadata descriptor
   if(!(mddTemp == "start" || mddTemp == "end" || mddTemp == "run" ||
      mddTemp == "hard drive" || mddTemp == "keyboard" || mddTemp == "printer" ||
      mddTemp == "monitor" || mddTemp == "allocate" || mddTemp == "block" ||
      mddTemp == "mouse" || mddTemp == "speaker"))
   {
      throw -6;
   }
   
   // add element to vector
   mdd.push_back(mddTemp);
   rp = mddAppend;
   
   // check if the number of cycles is missing
   if(fin.peek() == ';')
   {
      throw -7;
   }
   
   fin >> cyclesTemp;
   
   // check if the number of cycles is negative
   if(cyclesTemp < 0)
   {
      throw -7;
   }
   
   // add element to vector
   cycles.push_back(cyclesTemp);
}

// uses a modular function to read the entire metadata file
void readMetadataFile(ifstream& fin, vector<string>& mdd, vector<char>& mdc, 
                      vector<int>& cycles, int& count)
{
   // declare variables
   char c;
   count = 0;
   
   // prime filestream
   fin.ignore(256, NEWLINE);
   
   while(c != '.')
   {
      readOneMeta(fin, mdd, mdc, cycles);
      count++;
      fin >> c;
   }
}