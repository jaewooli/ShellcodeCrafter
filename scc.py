import subprocess

FOLDER_NAME = "scc_assem_files"

#init the configuration of SCC
#def sccInit(foldername:str = FOLDER_NAME):
#    os.mkdir(foldername)
#    FOLDER_NAME = foldername


def makeAssemFile(filename:str,data:str):
    f = open(FOLDER_NAME+"/"+filename+".s",'wb')
    f.write(data.encode())
    f.close()
    
def makeShellCode(filename: str):
    result = subprocess.run(['./scc', '-f', FOLDER_NAME + "/"+ filename+'.s'], capture_output=True, text= True)
    if result.returncode ==0:
        return result.stdout
    else:
        return result.stderr

#this method prases only the ShellCode which is made by function makeShellCode.
def shellCodeParse(shellcode: str):
    shellcode_index = shellcode.find("Shellcode:")
    return shellcode[shellcode_index+11:]