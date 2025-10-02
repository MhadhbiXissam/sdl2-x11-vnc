#!/bin/bash
# Clear the terminal
clear

# Configuration
DISPLAY_NUM=:99         # Virtual display number
SCREEN_WIDTH=1024
SCREEN_HEIGHT=768
COLOR_DEPTH=16
SCREEN_RES="${SCREEN_WIDTH}x${SCREEN_HEIGHT}x${COLOR_DEPTH}"

# Function to clean up Xvfb on exit
cleanup() {
    echo "Stopping virtual display..."
    if [[ -n "$XVFB_PID" ]]; then
        kill $XVFB_PID
        wait $XVFB_PID 2>/dev/null
    fi
    exit
}

# Trap EXIT, SIGINT, SIGTERM to ensure cleanup
trap cleanup EXIT SIGINT SIGTERM

# Start Xvfb in the background
Xvfb $DISPLAY_NUM -screen 0 $SCREEN_RES &
XVFB_PID=$!
echo "Started Xvfb on display $DISPLAY_NUM with PID $XVFB_PID"

# Give Xvfb a moment to start
sleep 1

# Export the display so apps know where to render
export DISPLAY=$DISPLAY_NUM
echo "DISPLAY set to $DISPLAY"



# Run your app (replace xterm with any GUI app)
xterm
# When xterm exits, cleanup will run automatically because of trap
