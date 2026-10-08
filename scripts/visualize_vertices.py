
from turtle_wrapper import *

# - - - VISUALIZE VERTICES - - -

root = tk.Tk()
SCREEN_X = root.winfo_screenwidth() - 100
SCREEN_Y = root.winfo_screenheight() - 100

# Format: pyhton3 visualize_vertices.py <base instance path start> <base instance path end> <path_to_vertices>
len_args = len(sys.argv)
args = sys.argv

if len_args != 4:
    raise Exception("Wrong format of arguments (" + str(len_args) + "). Expected <base instance path start> <base instance path end> <path to vertices>")

wrapper = TurtleGraphWrapper(args[1], args[2], SCREEN_X, SCREEN_Y)

if len_args < 4:
   wrapper.done()
   sys.exit(0)

# Draw the vertices to the screen
wrapper.drawVerticesWithNumbers(args[3])

wrapper.done()
