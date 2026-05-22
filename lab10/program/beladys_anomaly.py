from algorithms import simulate_fifo

def run_demo():
    print("==================================================")
    print("Belady's Anomaly ")
    print("Reference Sequence: [1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5]")
    print("==================================================")

    classic_sequence = [1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5]

    # 1. Simulate FIFO with N = 3
    hits_3, misses_3 = simulate_fifo(3, classic_sequence)
    print(f"Totals for N=3: Hits={hits_3}, Misses={misses_3}")

    # 2. Simulate FIFO with N = 4
    hits_4, misses_4 = simulate_fifo(4, classic_sequence)
    print(f"Totals for N=4: Hits={hits_4}, Misses={misses_4}")

    # Summary Analysis Block
    print("\n--- Anomaly Verification Summary ---")
    print(f"Misses with N=3 Frames: {misses_3}")
    print(f"Misses with N=4 Frames: {misses_4}")

    if misses_4 > misses_3:
        print(f"More resources less performanc, tell that to Sam Altman {misses_3} to {misses_4}.")
    else:
        print("\nDemo failed to show the anomaly.")

if __name__ == "__main__":
    run_demo()
