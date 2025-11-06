# communicator.py

import requests
import json
import sys 
import subprocess 


BASE_URL = "https://lichess.org/api"
api_token = ''

headers = {
    "Authorization": f"Bearer {api_token}",
}

engine_process = None


# --- Engine Management Functions ---

def start_engine():
    """Starts the persistent C++ engine process."""
    global engine_process
    engine_command = "./engine.exe"
    
    
    # Use Popen to keep the process running and establish pipes for communication
    engine_process = subprocess.Popen(
        engine_command,
        stdin=subprocess.PIPE, 
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True, # Important for easy string communication
        bufsize=1  # Line buffered output
    )
    print(f"\n--- C++ Engine started (PID: {engine_process.pid}) ---")
    return True

        
def communicate_with_engine(command: str) -> str:
    """Sends a command (like 'newgame' or 'move e7e5') to the persistent C++ engine and reads its move response."""
    
    
    engine_process.stdin.write(command + '\n')
    engine_process.stdin.flush()
    
    
    engine_move = engine_process.stdout.readline().strip().lower()
    print('engine says: ', engine_move)
    return engine_move
            
    



def play_move(game_id, move):
    print(move, 'made_by_engine')
    endpoint = f"/bot/game/{game_id}/move/{move}"
    url = BASE_URL + endpoint
    response = requests.post(url, headers = headers)
    response.raise_for_status()

def play_game(game_id, isWhite):
    endpoint = f"/bot/game/stream/{game_id}"
    url = BASE_URL + endpoint
    
    response = requests.get(url, headers=headers, stream=True)
    for line in response.iter_lines():
        if line:
            data = json.loads(line)
            # print(data)
            state = data
            if 'id' in data.keys():
                state = data['state']
            print(state['moves'])
            print('-'*20)
            moves = state['moves'].split()
            if (len(moves)+isWhite)%2==0:
                continue
            
            moveToEngine = 'aaaa'
            if len(moves)>0: 
                moveToEngine = moves[-1]
            input_move = communicate_with_engine(moveToEngine)
            play_move(game_id, input_move)


def stream_event():
    endpoint = f"/stream/event"
    url = BASE_URL + endpoint
    response = requests.get(url, headers=headers, stream=True)
    for line in response.iter_lines():
        if line:
            data = json.loads(line)
            if('type' in data.keys() and data['type'] == 'gameStart'):
                game_id = data['game']['gameId']
                isWhite = 1 if (data['game']['color'] == 'white') else 0
                return (game_id, isWhite)
    
def main():
    if not start_engine():
        return
    game_id, isWhite = stream_event()
    color = 'white' if isWhite else 'black'
    engine_process.stdin.write(color + '\n')
    play_game(game_id, isWhite)

if __name__ == '__main__':
    with open('lichess_token.txt', 'r') as f:
        api_token = f.readline()
        api_token.strip()
    headers = {
        "Authorization": f"Bearer {api_token}",
    }
    # print(api_token)
    main()