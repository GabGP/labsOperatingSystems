def print_trace_row(step, ref, result, frames, N, victim=""):
    # Fill empty frames with '-' for the trace
    display_frames = [str(f) for f in frames]
    
    while len(display_frames) < N:
        display_frames.append("-")
    
    frames_str = "[" + ", ".join(display_frames) + "]"
    print(f"{step:<5} {ref:<4} {result:<6} {frames_str:<15} {victim}")

def print_summary(hits, misses):
    total = hits + misses
    hit_rate = (hits / total) * 100 if total > 0 else 0
    print(f"Totals: hits={hits} misses={misses} (Hit Rate: {hit_rate:.2f}%)")