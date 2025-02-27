from flask import Flask, render_template, request
import json
import scc
import os

app = Flask(__name__)

filenameCounter = 0
timeCounter = 0
def makeFileName():
    global filenameCounter
    filenameCounter+=1
    return str(filenameCounter).rjust(5,"0")

'''
def deleteFiles(directory):
    for filename in os.listdir(directory):
        file = os.path.join(directory, filename)
        try:
            os.remove(file)
        except Exception as e:
            print(f"Failed to delete {file}: {e}")
'''

@app.route("/",methods = ('GET','POST'))
def home():
    if request.method == 'POST':
        inputcode = request.json
        filename = makeFileName()
        scc.makeAssemFile(filename,inputcode['assemblerCode'])
        preshellcode = scc.makeShellCode(filename)
        return json.dumps({'data':scc.shellCodeParse(preshellcode)})
    
    else:
        return render_template('home.html')
    
    
if __name__ == "__main__":
    app.run(debug=True)
    