import os
import numpy as np
from PIL import Image
from moviepy import VideoFileClip
import argparse


def frame_to_1bit_rle(frame, resize_dim=(320, 480)):
    img = Image.fromarray(frame)
    img = img.convert('L')

    if resize_dim:
        img = img.resize(resize_dim)

    img = img.point(lambda x: 255 if x > 128 else 0, mode='1')
    arr = np.array(img, dtype=np.uint8)
    h, w = arr.shape

    raw = []
    for y in range(h):
        byte = 0
        count = 0
        for x in range(w):
            bit = 1 if arr[y, x] else 0
            byte = (byte << 1) | bit
            count += 1

            if count == 8:
                raw.append(byte)
                byte = 0
                count = 0

        if count != 0:
            byte <<= (8 - count)
            raw.append(byte)

    # RLE
    rle = []
    last = raw[0]
    run = 1

    for b in raw[1:]:
        if b == last and run < 255:
            run += 1
        else:
            rle.append(run)
            rle.append(last)
            last = b
            run = 1

    rle.append(run)
    rle.append(last)

    return np.array(rle, dtype=np.uint8)


def add_frame_to_header(f, data, var_name):
    length = len(data)
    f.write(f"\n// {var_name}\n")
    f.write(f"const uint8_t {var_name}[{length}] = {{\n")

    for i in range(0, length, 16):
        chunk = data[i:i+16]
        line = ', '.join(f'0x{b:02X}' for b in chunk)
        f.write(f"    {line},\n")

    f.write("};\n")
    return length


def video_to_h(video_path, output_file, fps=5, resize_dim=(480, 320)):
    clip = VideoFileClip(video_path)

    frame_names = []

    with open(output_file, 'w') as f:
        # Header start
        f.write("#ifndef FRAMES_H\n")
        f.write("#define FRAMES_H\n")
        f.write("#include \"inttypes.h\"\n\n")

        # Frames
        for i, frame in enumerate(clip.iter_frames(fps=fps, dtype="uint8")):
            var_name = f"frame_{i:05d}"

            data = frame_to_1bit_rle(frame, resize_dim)
            add_frame_to_header(f, data, var_name)

            frame_names.append(var_name)

            print(f"Processed frame {i}")

        # Frames array
        f.write("FrameRLE_t frames[] = {\n")
        for name in frame_names:
            f.write(f"    {{&{name}[0], sizeof({name})}},\n")
        f.write("};\n\n")

        f.write("#endif\n")

    print(f"\nГотово! Создан файл {output_file}")
    print(f"Всего кадров: {len(frame_names)}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Video → RLE → single .h")
    parser.add_argument("video", help="Input video file")
    parser.add_argument("-o", "--output", default="bitmaps.h", help="Output .h file")
    parser.add_argument("--fps", type=int, default=5, help="Frames per second")
    parser.add_argument("--resize", nargs=2, type=int, metavar=("W", "H"), help="Resize")

    args = parser.parse_args()

    resize_dim = tuple(args.resize) if args.resize else (480, 320)

    video_to_h(args.video, args.output, args.fps, resize_dim)