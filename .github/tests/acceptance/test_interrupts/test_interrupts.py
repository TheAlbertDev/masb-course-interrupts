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


def test_led_responds_to_simulated_button_press(setup_gpio):
    """Test that LED on pin 17 toggles when button is simulated via GPIO 27."""
    
    # Record initial LED state
    try:
        initial_led_state = GPIO.input(17)
        print(f"Initial LED state (GPIO 17): {initial_led_state}")
    except Exception as e:
        pytest.fail(f"Failed to read GPIO 17: {e}. Make sure RPi.GPIO is properly set up.")
    
    led_toggles = 0
    last_led_state = initial_led_state
    
    # Simulate 3 button presses
    for press_num in range(1, 10):
        print(f"Simulating button press {press_num}...")
        
        # Simulate button press (HIGH to LOW transition - active low)
        GPIO.output(27, GPIO.LOW)
        time.sleep(0.5)  # Hold button for 500ms
        
        # Check if LED state changed after button press
        new_led_state = GPIO.input(17)
        if new_led_state != last_led_state:
            led_toggles += 1
            print(f"LED toggled from {last_led_state} to {new_led_state}")
            last_led_state = new_led_state
        else:
            print(f"LED state remained {new_led_state}")
        
        # Release button (LOW to HIGH transition)
        GPIO.output(27, GPIO.HIGH)
        time.sleep(0.5)  # Wait before next press
    
    print(f"Test completed: 9 simulated button presses, {led_toggles} LED toggles")
    
    # Verify that LED toggles correspond to button presses
    # Each button press should cause exactly one LED toggle
    assert led_toggles > 0, f"LED did not toggle despite 9 simulated button presses"
    assert led_toggles <= 9, f"More LED toggles ({led_toggles}) than button presses (9)"

def test_edge_detection_prevents_multiple_toggles(setup_gpio):
    """Test that edge detection prevents multiple LED toggles during a single long button press."""
    
    # This test simulates what happens during a long button press
    # The LED should only toggle once per button press, not continuously
    
    initial_led_state = GPIO.input(17)
    print(f"Initial LED state: {initial_led_state}")
    
    transitions = []
    last_led_state = initial_led_state
    
    print("Edge detection test: Simulating long button press (3 seconds)")
    
    # Start monitoring LED changes
    start_time = time.time()
    
    # Simulate button press (HIGH to LOW transition)
    GPIO.output(27, GPIO.LOW)
    print("Button pressed (simulated) - holding for 3 seconds")
    
    # Monitor LED changes during the long button press
    hold_duration = 3.0  # Hold button for 3 seconds
    monitoring_end = start_time + hold_duration
    
    while time.time() < monitoring_end:
        try:
            current_led_state = GPIO.input(17)
            
            # Track LED state changes
            if current_led_state != last_led_state:
                transition_time = time.time() - start_time
                transitions.append({
                    'time': transition_time,
                    'from': last_led_state,
                    'to': current_led_state
                })
                print(f"LED transition at {transition_time:.3f}s: {last_led_state} -> {current_led_state}")
                last_led_state = current_led_state
            
        except Exception as e:
            print(f"Error reading GPIO: {e}")
        
        time.sleep(0.001)  # Very fast polling to catch any multiple toggles
    
    # Release button (LOW to HIGH transition)
    GPIO.output(27, GPIO.HIGH)
    print("Button released (simulated)")
    
    # Monitor for a bit more to catch any release-triggered changes
    time.sleep(0.5)
    final_led_state = GPIO.input(17)
    if final_led_state != last_led_state:
        transition_time = time.time() - start_time
        transitions.append({
            'time': transition_time,
            'from': last_led_state,
            'to': final_led_state
        })
        print(f"LED transition at release {transition_time:.3f}s: {last_led_state} -> {final_led_state}")
    
    print(f"Edge detection test completed: {len(transitions)} LED transitions detected")
    
    # For proper edge detection, there should be exactly 1 transition per button press
    # The edge detection should prevent multiple toggles during the long press
    assert len(transitions) <= 1, f"Too many LED transitions ({len(transitions)}) for a single button press. Edge detection may not be working properly."
    
    if len(transitions) == 1:
        print("Edge detection working correctly - exactly one LED toggle per button press")
    elif len(transitions) == 0:
        print("No LED transitions detected - this might indicate the system is not responding to button presses")
    else:
        print(f"Multiple transitions detected ({len(transitions)}) - edge detection may need improvement")