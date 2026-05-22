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