from pydub import AudioSegment
import numpy as np
import argparse

TARGET_SR = 32000
DAC_BITS = 12
DAC_MAX = (1 << DAC_BITS) - 1
DAC_MID = DAC_MAX // 2

DOWNSAMPLE_FACTOR = 1  # в столько раз меньше элементов

def mp3_to_pcm(mp3_path, seconds):
    audio = AudioSegment.from_mp3(mp3_path)

    audio = audio.set_channels(1)
    audio = audio.set_frame_rate(TARGET_SR)

    audio = audio[:seconds * 1000]

    samples = np.array(audio.get_array_of_samples(), dtype=np.float32)

    # нормализация (-1..1)
    samples /= np.max(np.abs(samples)) + 1e-9

    # уменьшаем массив
    samples = samples[::DOWNSAMPLE_FACTOR]

    # перевод в uint16 под 12 бит DAC
    pcm = (samples * DAC_MID + DAC_MID).astype(np.uint16)

    return pcm

def generate_c_header(pcm, var_name):
    print("#pragma once")
    print("#include <stdint.h>")
    print()
    print(f"#define {var_name.upper()}_LEN {len(pcm)}")
    print(f"#define {var_name.upper()}_DOWNSAMPLE {DOWNSAMPLE_FACTOR}")
    print()
    print(f"const uint16_t {var_name}[{var_name.upper()}_LEN] = {{")

    for i in range(0, len(pcm), 16):
        chunk = pcm[i:i+16]
        line = ", ".join(f"0x{v:04X}" for v in chunk)
        print(f"    {line},")

    print("};")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("mp3", help="MP3 file")
    parser.add_argument("--seconds", type=int, required=True, help="Duration in seconds")
    parser.add_argument("--name", default="music", help="Array name")

    args = parser.parse_args()

    pcm = mp3_to_pcm(args.mp3, args.seconds)
    generate_c_header(pcm, args.name)

if __name__ == "__main__":
    main()
