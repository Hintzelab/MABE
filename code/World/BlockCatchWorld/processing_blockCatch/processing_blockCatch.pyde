# visualization for block catch world
# green = blocks that should be caught
# red = blocks that should be missed
# yellow = paddle with sensor
# outline yellow = paddle without sensor
# dark yellow = area seen by paddle sensors

# enter name of visualization file here.
fileName = 'c:/Users/cliff/MABE_FOR_MITCH/data/CatchPassVisualize_0.txt'
#fileName = 'c:/Users/cliff/WORK/INFO_A_NEW_START/RNN_sparse_discrete/CatchPassVisualize_0.txt'
#fileName = 'c:/Users/cliff/WORK/NBack8_IT/data/CatchPassVisualize_0.txt'

# enter size of window here (block size will be fit to window)
windowX = 600
windowY = 600

fontSize = 30

# speed of playback when launched... can be adjusted with left and right arrows
fps = 1


# global vars
fileHandle = open(fileName, 'r+')

paused = 1 # used to tell if we are 

worldX = 0
worldY = 0

nrOfLeftSensors = 0
gapWidth = 0
nrOfRightSensors = 0
gupdate = 0
orgID = 0

gridWidth = 0 # set in setup
gridHeight = 0 # set in setup
maxX = 0 # set in setup
maxY = 0 # set in setup
offset = 0

def mousePressed():
    global paused
    if paused == 1:
        noLoop()
        paused = 0
    elif paused == 0:
        loop()
        paused = 1

def keyPressed():
    global fps
    if key == CODED:
        if (keyCode == RIGHT):
            fps = fps * 2
            frameRate(fps)
        if (keyCode == LEFT):
            fps = fps / 2;
            if (fps<1):
                fps = 1;
            frameRate(fps);

def readNextLineFromFile():
    global fileHandle
    line = fileHandle.readline()#.strip()
    #############################################
    # DO NOT SKIP EMPTY LINES
    #while (line == ""):
    #    line = fileHandle.readline()#.strip()
    #############################################
    splitLine = split(line,',')
    return splitLine

def setup():
    global fileHandle
    global fps
    global worldX
    global worldY
    global gridHeight
    global gridWidth
    global offset
    global windowX
    global windowY
    global maxY
    global maxX
    
    global nrOfLeftSensors
    global gapWidth
    global nrOfRightSensors
    global gupdate
    global orgID


    background(0)
    stroke(0)
    frameRate(fps)
    #smooth(30)
    splitLine = readNextLineFromFile() # read past reset
    splitLine = readNextLineFromFile()
    maxX = int(splitLine[0])
    maxY = int(splitLine[1])
    update = splitLine[2]
    orgID = splitLine[3]
    gridWidth = int(windowX/maxX) * 1
    gridHeight = int((windowY - (fontSize * 3))/(maxY+1))
    gridHeight = int((windowY - 0)/(maxY+1))

    size(windowX, windowY)

  
def draw():
    global fps
    global gridSize
    global maxY
    global maxX
    global textSize
    global fileHandle

    ##
    splitLine = readNextLineFromFile()
    if splitLine[0][0] == 'E':
        # if end of data, reset file and pause
        print ("WTF")
        fileHandle = open(fileName, 'r+')
        splitLine = readNextLineFromFile() # get past reset
        splitLine = readNextLineFromFile() # get past init line
        fill(255,255,255,255)
        textSize(fontSize)
        text("at End of Data.\nLeft click to replay.", 20, fontSize * 1.5)
        mousePressed()

    else:
        background(0)
        worldX = int(splitLine[0])
        time = int(splitLine[1])
        cORm = splitLine[2] # catch or miss
        correct = splitLine[3]
        incorrect = splitLine[4]
    
        pattern = readNextLineFromFile()
        converted_pattern = [int(x) for x in pattern[0:-1]]
        patternHoles = readNextLineFromFile()
        converted_patternHoles = [int(x) for x in patternHoles[0:-1]]
        sensors = readNextLineFromFile()
        converted_sensors = [int(x) for x in sensors[0:-1]]
        gap = readNextLineFromFile()
        converted_gap = [int(x) for x in gap[0:-1]]
        
        for y in range(maxY+1):
            for x in range(worldX):
                fill(30,30,30,255)
                strokeWeight(2)
                stroke(0,0,0,255)
                rect(x * gridWidth, y * int(gridHeight),gridWidth-2,int(gridHeight)-2)

                if x in converted_sensors:
                    fill(150,150,255,60)
                    strokeWeight(2)
                    stroke(0,0,0,255)

                    if y == (maxY):
                        fill(150,150,255,255)
                    rect(x * gridWidth, y * int(gridHeight),gridWidth-2,int(gridHeight)-2)
    
                if x in converted_gap:
                    if y == (maxY):
                        fill(255,255,255,90)
                        strokeWeight(2)
                        stroke(0,0,0,255)
                        rect(x * gridWidth, y * int(gridHeight),gridWidth-2,int(gridHeight)-2)
    
                if y == (maxY - int(time)) and x in converted_pattern:
                    if cORm[0] == 'c':
                        fill(0,255,0,255)
                    else:
                        fill(255,0,0,255)
                    rect(x * gridWidth, y * int(gridHeight),gridWidth-2,int(gridHeight)-2)
    
                if y == (maxY - int(time)) and x in converted_patternHoles:
                    if cORm[0] == 'c':
                        fill(0, 0, 0, 0)
                        strokeWeight(2)
                        stroke(0,0,0,255)
                    else:
                        fill(0, 0, 0, 0)
                        strokeWeight(2)
                        stroke(0,0,0,255)
                    rect(x * gridWidth, y * int(gridHeight),gridWidth-2,int(gridHeight)-2)
    
        fill(255,255,255,255)
        textSize(fontSize)
    
        #text("time: " + str(time) + "  correct: " + str(correct) + "  incorrect: " + str(incorrect), 20, (maxY)* int(gridHeight) + fontSize * 2.5)
        #text("fps: " + str(fps) + "   (use left and right arrows to change)", 20, (maxY)* int(gridHeight) + fontSize * 2.5)
