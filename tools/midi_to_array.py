"""
MIDI 转数组
"""

import sys
import os

try:
    from mido import MidiFile, tempo2bpm
except ImportError:
    print("请运行: pip install mido")
    sys.exit(1)


def midi_note_to_frequency(note):
    """将 MIDI 音符号转换为频率 (Hz)"""
    if note == 0:
        return 0
    return int(440 * (2 ** ((note - 69) / 12)))


def parse_midi_file(filepath):
    """解析 MIDI 文件"""
    try:
        midi = MidiFile(filepath)
        return midi
    except Exception as e:
        print(f"错误: 无法读取 MIDI 文件 - {e}")
        return None


def select_track(midi):
    """选择音轨"""
    print(f"\n找到 {len(midi.tracks)} 个音轨:")
    
    for i, track in enumerate(midi.tracks):
        note_count = sum(1 for msg in track if msg.type == 'note_on' and msg.velocity > 0)
        print(f"  [{i}] {track.name if track.name else '未命名'} - {note_count} 个音符")
    
    while True:
        try:
            choice = input(f"\n请选择音轨 (0-{len(midi.tracks)-1}): ").strip()
            track_idx = int(choice)
            if 0 <= track_idx < len(midi.tracks):
                return track_idx
            else:
                print("无效的音轨编号，请重新输入")
        except ValueError:
            print("请输入有效的数字")
        except KeyboardInterrupt:
            print("\n\n已取消")
            sys.exit(0)


def convert_track_to_array(midi, track_idx):
    """将选定的音轨转换为数组"""
    track = midi.tracks[track_idx]
    ticks_per_beat = midi.ticks_per_beat
    tempo = 500000  # 默认 120 BPM
    
    # 从所有音轨中提取 tempo 信息
    for t in midi.tracks:
        for msg in t:
            if msg.type == 'set_tempo':
                tempo = msg.tempo
                break
        if tempo != 500000:
            break
    
    result = []
    active_notes = {}
    current_time = 0
    
    for msg in track:
        current_time += msg.time
        
        # 更新速度
        if msg.type == 'set_tempo':
            tempo = msg.tempo
        
        # 处理音符开始
        elif msg.type == 'note_on' and msg.velocity > 0:
            active_notes[msg.note] = current_time
        
        # 处理音符结束
        elif (msg.type == 'note_off') or (msg.type == 'note_on' and msg.velocity == 0):
            if msg.note in active_notes:
                start_time = active_notes[msg.note]
                duration_ticks = current_time - start_time
                duration_ms = int((duration_ticks / ticks_per_beat) * (tempo / 1000))
                
                if duration_ms > 0:
                    frequency = midi_note_to_frequency(msg.note)
                    result.append((frequency, duration_ms))
                
                del active_notes[msg.note]
    
    return result


def merge_simultaneous_notes(notes_array):
    """合并同时播放的音符，只保留频率最高的"""
    if not notes_array:
        return []
    
    merged = []
    i = 0
    
    while i < len(notes_array):
        freq, duration = notes_array[i]
        
        # 查找同时开始的音符（时长相近）
        simultaneous = [freq]
        j = i + 1
        
        while j < len(notes_array):
            next_freq, next_duration = notes_array[j]
            # 如果时长相近（误差在 10ms 内），认为是同时播放
            if abs(duration - next_duration) <= 10:
                simultaneous.append(next_freq)
                j += 1
            else:
                break
        
        # 只保留最高频率
        max_freq = max(simultaneous)
        merged.append((max_freq, duration))
        
        i = j if j > i + 1 else i + 1
    
    return merged


def format_as_c_array(notes_array, array_name="music_data"):
    """格式化为 C 语言数组"""
    lines = [f"const uint16_t {array_name}[{len(notes_array)}][2] = {{"]
    
    for i in range(0, len(notes_array), 4):
        line_notes = notes_array[i:i+4]
        formatted = ", ".join([f"{{{freq}, {dur}}}" for freq, dur in line_notes])
        lines.append(f"    {formatted},")
    
    lines[-1] = lines[-1].rstrip(',')  # 移除最后一个逗号
    lines.append("};")
    
    return "\n".join(lines)


def main():
    print("=" * 60)
    print("MIDI 转蜂鸣器数组工具")
    print("=" * 60)
    
    # 获取 MIDI 文件路径
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    else:
        filepath = input("\n请输入 MIDI 文件路径: ").strip().strip('"').strip("'")
    
    if not os.path.exists(filepath):
        print(f"错误: 文件不存在 - {filepath}")
        return
    
    # 解析 MIDI 文件
    print(f"\n正在解析: {filepath}")
    midi = parse_midi_file(filepath)
    if not midi:
        return
    
    # 选择音轨
    track_idx = select_track(midi)
    print(f"\n已选择音轨 {track_idx}")
    
    # 转换为数组
    print("正在转换...")
    notes_array = convert_track_to_array(midi, track_idx)
    
    if not notes_array:
        print("错误: 未找到有效的音符数据")
        return
    
    print(f"转换完成: {len(notes_array)} 个音符")
    
    # 生成 C 数组
    array_name = input("\n请输入数组名称 (默认: music_data): ").strip() or "music_data"
    c_array = format_as_c_array(notes_array, array_name)
    
    print("转换结果：")
    print(c_array)
    


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n已取消")
        sys.exit(0)
