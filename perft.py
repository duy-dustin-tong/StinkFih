import chess
import sys
import time
from typing import Optional, Tuple

# Set recursion limit higher for deep searches
sys.setrecursionlimit(5000)

def _perft_recursive(board: chess.Board, depth: int) -> int:
    """
    The core recursive function to count nodes in the move tree.
    This replaces the unreliable board.perft() call.
    """
    # Base case: when depth is 0, we count this as one node (the current position).
    if depth == 0:
        return 1

    nodes = 0
    
    # Iterate over all legal moves from the current position
    for move in board.legal_moves:
        # Make the move
        board.push(move)
        
        # Recurse to the next depth level and sum the results
        nodes += _perft_recursive(board, depth - 1)
        
        # Undo the move (crucial for returning the board to its original state)
        board.pop()
        
    return nodes

def calculate_perft(fen_string: str, depth: int) -> Optional[Tuple[int, float]]:
    """
    Calculates the total number of legal moves (nodes) available from a 
    given FEN position up to a specified depth using a manual recursive method.

    Args:
        fen_string: The Forsyth-Edwards Notation string representing the position.
        depth: The maximum search depth (number of ply).

    Returns:
        A tuple of (node_count, duration_seconds) if successful, otherwise None.
    """
    try:
        # 1. Initialize the board from the FEN string
        board = chess.Board(fen_string)
    except ValueError as e:
        print(f"Error: Invalid FEN string provided. {e}")
        return None

    if depth < 0:
        print("Error: Depth must be a non-negative integer.")
        return None
    
    # 2. Start timing the calculation
    start_time = time.time()
    
    # 3. Calculate Perft using the custom recursive function
    try:
        # Special case: depth 0 is handled inside the recursive function
        node_count = _perft_recursive(board, depth)
    except Exception as e:
        print(f"An unexpected error occurred during Perft calculation: {e}")
        return None

    # 4. Stop timing
    duration = time.time() - start_time
    
    return node_count, duration

def main():
    """
    Main function to take user input and display the Perft results.
    """
    print("--- Chess Performance Test (Perft) Calculator ---")
    
    # Get FEN string input
    fen_input = input("Enter the FEN string (e.g., 'rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1' for starting position): ")
    
    # Get depth input
    try:
        depth_input = int(input("Enter the search depth (e.g., 4 or 5): "))
    except ValueError:
        print("Invalid depth. Please enter an integer.")
        return

    # Calculate and retrieve results
    result = calculate_perft(fen_input, depth_input)
    
    if result:
        node_count, duration = result
        
        print("\n--- Perft Results ---")
        print(f"Starting FEN: {fen_input}")
        print(f"Search Depth: {depth_input}")
        print(f"Total Nodes (Positions) Found: {node_count:,}")
        
        # Display performance metrics
        print(f"Time Taken: {duration:.2f} seconds")
        if duration > 0:
            nodes_per_second = node_count / duration
            print(f"Nodes per Second (NPS): {nodes_per_second:,.0f}")
        
        print("---------------------")

if __name__ == "__main__":
    main()