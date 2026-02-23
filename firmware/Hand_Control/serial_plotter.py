import serial
import threading
import time
import collections
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# --- Configuration ---
PORT = "COM3"
BAUD = 115200
WINDOW_SIZE = 10  # Seconds to show on the X-axis

# Deques for the scrolling graphs
times = collections.deque(maxlen=2000)
raw_data = collections.deque(maxlen=2000)
wl_data = collections.deque(maxlen=2000)
thresh_data = collections.deque(maxlen=2000)
counter_data = collections.deque(maxlen=2000)

# Dictionary for the instant LED status
current_status = {"Detection": 0.0, "Disconnected": 0.0, "TimedOut": 0.0}

is_running = True
start_time = time.time()


# ==========================================
# THREAD 1: Data Acquisition
# ==========================================
def read_serial_data():
    try:
        arduino = serial.Serial(port=PORT, baudrate=BAUD, timeout=0.1)
        time.sleep(2)
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return

    while is_running:
        if arduino.in_waiting > 0:
            try:
                raw_line = arduino.readline().decode("utf-8").strip(", \r\n")
                if not raw_line:
                    continue

                data = {
                    k: float(v)
                    for pair in raw_line.split(",")
                    if ":" in pair
                    for k, v in [pair.split(":")]
                }

                # 1. Update LEDs directly
                if "Detection" in data:
                    current_status["Detection"] = data["Detection"]
                if "Disconnected" in data:
                    current_status["Disconnected"] = data["Disconnected"]
                if "TimedOut" in data:
                    current_status["TimedOut"] = data["TimedOut"]

                # 2. Update Graph History
                current_t = time.time() - start_time
                times.append(current_t)
                raw_data.append(data.get("Raw", 0))
                wl_data.append(data.get("WL", 0))
                thresh_data.append(data.get("Threshold", 0))
                counter_data.append(data.get("Counter", 0))

            except (ValueError, UnicodeDecodeError):
                pass

    arduino.close()


data_thread = threading.Thread(target=read_serial_data, daemon=True)
data_thread.start()

# ==========================================
# THREAD 2: Plot Rendering
# ==========================================
fig = plt.figure(figsize=(10, 9))
fig.canvas.manager.set_window_title("Arduino Hybrid Dashboard")

# Use GridSpec to customize the layout
gs = fig.add_gridspec(4, 1, height_ratios=[1, 2, 2, 2])

# Row 0: Status Lights (Independent Axes)
ax_lights = fig.add_subplot(gs[0, 0])
ax_lights.axis("off")
ax_lights.set_xlim(0, 1)
ax_lights.set_ylim(0, 1)

(light_det,) = ax_lights.plot(
    [0.2], [0.6], marker="o", markersize=35, color="lightgray"
)
ax_lights.text(0.2, 0.2, "Detection", ha="center", fontsize=12, fontweight="bold")

(light_disc,) = ax_lights.plot(
    [0.5], [0.6], marker="o", markersize=35, color="lightgray"
)
ax_lights.text(0.5, 0.2, "Disconnected", ha="center", fontsize=12, fontweight="bold")

(light_tout,) = ax_lights.plot(
    [0.8], [0.6], marker="o", markersize=35, color="lightgray"
)
ax_lights.text(0.8, 0.2, "Timed Out", ha="center", fontsize=12, fontweight="bold")

# Row 1: Raw Data
ax_raw = fig.add_subplot(gs[1, 0])
(line_raw,) = ax_raw.plot([], [], color="purple")
ax_raw.set_ylabel("Raw")
ax_raw.set_ylim(0, 615)
ax_raw.grid(True, linestyle=":", alpha=0.7)

# Row 2: WL and Threshold (Shares X with Raw)
ax_wl = fig.add_subplot(gs[2, 0], sharex=ax_raw)
(line_wl,) = ax_wl.plot([], [], label="WL", color="blue")
(line_thresh,) = ax_wl.plot([], [], label="Threshold", color="red", linestyle="--")
ax_wl.set_ylabel("WL / Thresh")
ax_wl.legend(loc="upper left")
ax_wl.grid(True, linestyle=":", alpha=0.7)

# Row 3: Counter (Shares X with Raw)
ax_counter = fig.add_subplot(gs[3, 0], sharex=ax_raw)
(line_counter,) = ax_counter.plot([], [], color="teal")
ax_counter.set_ylabel("Counter")
ax_counter.set_ylim(0, 2000)
ax_counter.set_xlabel("Time (seconds)")
ax_counter.grid(True, linestyle=":", alpha=0.7)


def update_dashboard(frame):
    # 1. Update Lights
    light_det.set_color(
        "#00FF00" if current_status["Detection"] >= 1000 else "lightgray"
    )
    light_disc.set_color(
        "#FF0000" if current_status["Disconnected"] >= 1000 else "lightgray"
    )
    light_tout.set_color(
        "#FFA500" if current_status["TimedOut"] >= 1000 else "lightgray"
    )

    if not times:
        return

    # 2. Update Lines
    line_raw.set_data(times, raw_data)
    line_wl.set_data(times, wl_data)
    line_thresh.set_data(times, thresh_data)
    line_counter.set_data(times, counter_data)

    # 3. Calculate 10-second sliding window
    current_time = times[-1]
    min_x = max(0, current_time - WINDOW_SIZE)
    max_x = max(WINDOW_SIZE, current_time)

    # We only need to set xlim on ax_raw because ax_wl and ax_counter share it
    ax_raw.set_xlim(min_x, max_x)

    # Let the WL/Threshold plot dynamically adjust its Y-axis
    ax_wl.relim()
    ax_wl.autoscale_view(scalex=False, scaley=True)

    return (
        line_raw,
        line_wl,
        line_thresh,
        line_counter,
        light_det,
        light_disc,
        light_tout,
    )


def on_close(event):
    global is_running
    is_running = False


fig.canvas.mpl_connect("close_event", on_close)

# Run animation at 20 FPS (50ms)
ani = FuncAnimation(
    fig, update_dashboard, blit=False, interval=50, cache_frame_data=False
)

plt.tight_layout()
plt.show()

data_thread.join()
