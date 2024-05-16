import tkinter as tk
from tkinter import ttk
from tkinter import LabelFrame
import configparser
import os

config = configparser.ConfigParser()

def saveConfig():
    # Set configuration
    config['Display']['theme'] = selected_option1.get()
    config['Performance']['refresh_rate'] = selected_option2.get()
    config['Extension']['variant'] = selected_option3.get()
    config['Sound']['note'] = selected_option4.get()

    config['Display']['window_scale'] = selected_value1.get()
    config['Performance']['speed'] = selected_value2.get()

    config['Display']['pixel_boundary'] = 'true' if checkbox_var.get() else 'false'
    config['Debug_logs']['instruction_execution'] = 'true' if checkbox_var1.get() else 'false'
    config['Debug_logs']['register_changes'] = 'true' if checkbox_var2.get() else 'false'
    config['Debug_logs']['memory_access']  = 'true' if checkbox_var3.get() else 'false'
    config['Debug_logs']['stack_operations'] = 'true' if checkbox_var4.get() else 'false'
    config['Debug_logs']['input_keys'] = 'true' if checkbox_var5.get() else 'false'
    config['Debug_logs']['timers'] = 'true' if checkbox_var6.get() else 'false'
    config['Debug_logs']['performance_metrics'] = 'true' if checkbox_var7.get() else 'false'

    # Save configuration to config file
    with open('config.ini', 'w') as configfile:
        config.write(configfile)

root = tk.Tk()
root.geometry()
root.resizable(False, False)
root.title("Emulator configuration")

#adding labelframes
display_frame = LabelFrame(root, text='Display', width=300, height=200)
display_frame.grid(row=0, column=0, padx=10, pady=10, sticky='nsew')

sound_frame = LabelFrame(root, text='Sound', width=300, height=200)
sound_frame.grid(row=1, column=0, padx=10, pady=10, sticky='nsew')

perf_frame = LabelFrame(root, text='Performance', width=300, height=200)
perf_frame.grid(row=2, column=0, padx=10, pady=10, sticky='nsew')

chip8_frame = LabelFrame(root, text='Chip-8', width=300, height=200)
chip8_frame.grid(row=1, column=1, padx=10, pady=10, sticky='nsew')

debug_frame = LabelFrame(root, text='Debug Logs', width=300, height=200)
debug_frame.grid(row=0, column=1, padx=10, pady=10, sticky='nsew')

#adding save button
btn_1 = tk.Button(root, text= "Save", width=10, font=("Arial", 14), command=saveConfig)
btn_1.grid(row=2, column =1, padx=20, pady=20, sticky='se')

# Adding label1(scale)
scale_label = ttk.Label(display_frame, text="Scale:")
scale_label.grid(row=0, column=0, padx=5, pady=5)

selected_value1 = tk.StringVar()
selected_value1.set(20)
scale_value = ttk.Spinbox(display_frame, from_=10, to=60, width = 10, textvariable=selected_value1)
scale_value.grid(row=0, column=1, padx=5, pady=5)

# Adding label2
label2 = ttk.Label(display_frame, text="Theme:")
label2.grid(row=1, column=0, padx=5, pady=5)

#Adding dropdown menu
options = ["", "White", "Green", "Amber", "Game boy", "80s"]
selected_option1 = tk.StringVar()
selected_option1.set(options[0])
dropdown1 = ttk.OptionMenu(display_frame, selected_option1, *options)
dropdown1.grid(row=1, column=1, padx=5, pady=5)

#adding label7(checkbox)
checkbox_var = tk.BooleanVar()
checkbox = ttk.Checkbutton(display_frame, text="Pixel outline", variable=checkbox_var)
checkbox.grid(row=2, column=1, columnspan=1, padx=5, pady=5, sticky= 'w')

# Adding label3
label3 = ttk.Label(perf_frame, text="Refresh rate:")
label3.grid(row=0, column=0, padx=5, pady=5)

#Adding dropdown menu
options = ["", "30hz", "60hz", "90hz", "120hz"]
selected_option2 = tk.StringVar()
selected_option2.set(options[0])
dropdown2 = ttk.OptionMenu(perf_frame, selected_option2, *options)
dropdown2.grid(row=0, column=1, padx=5, pady=5)

# Adding label4
label4 = ttk.Label(chip8_frame, text="Extension:")
label4.grid(row=0, column=0, padx=5, pady=5)

#Adding dropdown menu
options = ["", "Standard", "Super"]
selected_option3 = tk.StringVar()
selected_option3.set(options[0])
dropdown3 = ttk.OptionMenu(chip8_frame, selected_option3, *options)
dropdown3.grid(row=0, column=1, padx=5, pady=5)

#adding label5(checkbox)
#debugging
checkbox_var1 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Instruction execution", variable=checkbox_var1)
checkbox.grid(row=0, column=0, padx=5, pady=5, rowspan=2, sticky='w')

checkbox_var2 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Register change", variable=checkbox_var2)
checkbox.grid(row=2, column=0, padx=5, pady=5, rowspan=2, sticky='w' )

checkbox_var3 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Memory access", variable=checkbox_var3)
checkbox.grid(row=4, column=0, padx=5, pady=5, rowspan=2, sticky='w')

checkbox_var4 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Input", variable=checkbox_var4)
checkbox.grid(row=6, column=0, padx=5, pady=5, rowspan=2, sticky='w' )

checkbox_var5 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Stack", variable=checkbox_var5)
checkbox.grid(row=0, column=2, padx=5, pady=5, rowspan=2, sticky='w' )

checkbox_var6 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Timer", variable=checkbox_var6)
checkbox.grid(row=2, column=2, padx=5, pady=5, rowspan=2, sticky='w' )

checkbox_var7 = tk.BooleanVar()
checkbox = ttk.Checkbutton(debug_frame, text="Performance metrics", variable=checkbox_var7)
checkbox.grid(row=4, column=2, padx=5, pady=5, rowspan=2, sticky='w')

# Adding label6
label6 = ttk.Label(sound_frame, text="Sound note:")
label6.grid(row=0, column=0, padx=5, pady=5)

#Adding dropdown menu
options = ["", "C", "D", "E", "F", "G", "A", "B"]
selected_option4 = tk.StringVar()
selected_option4.set(options[0])  
dropdown = ttk.OptionMenu(sound_frame, selected_option4, *options)
dropdown.grid(row=0, column=1, padx=5, pady=5)

# Adding label8(speed)
speed_label = ttk.Label(perf_frame, text="Speed:")
speed_label.grid(row=1, column=0, padx=5, pady=5,sticky='w')

#entry field for the speed
selected_value2 = tk.StringVar()
selected_value2.set(700)
speed_entry = ttk.Spinbox(perf_frame, from_=1, to=1500, width = 10, textvariable=selected_value2)
speed_entry.grid(row=1, column=1, padx=5, pady=5,)

# Add sections and key-value pairs
config['Display'] = {
    'window_scale': '20',
    'theme': 'White',
    'pixel_boundary': 'false'
}

config['Sound'] = {
    'note': 'A'
}

config['Performance'] = {
    'speed': '700',
    'refresh_rate': '60hz'
}

config['Debug_logs'] = {
    'instruction_execution': 'false',
    'register_changes': 'false',
    'memory_access': 'false',
    'stack_operations': 'false',
    'input_keys': 'false',
    'timers': 'false',
    'performance_metrics': 'false'
}

config['Extension'] = {
    'variant': 'Standard'
}

if os.path.exists("config.ini"):
    # Read current configuration from config file
    config.clear()
    config.read("config.ini")
else:
    # Save default configuration to config file
    with open('config.ini', 'w') as configfile:
        config.write(configfile)

# Set configuration
selected_option1.set(config['Display']['theme'])
selected_option2.set(config['Performance']['refresh_rate'])
selected_option3.set(config['Extension']['variant'])
selected_option4.set(config['Sound']['note'])

selected_value1.set(int(config['Display']['window_scale']))
selected_value2.set(int(config['Performance']['speed']))

checkbox_var.set(config['Display']['pixel_boundary'] == 'true')
checkbox_var1.set(config['Debug_logs']['instruction_execution'] == 'true')
checkbox_var2.set(config['Debug_logs']['register_changes'] == 'true')
checkbox_var3.set(config['Debug_logs']['memory_access'] == 'true')
checkbox_var4.set(config['Debug_logs']['stack_operations'] == 'true')
checkbox_var5.set(config['Debug_logs']['input_keys'] == 'true')
checkbox_var6.set(config['Debug_logs']['timers'] == 'true')
checkbox_var7.set(config['Debug_logs']['performance_metrics'] == 'true')

if __name__ == '__main__':
    root.mainloop()
