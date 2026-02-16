import pytest
import time
import RPi.GPIO as GPIO
import subprocess

@pytest.fixture(scope="function")
def setup_gpio():
    """Setup and teardown for GPIO testing."""
    # Set the pin numbering mode (also set in each test for safety)
    GPIO.setwarnings(False)  # Disable warnings about channels in use
    try:
        GPIO.setmode(GPIO.BCM)
    except:
        # If already set, this might throw an error - ignore it
        pass
    
    # GPIO 17 will be set as an input (no pull-up/down as it uses push-pull driver)
    GPIO.setup(17, GPIO.IN)
    
    # Reset the system before each test using OpenOCD
    subprocess.run([
        "pio", "pkg", "exec", "-p", "tool-openocd", "-c",
        "openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c 'init; reset run; shutdown'"
    ], check=True)
    time.sleep(0.5)  # Wait for system to initialize
    
    yield

    # Clean up GPIO settings after tests
    GPIO.cleanup()