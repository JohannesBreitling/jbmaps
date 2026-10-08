
from turtle_wrapper import *

# - - - GRAPH VISUALIZATION - - -

root = tk.Tk()
SCREEN_X = root.winfo_screenwidth() - 100
SCREEN_Y = root.winfo_screenheight() - 100

# Format: pyhton3 visualization.py <base instance path> <mode 1> <path 1 start> <path 1 end> <color 1> ...
len_args = len(sys.argv)
args = sys.argv

if (len_args - 3) % 4 != 0:
    raise Exception("Wrong format of arguments (" + str(len_args) + "). Expected <base instance path> <mode 1> <path 1 start> <path 1 end> <color 1> ...")

wrapper = TurtleGraphWrapper(args[1], args[2], SCREEN_X, SCREEN_Y)

if len_args < 4:
   wrapper.done()
   sys.exit(0)

i = 3
while i < len_args:
    mode = args[i]
    path_starts = args[i + 1]
    path_ends = args[i + 2]
    color = args[i + 3]

    if (mode == "path"):
        wrapper.drawPathFromFile(path_starts, path_ends, color)

    if (mode == "edges"):
        wrapper.drawEdgesFromFile(path_starts, path_ends, color)
    
    if (mode != "edges" and mode != "path"):
        raise Exception("Invalid mode: " + mode)

    i += 4

wrapper.done()
