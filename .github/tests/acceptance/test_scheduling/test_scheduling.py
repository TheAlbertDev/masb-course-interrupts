import pytest
import time
import RPi.GPIO as GPIO
import sys

def test_gpio_17_toggles_every_second(setup_gpio):
    """Test that GPIO pin 17 toggles every second."""
    
    # Print GPIO information for debugging
    print(f"Testing with RPi.GPIO version: {GPIO.VERSION}")
    print(f"Running on Python: {sys.version}")
    
    # List to store the timestamps of transitions
    transitions = []
    
    # Initial state - handle potential errors
    try:
        last_state = GPIO.input(17)
        print(f"Initial GPIO 17 state: {last_state}")
    except Exception as e:
        pytest.fail(f"Failed to read GPIO 17: {e}. Make sure RPi.GPIO is properly set up.")
    
    # Monitor for 5 seconds (should see about 5 transitions)
    start_time = time.time()
    timeout = start_time + 5
    
    while time.time() < timeout:
        try:
            current_state = GPIO.input(17)
            
            # If state changed, record the timestamp
            if current_state != last_state:
                transition_time = time.time()
                transitions.append(transition_time)
                print(f"Transition detected at {transition_time-start_time:.2f}s: {last_state} -> {current_state}")
                last_state = current_state
        except Exception as e:
            print(f"Error reading GPIO: {e}")
            time.sleep(0.1)  # Longer delay on error
        
        # Small delay to avoid hogging CPU
        time.sleep(0.01)
    
    # Calculate time between transitions
    intervals = []
    for i in range(1, len(transitions)):
        interval = transitions[i] - transitions[i-1]
        intervals.append(interval)
    
    # Check if we have enough transitions
    assert len(transitions) >= 4, f"Not enough transitions detected: {len(transitions)}"
    
    # Calculate average interval
    avg_interval = sum(intervals) / len(intervals)
    
    # Print for debugging
    print(f"Detected {len(transitions)} transitions")
    print(f"Average interval: {avg_interval:.3f} seconds")
    print(f"Intervals: {[round(i, 3) for i in intervals]}")
    
    # Check if the average interval is close to 1 second (allowing 0.2 seconds tolerance)
    assert 0.8 <= avg_interval <= 1.2, f"Average toggle interval {avg_interval:.3f} is not close to 1 second"