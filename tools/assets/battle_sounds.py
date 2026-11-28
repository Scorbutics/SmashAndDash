#!/usr/bin/env python3
"""Writes the sounds of the changes of statistics in a battle (Sounds/battle_stat_up.wav, battle_stat_down.wav).

Made here from sine waves (no sample to credit), as in the first games : a statistic rises, a quick rising shimmer of
notes ; it falls, the same falling, lower. 22050 Hz, 16 bits, mono.

Usage : battle_sounds.py [directory]   (default : Sounds)
"""
import math
import os
import struct
import sys
import wave

RATE = 22050


def sweep(start, end, duration, steps, volume):
    """A run of short notes from "start" to "end" Hz (a geometric ladder), each with a soft attack and decay, a tremolo."""
    samples = []
    note = duration / steps
    for step in range(steps):
        frequency = start * (end / start) ** (step / max(1, steps - 1))
        count = int(note * RATE)
        phase = 0.0
        for i in range(count):
            t = i / RATE
            envelope = min(1.0, t / 0.006) * math.exp(-t * 9.0)
            # A little vibrato and a fifth above : brighter
            f = frequency * (1.0 + 0.012 * math.sin(2 * math.pi * 30 * t))
            phase += 2 * math.pi * f / RATE
            value = math.sin(phase) * 0.7 + math.sin(phase * 1.5) * 0.2 + math.sin(phase * 2.0) * 0.1
            samples.append(value * envelope * volume)
    # The tail : the last note rings a little longer
    tail = int(0.18 * RATE)
    last = samples[-int(note * RATE):]
    for i in range(tail):
        samples.append(last[i % len(last)] * (1.0 - i / tail) * 0.5)
    return samples


def write(path, samples):
    with wave.open(path, 'wb') as out:
        out.setnchannels(1)
        out.setsampwidth(2)
        out.setframerate(RATE)
        peak = max(1e-6, max(abs(s) for s in samples))
        scale = 0.8 / peak
        out.writeframes(b''.join(struct.pack('<h', int(max(-1.0, min(1.0, s * scale)) * 32767)) for s in samples))


def main():
    directory = sys.argv[1] if len(sys.argv) > 1 else 'Sounds'
    write(os.path.join(directory, 'battle_stat_up.wav'), sweep(440.0, 1760.0, 0.42, 9, 1.0))
    write(os.path.join(directory, 'battle_stat_down.wav'), sweep(880.0, 196.0, 0.48, 9, 1.0))


if __name__ == '__main__':
    main()
