#include <iostream>
#include <string>
#include <fstream>
#include <getopt.h>
#include <sstream>
using namespace std;

static int MAX_FILENAMELENGTH = 30;
static const char* const short_options = "hf:Ac";
static const struct option long_options[] = {
        {"att",no_argument,0,'A'},
        {"help",no_argument,0,'h'},
        {"line",required_argument,0,'l'},
        {NULL,0,NULL,0}
    };

static const char* const Help = 
"Usage:\n\tShellcodeParser options\nOptions:\n\t-h\t\tprint this summary\n\t-f [file]\t\tselect the file(must)\n\t-c\t\tif file is c file\n\t-A --att \tconvert att syntax (default: intel)\n\t-l [LINES] \tmodify object file dump lines(default: 100)";

static bool isATT = false;
static bool needCompile = false;
static int objPirntLine = 100;
static string fileList[] = {"","","",""}; //fileList[0]: targetfile fileList[1]: .o , fileList[2]: binary, fileLIst[3]: binary_tmp
static string tfile = "";

void setFileList(string& targetFileName, string& outputfileName){
    fileList[0] = targetFileName;
    fileList[1] = outputfileName + ".o";
    fileList[2] = outputfileName;
    fileList[3] = outputfileName+"_tmp";
}
void optionManager(int argc, char* argv[], const char* const short_options, const struct option long_options[],int* __longnid ){
    int next_option = 0;

    while(next_option != -1){
        next_option = getopt_long(argc, argv, short_options, long_options,NULL);
        switch(next_option){
            case 'h':
                cout << Help<<endl;
                exit(EXIT_FAILURE);
            case 'A':
                isATT = true;
                break;
            case 'l':
                objPirntLine = stoi(optarg);
                break;
            case 'f':
                tfile = optarg;
                break;
            case 'c':
                needCompile = true;
                break;
            case -1:
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
}

void assembleFile(const bool isATT, const string& targetFileName, const string& objectFileName){
    char command1[120];
    if (isATT){
        sprintf(command1,"as -o %s.o %s",objectFileName.c_str(), targetFileName.c_str());
    }else{
        sprintf(command1,"nasm -f elf64 -o %s.o %s",objectFileName.c_str(), targetFileName.c_str());
    }
    system(command1);
}

void compileFile(const bool isATT, const string& targetFileName, const string& objectFileName){     //need to add compile C file
    char command1[120];

    if (needCompile){
        sprintf(command1,"gcc -S %s -O0 -masm=intel",targetFileName.c_str());

        system(command1);
        assembleFile(isATT,objectFileName+".s",objectFileName);
    }else{
        assembleFile(isATT,targetFileName,objectFileName);
    }
}

void linkFile(const string& objectFileName, const string& binaryFileName){
    char command[120];

    sprintf(command,"ld -o %s %s",binaryFileName.c_str(),objectFileName.c_str());
    system(command);
}

void printObjDump(const string& binaryFileName, const int objPrintLine){
    char command[120];
    sprintf(command, "objdump -d %s | grep \\<_start\\> -A %d",binaryFileName.c_str(), objPrintLine);
    system(command);
}

void printObjDump(const string& binaryFileName, const int objPrintLine, const string& tmpFileName){
    char command[120];
    sprintf(command, "objdump -d %s | grep \\<_start\\> -A %d > %s",binaryFileName.c_str(), objPrintLine, tmpFileName.c_str());
    system(command);
}

void removeNewFiles(const string& objectFileName, const string& binaryFileName, const string& tmpFileName){
    char command[120];
    sprintf(command, "rm %s %s %s",objectFileName.c_str(), binaryFileName.c_str(), tmpFileName.c_str());
    system(command);
}

stringstream writeShellcode(string& tmpFileName){
    stringstream shellcode;
    string trash;
    char tmp;
    bool isWriting = false;
    bool islinestart = false;
    int index = 0;
    int nextInsert = -1;

    ifstream FileReader(tmpFileName,ios_base::in);

    getline(FileReader,trash);
    while(!FileReader.eof()){
        FileReader.get(tmp);
        ++index;
        if (islinestart && index < 32) {
            if (tmp == '\x20' || tmp == '\x09') {
                isWriting = true;
                nextInsert = 2;
            }else if(tmp == '\x0a'){
                index = -1;
                isWriting = false;
                islinestart = false;
            }
            else {
                if (isWriting) {
                    if (nextInsert == 2) {
                        shellcode << "\\x" << tmp;
                    }
                    else {
                        shellcode << tmp;
                    }
                    nextInsert -= 1;
                    if (nextInsert == 0){
                        isWriting = false;
                    }
                }
            }
            }
        else if (index < 10) {
            if (tmp == '\x3a')
                islinestart = true;
                isWriting = true;
            continue;
        }
        else {
            index = -1;
            getline(FileReader, trash);
            isWriting = false;
            islinestart = false;
        }
    }
    return shellcode;
}

void printShellcode(const string& shellcode){
    cout <<"\nShellcode:\t"<<shellcode<<endl;
}


int main(int argc, char* argv[]){
    if (argc == 1){
        cerr<<"Usage: ShellcodeParser options"<<endl; 
        exit(EXIT_FAILURE);
        
    }
    optionManager(argc, argv, short_options, long_options, NULL);

    int position = tfile.find_last_of(".");

    string targetfilename = tfile;
    string filetype = tfile.substr(position+1);
    string outputfilename = tfile.substr(0,position);

    if(tfile == ""){
        cerr << "Use -f option to select a file"<<endl;
        exit(EXIT_FAILURE);
    }else if (targetfilename.length() > MAX_FILENAMELENGTH){
        cerr<<"\n\tFile name should be shorter than "<<MAX_FILENAMELENGTH<<endl<<endl;
        exit(EXIT_FAILURE);
    }else if (filetype !="s" && filetype !="asm" && filetype !="c"){
        cerr<<"\n\tOnly .s or .asm or .c file can be inverted to shellcode\n"<<endl;
        exit(EXIT_FAILURE);
    }
    setFileList(targetfilename, outputfilename);

    try{
        compileFile(isATT,fileList[0],fileList[2]);
        linkFile(fileList[1],fileList[2]);
        printObjDump(fileList[2],objPirntLine);
        printObjDump(fileList[2],objPirntLine,fileList[3]);
        printShellcode(writeShellcode(fileList[3]).str());
        removeNewFiles(fileList[1],fileList[2],fileList[3]);
    }catch(exception& e){
        cerr<<e.what()<<endl;
        exit(EXIT_FAILURE);
    }
}