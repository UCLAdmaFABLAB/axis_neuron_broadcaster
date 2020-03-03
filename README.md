# AXIS Neuron Broadcaster

## Broadcasts live data from AXIS Neuron motion capture system to OSC clients

### Overview

This tool was designed to consume data sent by the Axis Neuron Studio software and transmit it over a network to an OSC client like PureData, Processing, or supercollider.

### Installation

The x64 binary is available [here](https://github.com/ucladmafablab/axis_neuron_broadcaster/archive/0.1.1.zip).

To build this software, you will need Visual Studio 2019 with the C++ MFC v142 toolset. You will also need to add the included NDRLib Directory to *Additional Include Directories*.

### Setup

By default, we connect to BVH (raw) data stream on port 7001 and Calculated data on port 7003 over TCP. Please enable these outputs under the *Broadcasting* tab in Axis Neuron Settings.

![AXIS Neuron Settings](/doc/axis_settings.png)

When a motion capture suit is connected, or the software is playing back a recorded scene, this tool will recieve and transmit live data. To initiate the connection, click "Connect TCP" next to the port number 7001. If the Neuron software is currently broadcasting, Displacement and Angle data should be visible for the selected Bone number.

Now click "Connect Calc" next to port 7003 to connect to the Calculated data stream. You should now see live data next to "Calc Pos" and "Calc Acc." Both BVH and Calculated data can be consumed at once.

![Broadcaster Dialog](/doc/broadcaster_dialog.png)

### Output

OSC data is broadcast on UDP port 8000. Assuming your firewall and router do not block such connections, you can listen for this data on any machine on your local subnet. With a typical OSC library, you should be able to simply listen/dump on port 8000 and start seeing data.

To avoid overwhelming networks and receiving programs, the program is set up to throttle data. BVH and calculated data will only be sent every 0.5 seconds. This value can be changed by adjusting `dataDelayS` in `demo_MFCDlg.h`.

### Format

6 data points are transmitted as (x, y, z) floats for each of the 59 bone positions, where `$n` in the route is the bone number.

| Route | Description | 0 | 1 | 2 |
| ----- | ----------- | - | - | - |
| `/b/$n/d` | Displacement (BVH) | x | y | z |
| `/b/$n/a` | Angle (BVH) | x | y | z |
| `/b/$n/p` | Position | x | y | z |
| `/b/$n/v` | Velocity | x | y | z |
| `/b/$n/A` | Acceleration | x | y | z |

