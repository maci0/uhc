from tkinter import *

# Create the main window
root = Tk()
root.title("Simple Tkinter Application")
root.geometry("1280x720")


def draw_pixel(x, y, color):
    canvas.create_line(x, y, x + 1, y, fill=color)

canvas = Canvas(root, width=1280, height=720)
canvas.create_line(10, 10, 20, 10, fill="#476042")

canvas.pack()


# Start the Tkinter event loop
root.mainloop()