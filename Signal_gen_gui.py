import tkinter as tk
from tkinter import scrolledtext, messagebox
import sys
from tkinter import ttk
import serial

# Class to handle serial communication with Arduino
class START_COM:
    def __init__(self, arduino_port: str, baud_rate: int):
        self.arduino_port = arduino_port  # Serial port of the Arduino
        self.baud_rate = baud_rate  # Baud rate for serial communication
        self.ser = None  # Serial object
        self.data = ""  # Variable to store incoming data
        self.reading_thread = None  # Placeholder for future threading implementation

    def open_coms(self):
        """
        Open the serial port.
        """
        try:
            self.ser = serial.Serial(self.arduino_port, self.baud_rate, timeout=1)
        except Exception as e:
            # Display an error message if the port fails to open
            messagebox.showerror("Error", f"Failed to open port due to: {e}")

    def read_message(self) -> None:
        """
        Read a line from the serial port.
        """
        self.string = ""
        try:
            self.data = self.ser.readline().decode('utf-8').strip()  # Read and decode the data
            if self.data:
                self.data = self.data.strip()  # Strip any extraneous whitespace
                self.read = 0        
                sys.stdout.flush()  # Flush the output to ensure it is displayed
        except KeyboardInterrupt:
            self.read = 0  # Stop reading on keyboard interrupt

    def send(self, message) -> None:
        """
        Send a message to the serial port.
        """
        if self.ser and self.ser.is_open:
            self.ser.write(message.encode())  # Encode and send the message

    def close_coms(self) -> None:
        """
        Close the serial port.
        """
        if self.ser and self.ser.is_open:
            self.ser.close()

# Custom Button class to show a loading animation
class Button_loading_type(tk.Button):
    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)
        self.args = kwargs
        if "text" in self.args:
            self.display_text = self.args["text"]  # Text to display on the button
        self.char_list = ["-", "\\", "/"]  # Characters for loading animation
        self.cycler = self.character_cycler()  # Iterator for cycling through characters
        self.ongoing = False  # Flag to control the animation

    def character_cycler(self):
        """
        Cycle through the characters for the loading animation.
        """
        while True:
            for char in self.char_list:
                yield char

    def display_animation(self):
        """
        Update the button text to show the loading animation.
        """
        if self.ongoing:
            self.config(text=f"{self.display_text} {next(self.cycler)}")
            self.id = self.after(200, self.display_animation)  # Update every 200 ms

    def start_loading(self):
        """
        Start the loading animation.
        """
        self["state"] = tk.DISABLED  # Disable the button
        self.ongoing = True
        self.display_animation()

    def stop_loading(self):
        """
        Stop the loading animation.
        """
        self["state"] = tk.NORMAL  # Enable the button
        self.ongoing = False
        self.config(text=self.display_text)  # Reset the button text
        self.after_cancel(self.id)  # Cancel the animation

# Frame for initializing the communication
class Init_first_frame(tk.LabelFrame):
    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)
        # Button to initialize the communication
        self.init_comms_btn = Button_loading_type(self, text="Init port communication", command=self.update_progress)
        self.init_comms_btn.pack(anchor="center", padx=100, pady=100, ipadx=100, ipady=100)

        # Progress bar to show the progress of initialization
        self.progress = ttk.Progressbar(self, length=200, mode='determinate')
        self.progress.pack(pady=20)
    
    def update_progress(self):
        """
        Update the progress bar and initialize communication.
        """
        if self.progress["value"] == 0:
            self.init_comms_btn.start_loading()
            COMMS.open_coms()  # Open the serial port
        self.progress['value'] += 20  # Increment the progress bar
        if self.progress['value'] < 100:
            self.master.after(500, self.update_progress)  # Update every 500 ms
        else:
            self.destroy()  # Destroy the frame when done

# Frame for controlling signals, offsets, frequency, and amplitude
class Init_seccond_frame(tk.LabelFrame):
    def __init__(self, master, **kwargs):
        super().__init__(master, **kwargs)
        self.grid_columnconfigure([0, 1, 2], weight=1)  # Configure grid columns
        self.grid_rowconfigure([0, 1], weight=1)  # Configure grid rows
        
        # Frame for standard signal buttons
        self.frame_1_mid = tk.LabelFrame(self, text="Standard signals")
        self.frame_1_mid.grid(row=0, column=0, sticky="nsew")

        # Buttons for different signal types
        self.sin_btn = tk.Button(self.frame_1_mid, text="Sin signal", pady=10, command=lambda: self.send_written_msg("1"))
        self.sin_btn.pack(pady=15)

        self.saw_btn = tk.Button(self.frame_1_mid, text="Saw signal", pady=10, command=lambda: self.send_written_msg("2"))
        self.saw_btn.pack(pady=15)

        self.tri_btn = tk.Button(self.frame_1_mid, text="Tri signal", pady=10, command=lambda: self.send_written_msg("3"))
        self.tri_btn.pack(pady=15)

        self.cont_btn = tk.Button(self.frame_1_mid, text="continuous signal", pady=10, command=lambda: self.send_written_msg("7"))
        self.cont_btn.pack(pady=15)

        # Frame for offset control
        self.frame_2_mid = tk.LabelFrame(self, text="Offsets")
        self.frame_2_mid.grid(row=0, column=1, sticky="nsew")

        # Scale for setting offset
        self.offset_scale = tk.Scale(self.frame_2_mid, from_=0, to=2000, resolution=100, orient=tk.HORIZONTAL, length=300, command=self.on_offset_scale_move)
        self.offset_scale.pack(pady=20)
        self.offset_scale.set(1000)

        # Frame for frequency control
        self.frame_3_mid = tk.LabelFrame(self, text="Frequency")
        self.frame_3_mid.grid(row=0, column=2, sticky="nsew")

        # Buttons for frequency control
        self.sin_btn = tk.Button(self.frame_3_mid, text="Freq +", pady=10, command=lambda: self.send_written_msg("8"))
        self.sin_btn.pack(pady=15)

        self.saw_btn = tk.Button(self.frame_3_mid, text="Freq 1khz", pady=10, command=lambda: self.send_written_msg("9"))
        self.saw_btn.pack(pady=15)

        self.tri_btn = tk.Button(self.frame_3_mid, text="Freq -", pady=10, command=lambda: self.send_written_msg("a"))
        self.tri_btn.pack(pady=15)

        # Frame for amplitude control
        self.frame_4_mid = tk.LabelFrame(self, text="Amplitude")
        self.frame_4_mid.grid(row=1, column=0, sticky="nsew")

        # Scale for setting amplitude
        self.ampl_scale = tk.Scale(self.frame_4_mid, from_=1, to=3, orient=tk.HORIZONTAL, length=300, command=self.on_ampl_scale_move)
        self.ampl_scale.pack(pady=20)

    def on_ampl_scale_move(self, value):
        """
        Handle the amplitude scale movement.
        """
        if value == "1":
            print("sending")
            self.send_written_msg("x")
        if value == "2":
            print("sending")
            self.send_written_msg("y")
        if value == "3":
            print("sending")
            self.send_written_msg("z")

    def on_offset_scale_move(self, value):
        """
        Handle the offset scale movement.
        """
        if value == "1000":
            self.send_written_msg("5")
        else:
            if int(value) < 1000:
                aux = int(value)
                while aux > 0:
                    self.send_written_msg("6")
                    aux = aux - 100
            if int(value) > 1000:
                aux = int(value)
                while aux < 2000:
                    self.send_written_msg("4")
                    aux = aux + 100

    def send_written_msg(self, msg):
        """
        Send a written message to the serial port.
        """
        COMMS.send(message=msg)

# Main GUI class
class GUI:
    def __init__(self):
        self.root = tk.Tk()  # Create the main window
        self.root.title("Poor man's signal generator")  # Set the window title
        self.root.state("zoomed")  # Maximize the window
        self.root.resizable(width=False, height=False)  # Disable window resizing
    
        self.root.grid_columnconfigure(0, weight=1, pad=10)  # Configure the main grid column
        self.root.grid_rowconfigure(0, weight=1, pad=10)  # Configure the main grid row

        # Initialize the first frame for communication setup
        self.work_frame = Init_seccond_frame(self.root, text="Work space")
        self.work_frame.grid(column=0, row=0, sticky="nsew")
        
        self.work_frame = Init_first_frame(self.root)
        self.work_frame.grid(column=0, row=0, sticky="nsew")

        self.root.mainloop()  # Start the Tkinter main loop

if __name__ == "__main__":
    COMMS = START_COM("COM12", 4800)  # Initialize the serial communication
    window = GUI()  # Create and run the GUI
    COMMS.close_coms()  # Close the serial communication when the GUI is closed
