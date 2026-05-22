import sys
from algorithms import simulate_fifo, simulate_min, simulate_lru, simulate_clock
from logging import print_summary
from beladys_anomaly import run_demo as run_beladys_demo

def read_input(filepath):
    try:
        with open(filepath, 'r') as f:
            content = f.read().split()
            
        if not content:
            raise ValueError("Empty dataset.")
            
        # Parse and validate N
        N = int(content[0])
        if N < 1:
            raise ValueError("N must be >= 1.")
            
        # Parse and validate the reference string
        sequence = [int(x) for x in content[1:]]
        if not sequence:
            raise ValueError("Reference sequence is empty.")
            
        return N, sequence
        
    except FileNotFoundError:
        print(f"Error: Could not find file '{filepath}'.")
        sys.exit(1)
    except ValueError as e:
        print(f"Validation Error: {e}")
        sys.exit(1)

def main():
    filepath = '../dataset.txt'
    print(f"Reading from {filepath}...")
    
    N, sequence = read_input(filepath)
    print(f"Frames (N): {N}")
    print(f"Reference Sequence: {sequence}")
    
    # FIFO
    fifo_hits, fifo_misses = simulate_fifo(N, sequence)
    print_summary(fifo_hits, fifo_misses)
    
    # MIN
    min_hits, min_misses = simulate_min(N, sequence)
    print_summary(min_hits, min_misses)
    
    # LRU
    lru_hits, lru_misses = simulate_lru(N, sequence)
    print_summary(lru_hits, lru_misses)

    # CLOCK
    clock_hits, clock_misses = simulate_clock(N, sequence)
    print_summary(clock_hits, clock_misses)

    print("\n--- Comparison Summary ---")
    print(f"{'Algorithm':<10} | {'Hits':<5} | {'Misses':<6} | {'Hit Rate'}")
    print("-" * 45)
    print(f"{'FIFO':<10} | {fifo_hits:<5} | {fifo_misses:<6} | {(fifo_hits/(fifo_hits+fifo_misses)*100):.2f}%")
    print(f"{'MIN/OPT':<10} | {min_hits:<5} | {min_misses:<6} | {(min_hits/(min_hits+min_misses)*100):.2f}%")
    print(f"{'LRU':<10} | {lru_hits:<5} | {lru_misses:<6} | {(lru_hits/(lru_hits+lru_misses)*100):.2f}%")
    print(f"{'CLOCK':<10} | {clock_hits:<5} | {clock_misses:<6} | {(clock_hits/(clock_hits+clock_misses)*100):.2f}%")

    # BELADY'S FIFO (Anomaly Demo)
    print("\n")
    run_beladys_demo()

if __name__ == "__main__":
    main()