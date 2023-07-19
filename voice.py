import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
from tkinter import filedialog
import os
import subprocess
import shutil

champ_folders = {}
skin_vars = {}

def select_button():
    selected_directory = filedialog.askdirectory()
    compare_window = tk.Toplevel(root)
    frame_grid = tk.Frame(
        master=compare_window,
        relief=tk.RAISED,
        borderwidth=2
    )
    counter = 0
    for root_dir, dirs, files in os.walk(selected_directory):
        counter += 1
        frame_grid.grid(row=counter, column=1)
        label_grid = tk.Label(master=frame_grid, text=f"Row No. {counter}\nColumn No. 2")
        label_grid.pack()
    compare_window.mainloop()
    # for root_dir, dirs, files in os.walk(selected_directory):
    #     print("Исходный каталог:", root_dir)



def extract_audio(selected_indices):
    champions = [champ_listbox.get(i) for i in selected_indices]
    for champ in champions:
        # Використовуємо тільки вибрані підшкіри
        selected_skins = [k for k, v in skin_vars[champ].items() if v.get() == 1]

        for skin in selected_skins:
            path_to_files = f'./characters/{champ}/skins/{skin}'
            if not os.path.exists(f'{path_to_files}/vo_files'):
                for root, dirs, files in os.walk(path_to_files):
                    for file in files:
                        if file.endswith('_vo_audio.wpk'):
                            command = f'bnk-extract -a {os.path.join(root, file)} -o {os.path.join(root, "vo_files")} --wems-only'
                            subprocess.run(command, shell=True)
        tree_view_files(champ)
    messagebox.showinfo("Info", "Files have been extracted successfully!")


def tree_view_files(champ):
    path_to_files = f'./characters/{champ}/skins'
    if champ in champ_folders:
        for child in tree.get_children(champ_folders[champ]):
            tree.delete(child)
    else:
        champ_folders[champ] = tree.insert('', 'end', text=champ)

    # using only selected skins
    selected_skins = [k for k, v in skin_vars[champ].items() if v.get() == 1]

    for root, dirs, files in os.walk(path_to_files):
        skin_name = root.split('/')[-1].replace('skins\\', '')
        if 'vo_files' in dirs and os.listdir(os.path.join(root, 'vo_files')) and skin_name in selected_skins:
            for dir in dirs:
                if 'vo_files' == dir:
                    skin_folder = tree.insert(champ_folders[champ], 'end', text=skin_name)
                    for file in os.listdir(os.path.join(root, dir)):
                        if file.endswith('.wem'):
                            tree.insert(skin_folder, 'end', text=skin_name, values=(file, os.path.getsize(os.path.join(root, dir, file))))

def on_champion_select(event):
    selected_indices = champ_listbox.curselection()
    champions = [champ_listbox.get(i) for i in selected_indices]

    for champ in list(champ_folders.keys()):
        if champ not in champions:
            tree.delete(champ_folders[champ])
            del champ_folders[champ]

    for champ in champions:
        if champ not in champ_folders:
            skins_popup(champ)

def skins_popup(champ):
    top = tk.Toplevel(root)
    top.title(f"Select {champ}'s Skins")

    for skin, var in skin_vars[champ].items():
        c = tk.Checkbutton(top, text=skin, variable=var)
        c.pack()

    btn = tk.Button(top, text='OK', command=lambda: [tree_view_files(champ), top.destroy()])
    btn.pack()

root = tk.Tk()
root.title('BNK Extract GUI')
root.geometry('800x600')

champions = os.listdir('./characters')
champ_listbox = tk.Listbox(root, selectmode=tk.MULTIPLE)
champ_listbox.bind('<<ListboxSelect>>', on_champion_select)
for champ in champions:
    champ_listbox.insert(tk.END, champ)

tree = ttk.Treeview(root)
tree["columns"] = ("one", "two")
tree.column("#0", width=150, stretch=tk.YES)
tree.column("one", width=150)
tree.column("two", width=100)
tree.heading("#0", text="Champion Name")
tree.heading("one", text="File Name")
tree.heading("two", text="File Size")

# створюємо фрейм для кнопок
button_frame = tk.Frame(root)
button_frame.grid(row=1, column=0, columnspan=2, sticky='ew', pady=(10, 0))

extract_button = tk.Button(button_frame, text='Extract Audio', command=lambda: extract_audio(champ_listbox.curselection()))
select_new_voices = tk.Button(button_frame, text='Select New Voices', command=lambda: select_button())
# добавляємо кнопки-муляжі
dummy_button2 = tk.Button(button_frame, text='Button 2')

# виставляємо кнопки на екран
extract_button.grid(row=0, column=0, padx=(0, 10), sticky='w')
select_new_voices.grid(row=0, column=1, padx=(0, 10))
dummy_button2.grid(row=0, column=2)

for champ in champions:
    path_to_files = f'./characters/{champ}/skins'
    skin_names = [name for name in os.listdir(path_to_files)]
    skin_vars[champ] = {name: tk.IntVar() for name in skin_names}

root.grid_columnconfigure(0, weight=1)
root.grid_columnconfigure(1, weight=2)

root.grid_rowconfigure(0, weight=1)
root.grid_rowconfigure(1, weight=0)

champ_listbox.grid(row=0, column=0, sticky='nsew')
tree.grid(row=0, column=1, sticky='nsew')

root.mainloop()
