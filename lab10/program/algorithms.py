from logging import print_trace_row

def simulate_fifo(N, sequence):
    """FIFO (First-In, First-Out) Page Replacement."""
    print(f"\n--- FIFO (N={N}) ---")
    print(f"{'step':<5} {'ref':<4} {'result':<6} {'frames':<15} {'victim'}")
    
    frames = []
    hits = 0
    misses = 0
    
    for step, ref in enumerate(sequence, start=1):
        victim = ""
        if ref in frames:
            result = "HIT"
            hits += 1
        else:
            result = "MISS"
            misses += 1
            if len(frames) == N:
                victim = frames.pop(0)
            frames.append(ref)
            
        print_trace_row(step, ref, result, frames, N, victim)
        
    return hits, misses

def simulate_min(N, sequence):
    """MIN/Optimal Page Replacement."""
    print(f"\n--- MIN (N={N}) ---")
    print(f"{'step':<5} {'ref':<4} {'result':<6} {'frames':<15} {'victim'}")
    
    frames = []
    hits = 0
    misses = 0
    
    for step, ref in enumerate(sequence, start=1):
        victim = ""
        if ref in frames:
            result = "HIT"
            hits += 1
        else:
            result = "MISS"
            misses += 1
            if len(frames) == N:
                # Find the page used farthest in the future
                future_seq = sequence[step:]
                best_candidates = []
                max_dist = -1
                
                for p in frames:
                    try:
                        dist = future_seq.index(p)
                    except ValueError:
                        # If it never appears again, set distance to infinity
                        dist = float('inf')
                        
                    if dist > max_dist:
                        max_dist = dist
                        best_candidates = [p]
                    elif dist == max_dist:
                        best_candidates.append(p)
                        
                # Tie-break: smallest page ID
                victim = min(best_candidates)
                frames.remove(victim)
                
            frames.append(ref)
            
        print_trace_row(step, ref, result, frames, N, victim)
        
    return hits, misses

def simulate_lru(N, sequence):
    """LRU (Least Recently Used) Page Replacement."""
    print(f"\n--- LRU (N={N}) ---")
    print(f"{'step':<5} {'ref':<4} {'result':<6} {'frames':<15} {'victim'}")

    frames = []
    hits = 0
    misses = 0

    for step, ref in enumerate(sequence, start=1):
        victim = ""
        if ref in frames:
            result = "HIT"
            hits += 1
            # Update recency
            frames.remove(ref)
            frames.append(ref)
        else:
            result = "MISS"
            misses += 1
            if len(frames) == N:
                # Evict the least recently used
                victim = frames.pop(0)
            frames.append(ref)

        print_trace_row(step, ref, result, frames, N, victim)

    return hits, misses

def simulate_clock(N, sequence):
    """Clock (Second-Chance) Page Replacement."""
    print(f"\n--- CLOCK (N={N}) ---")
    print(f"{'step':<5} {'ref':<4} {'result':<6} {'frames':<15} {'ref_bits':<15} {'victim'}")

    frames = []
    ref_bits = []
    pointer = 0
    hits = 0
    misses = 0

    for step, ref in enumerate(sequence, start=1):
        victim = ""
        if ref in frames:
            result = "HIT"
            hits += 1
            # Give the page a second chance
            ref_bits[frames.index(ref)] = 1
        else:
            result = "MISS"
            misses += 1
            if len(frames) < N:
                # There is still empty space
                frames.append(ref)
                ref_bits.append(1)
                pointer = len(frames) % N
            else:
                # Sweep the clock hand until we find a ref bit == 0
                while ref_bits[pointer] == 1:
                    ref_bits[pointer] = 0
                    pointer = (pointer + 1) % N
                victim = frames[pointer]
                frames[pointer] = ref
                ref_bits[pointer] = 1
                pointer = (pointer + 1) % N

        # Print trace with reference bits
        display_frames = [str(f) for f in frames]
        display_bits = [str(b) for b in ref_bits]
        while len(display_frames) < N:
            display_frames.append("-")
            display_bits.append("-")
        frames_str = "[" + ", ".join(display_frames) + "]"
        bits_str = "[" + ", ".join(display_bits) + "]"
        print(f"{step:<5} {ref:<4} {result:<6} {frames_str:<15} {bits_str:<15} {victim}")

    return hits, misses