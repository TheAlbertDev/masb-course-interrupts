import pytest
import time
import RPi.GPIO as GPIO
import sys


def test_led_initially_off(setup_gpio):
    """Test that LED is initially off when the program starts."""
    
    # Monitor LED state for 5 seconds to ensure it stays consistently off
    try:
        initial_led_state = GPIO.input(17)
        print(f"Initial LED state (GPIO 17): {initial_led_state}")
    except Exception as e:
        pytest.fail(f"Failed to read GPIO 17: {e}. Make sure RPi.GPIO is properly set up.")
    
    # Monitor LED for 5 seconds to detect any unexpected blinking
    transitions = 0
    led_states = []
    last_state = initial_led_state
    start_time = time.time()
    monitoring_duration = 5.0
    
    print(f"Monitoring LED for {monitoring_duration} seconds to ensure it stays off...")
    
    while time.time() - start_time < monitoring_duration:
        current_state = GPIO.input(17)
        led_states.append(current_state)
        
        if current_state != last_state:
            transitions += 1
            transition_time = time.time() - start_time
            print(f"Unexpected transition at {transition_time:.3f}s: {last_state} -> {current_state}")
            last_state = current_state
        
        time.sleep(0.01)  # Check every 10ms
    
    final_state = GPIO.input(17)
    print(f"Final LED state after {monitoring_duration}s: {final_state}")
    print(f"Total transitions detected: {transitions}")
    
    # LED should have no transitions (stay consistently off)
    assert transitions == 0, f"LED should not blink initially, but detected {transitions} transitions"
    
    # LED should be off (LOW) throughout the monitoring period
    assert final_state == GPIO.LOW, f"LED should be off (LOW) initially, but found {final_state}"
    
    print("✓ LED is correctly and consistently off initially")


def test_led_starts_blinking_on_first_button_press(setup_gpio):
    """Test that LED starts blinking every 500ms after first button press."""
    
    # Verify LED is initially off
    initial_state = GPIO.input(17)
    print(f"Initial LED state: {initial_state}")
    
    # Simulate first button press
    print("Simulating first button press...")
    GPIO.output(27, GPIO.LOW)
    time.sleep(0.2)  # Hold button for 0.2 seconds
    GPIO.output(27, GPIO.HIGH)
    
    # Wait a moment for the system to react
    time.sleep(0.2)
    
    # Monitor LED for blinking pattern over 3 seconds
    led_transitions = []
    last_led_state = GPIO.input(17)
    start_time = time.time()
    monitoring_duration = 3.0  # Monitor for 3 seconds
    
    print(f"Monitoring LED for {monitoring_duration} seconds after button press...")
    
    while time.time() - start_time < monitoring_duration:
        current_led_state = GPIO.input(17)
        
        if current_led_state != last_led_state:
            transition_time = time.time() - start_time
            led_transitions.append({
                'time': transition_time,
                'from': last_led_state,
                'to': current_led_state
            })
            print(f"LED transition at {transition_time:.3f}s: {last_led_state} -> {current_led_state}")
            last_led_state = current_led_state
        
        time.sleep(0.01)  # Small delay to avoid excessive CPU usage
    
    print(f"Detected {len(led_transitions)} LED transitions")
    
    # Check if LED is blinking (should have multiple transitions)
    assert len(led_transitions) >= 4, f"Expected at least 4 transitions (2 blinks) in 3 seconds, but got {len(led_transitions)}"
    
    # Calculate intervals between transitions to verify ~500ms timing
    intervals = []
    for i in range(1, len(led_transitions)):
        interval = led_transitions[i]['time'] - led_transitions[i-1]['time']
        intervals.append(interval)
    
    # Check that intervals are approximately 500ms (0.5s) with some tolerance
    expected_interval = 0.5
    tolerance = 0.15  # Allow 150ms tolerance
    
    valid_intervals = [interval for interval in intervals if abs(interval - expected_interval) <= tolerance]
    print(f"Intervals: {[round(i, 3) for i in intervals]}")
    print(f"Valid intervals (~500ms ± {tolerance*1000}ms): {len(valid_intervals)}/{len(intervals)}")
    
    # At least 50% of intervals should be close to 500ms
    assert len(valid_intervals) >= len(intervals) * 0.5, f"Less than 50% of intervals are close to 500ms"
    
    print("✓ LED is blinking at approximately 500ms intervals after first button press")


def test_led_stops_blinking_on_second_button_press(setup_gpio):
    """Test that LED stops blinking and turns off after second button press."""
    
    # First button press to start blinking
    print("First button press - should start blinking...")
    GPIO.output(27, GPIO.LOW)
    time.sleep(0.2)
    GPIO.output(27, GPIO.HIGH)
    time.sleep(1)  # Wait for blinking to start
    
    # Verify blinking is active by checking for transitions
    transitions_before = 0
    last_state = GPIO.input(17)
    check_start = time.time()
    
    while time.time() - check_start < 1.5:  # Check for 1.5 seconds
        current_state = GPIO.input(17)
        if current_state != last_state:
            transitions_before += 1
            last_state = current_state
        time.sleep(0.01)
    
    print(f"Transitions detected while blinking: {transitions_before}")
    assert transitions_before >= 2, "LED should be blinking after first button press"
    
    # Second button press to stop blinking
    print("Second button press - should stop blinking...")
    GPIO.output(27, GPIO.LOW)
    time.sleep(0.2)
    GPIO.output(27, GPIO.HIGH)
    time.sleep(0.5)  # Wait for system to react
    
    # Monitor LED for 2 seconds to verify it stops blinking and stays off
    transitions_after = 0
    led_states = []
    last_state = GPIO.input(17)
    check_start = time.time()
    
    while time.time() - check_start < 2.0:  # Check for 2 seconds
        current_state = GPIO.input(17)
        led_states.append(current_state)
        if current_state != last_state:
            transitions_after += 1
            print(f"Transition at {time.time() - check_start:.3f}s: {last_state} -> {current_state}")
            last_state = current_state
        time.sleep(0.01)
    
    print(f"Transitions after second button press: {transitions_after}")
    
    # LED should stop blinking (no or very few transitions)
    assert transitions_after <= 1, f"LED should stop blinking after second button press, but detected {transitions_after} transitions"
    
    # LED should be off (final state should be LOW)
    final_state = GPIO.input(17)
    print(f"Final LED state: {final_state}")
    assert final_state == GPIO.LOW, f"LED should be off (LOW) after stopping, but found {final_state}"
    
    print("✓ LED correctly stops blinking and turns off after second button press")


def test_led_toggle_between_blinking_and_off(setup_gpio):
    """Test the complete cycle: off -> blinking -> off -> blinking."""
    
    # Helper function to count transitions
    def count_transitions(duration):
        transitions = 0
        last_state = GPIO.input(17)
        start = time.time()
        
        while time.time() - start < duration:
            current_state = GPIO.input(17)
            if current_state != last_state:
                transitions += 1
                last_state = current_state
            time.sleep(0.01)
        
        return transitions
    
    # Initial state should be off
    initial_state = GPIO.input(17)
    print(f"Initial LED state: {initial_state}")
    assert initial_state == GPIO.LOW, "LED should initially be off"
    
    # First press: should start blinking
    print("Press 1: Starting blinking...")
    GPIO.output(27, GPIO.LOW)
    time.sleep(0.2)
    GPIO.output(27, GPIO.HIGH)
    time.sleep(1)
    
    transitions_1 = count_transitions(1.5)
    print(f"Transitions during first blinking period: {transitions_1}")
    assert transitions_1 >= 2, "LED should be blinking after first press"
    
    # Second press: should stop blinking
    print("Press 2: Stopping blinking...")
    GPIO.output(27, GPIO.LOW)
    time.sleep(0.2)
    GPIO.output(27, GPIO.HIGH)
    time.sleep(0.5)
    
    transitions_2 = count_transitions(1.5)
    final_state_1 = GPIO.input(17)
    print(f"Transitions after second press: {transitions_2}")
    print(f"LED state after stopping: {final_state_1}")
    assert transitions_2 <= 1, "LED should stop blinking after second press"
    assert final_state_1 == GPIO.LOW, "LED should be off after stopping"
    
    # Third press: should start blinking again
    print("Press 3: Starting blinking again...")
    GPIO.output(27, GPIO.LOW)
    time.sleep(0.2)
    GPIO.output(27, GPIO.HIGH)
    time.sleep(1)
    
    transitions_3 = count_transitions(1.5)
    print(f"Transitions during second blinking period: {transitions_3}")
    assert transitions_3 >= 2, "LED should be blinking again after third press"
    
    print("✓ LED successfully toggles between blinking and off states")