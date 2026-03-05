from pydub import AudioSegment
import numpy as np
import argparse

TARGET_SR = 32000  # частота дискретизации


INDEX_TABLE = np.array([
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
], dtype=np.int32)

STEP_TABLE = np.array([
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
], dtype=np.int32)


def mp3_to_pcm(mp3_path: str, seconds=None):
    audio = AudioSegment.from_mp3(mp3_path)

    # mono
    audio = audio.set_channels(1)

    # ставим частоту дискретизации (это единственный ресемплинг, который нужен)
    audio = audio.set_frame_rate(TARGET_SR)

    # обрезка по времени
    if seconds is not None:
        audio = audio[:seconds * 1000]

    samples = np.array(audio.get_array_of_samples(), dtype=np.int16)
    return samples


def ima_adpcm_encode(pcm16: np.ndarray):
    """
    PCM int16 -> IMA ADPCM 4-bit
    Возвращает:
        packed_bytes, first_sample, start_index
    """
    if len(pcm16) < 2:
        raise ValueError("PCM слишком короткий")

    predictor = int(pcm16[0])
    index = 0
    step = int(STEP_TABLE[index])

    adpcm_nibbles = []

    for sample in pcm16[1:]:
        sample = int(sample)
        diff = sample - predictor

        code = 0
        if diff < 0:
            code = 8
            diff = -diff

        delta = step >> 3

        if diff >= step:
            code |= 4
            diff -= step
            delta += step

        if diff >= (step >> 1):
            code |= 2
            diff -= (step >> 1)
            delta += (step >> 1)

        if diff >= (step >> 2):
            code |= 1
            delta += (step >> 2)

        if code & 8:
            predictor -= delta
        else:
            predictor += delta

        predictor = max(-32768, min(32767, predictor))

        index += int(INDEX_TABLE[code & 0x0F])
        index = max(0, min(88, index))

        step = int(STEP_TABLE[index])

        adpcm_nibbles.append(code & 0x0F)

    packed = bytearray()
    for i in range(0, len(adpcm_nibbles), 2):
        if i + 1 < len(adpcm_nibbles):
            packed.append((adpcm_nibbles[i] & 0x0F) | ((adpcm_nibbles[i + 1] & 0x0F) << 4))
        else:
            packed.append(adpcm_nibbles[i] & 0x0F)

    return packed, int(pcm16[0]), 0


def write_header_file(filename, var_name, adpcm_bytes, first_sample, index):
    var_upper = var_name.upper()

    with open(filename, "w", encoding="utf-8") as f:
        f.write("#pragma once\n")
        f.write("#include <stdint.h>\n\n")

        f.write(f"#define {var_upper}_LEN {len(adpcm_bytes)}\n")
        f.write(f"#define {var_upper}_SR {TARGET_SR}\n")
        f.write(f"#define {var_upper}_FIRST {first_sample}\n")
        f.write(f"#define {var_upper}_INDEX {index}\n\n")

        f.write(f"const uint8_t {var_name}[{var_upper}_LEN] = {{\n")

        for i in range(0, len(adpcm_bytes), 32):
            chunk = adpcm_bytes[i:i + 32]
            line = ", ".join(str(b) for b in chunk)
            f.write(f"    {line},\n")

        f.write("};\n")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("mp3", help="Путь к mp3 файлу")
    parser.add_argument("--seconds", type=int, default=None, help="Обрезка по времени (сек)")
    parser.add_argument("--name", default="what", help="Имя массива в C")
    parser.add_argument("--out", default="music.h", help="Имя выходного .h файла")
    args = parser.parse_args()

    pcm = mp3_to_pcm(args.mp3, args.seconds)

    duration_sec = len(pcm) / TARGET_SR
    print("====== INFO ======")
    print("File:", args.mp3)
    print("Sample rate:", TARGET_SR)
    print("Samples:", len(pcm))
    print("Duration:", round(duration_sec, 3), "sec")
    print("==================")

    adpcm_bytes, first_sample, index = ima_adpcm_encode(pcm)

    write_header_file(args.out, args.name, adpcm_bytes, first_sample, index)

    print(f"\nГотово! Сохранено в файл: {args.out}")
    print(f"ADPCM bytes: {len(adpcm_bytes)}")
    print(f"FIRST: {first_sample}")
    print(f"INDEX: {index}")


if __name__ == "__main__":
    main()