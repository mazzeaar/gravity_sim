import os
from PIL import Image, ImageFilter

def generate_gif(folder_path, output_path, frame_duration=20, blur_amount=0.5):
    # Get a list of PNG files in the folder and sort them by name
    file_names = sorted(
        [f for f in os.listdir(folder_path) if f.endswith('.png')],
        key=lambda x: int(x.split(".")[0])
    )

    frames = []

    # Iterate over sorted file names
    for file_name in file_names:
        print(f'Processing File: {file_name}')
        frame_path = os.path.join(folder_path, file_name)

        # Open the frame as PIL Image
        frame = Image.open(frame_path)

        # Apply lossy compression to reduce file size
        frame = frame.convert('P', dither=Image.NONE, palette=Image.ADAPTIVE, colors=256)

        # Append the processed frame to the frames list
        frames.append(frame)

    # Save the frames as a GIF with optimized palette and reduced frame count
    frames[0].save(
        output_path,
        format='GIF',
        append_images=frames[1:],
        save_all=True,
        duration=frame_duration,
        loop=0,
        optimize=True,
        quality=85
    )

    print(f"GIF generated successfully at '{output_path}'.")

# Specify the folder path containing the PNG frames
folder_path = 'dump/'

# Specify the output path for the generated GIF
output_path = './galaxy.gif'

# Generate the GIF
generate_gif(folder_path, output_path)
